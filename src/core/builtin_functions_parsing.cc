/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "builtin_functions_parsing.h"
#include "core/Arguments.h"
#include "core/Builtins.h"
#include "core/function.h"
#include "core/Parameters.h"
#include "utils/printutils.h"

#include <string>
#include <vector>
#include <utility>

// Helper structure to track delimiter pairs
struct DelimiterPair {
  char open;
  char close;
  
  DelimiterPair(char o, char c) : open(o), close(c) {}
};

// Map of opening delimiters to closing delimiters (excluding unicode for now)
static const std::vector<DelimiterPair> DELIMITER_PAIRS = {
  {'{', '}'},
  {'[', ']'},
  {'<', '>'},
  {'(', ')'},
  {'\'', '\''},
  {'"', '"'}
};

static char get_closing_delimiter(char open_delim)
{
  for (const auto& pair : DELIMITER_PAIRS) {
    if (pair.open == open_delim) {
      return pair.close;
    }
  }
  return '\0';
}

// Helper to find matching closing delimiter
static size_t find_matching_close(const std::string& str, size_t start, char open_delim, char close_delim)
{
  int depth = 1;
  for (size_t i = start; i < str.length(); ++i) {
    if (str[i] == open_delim) {
      ++depth;
    } else if (str[i] == close_delim) {
      --depth;
      if (depth == 0) {
        return i;
      }
    }
  }
  return std::string::npos;
}

// =============================================================================
// Parsing Functions
// =============================================================================

/**
 * @brief Extract content between delimiter pairs from a string.
 * 
 * Supports nested delimiters of the same type.
 * Recognized delimiter pairs: {}, [], <>, (), '', ""
 * 
 * @param input String to parse (positional arg 0)
 * @param delims Delimiter character(s) to search for (positional arg 1, default "{")
 * @param pos Which match to return (positional arg 2, default 1)
 *            pos=0: return vector of all matches
 *            pos=N: return Nth match (1-indexed) as string, or undef if not found
 * @return String or vector of strings containing content between delimiters
 * 
 * OpenSCAD usage:
 *   parse_delimited(string) -> first match between {}
 *   parse_delimited(string, delims) -> first match between specified delimiters
 *   parse_delimited(string, delims, pos) -> Nth match or all matches
 * Example: parse_delimited("a {b} c {d}", "{", 1) returns "b"
 * Example: parse_delimited("a {b} c {d}", "{", 0) returns ["b", "d"]
 * Example: parse_delimited("say 'hello'", "'", 1) returns "hello"
 */
Value builtin_parse_delimited(Arguments arguments, const Location& loc)
{
  if (arguments.size() < 1 || arguments.size() > 3) {
    print_argCnt_warning("parse_delimited", arguments.size(), "1 to 3", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "parse_delimited() requires first argument to be a string");
    return Value::undefined.clone();
  }

  std::string input = arguments[0]->toStrUtf8Wrapper().toString();
  
  // Default delimiter is "{"
  std::string delims = "{";
  if (arguments.size() >= 2) {
    if (arguments[1]->type() != Value::Type::STRING) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "parse_delimited() requires second argument to be a string");
      return Value::undefined.clone();
    }
    delims = arguments[1]->toStrUtf8Wrapper().toString();
  }
  
  if (delims.empty()) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "parse_delimited() delimiter string cannot be empty");
    return Value::undefined.clone();
  }

  char open_delim = delims[0];
  char close_delim = get_closing_delimiter(open_delim);
  
  if (close_delim == '\0') {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "parse_delimited() delimiter '%1$c' is not recognized", open_delim);
    return Value::undefined.clone();
  }

  // Default pos is 1 (return first match as string)
  double pos_val = 1.0;
  if (arguments.size() >= 3) {
    if (arguments[2]->type() != Value::Type::NUMBER) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "parse_delimited() requires third argument to be a number");
      return Value::undefined.clone();
    }
    pos_val = arguments[2]->toDouble();
  }

  // Find all matches
  std::vector<std::string> matches;
  size_t search_pos = 0;
  
  while (search_pos < input.length()) {
    size_t open_pos = input.find(open_delim, search_pos);
    if (open_pos == std::string::npos) break;
    
    size_t close_pos = find_matching_close(input, open_pos + 1, open_delim, close_delim);
    if (close_pos == std::string::npos) break;
    
    // Extract content between delimiters (excluding the delimiters themselves)
    std::string content = input.substr(open_pos + 1, close_pos - open_pos - 1);
    matches.push_back(content);
    
    search_pos = close_pos + 1;
  }

  // pos=0 means return all matches as vector
  if (pos_val == 0.0) {
    VectorType result(arguments.session());
    for (const auto& match : matches) {
      result.emplace_back(match);
    }
    return std::move(result);
  }
  
  // pos=N means return Nth match (1-indexed)
  int index = static_cast<int>(pos_val) - 1;
  if (index >= 0 && index < static_cast<int>(matches.size())) {
    return {matches[index]};
  }
  
  return Value::undefined.clone();
}

/**
 * @brief Parse all delimiter types recursively (STUB - not fully implemented).
 * 
 * Intended to parse nested structures with mixed delimiters ({}, [], <>, (), '', "", «»)
 * returning a nested vector preserving structure and whitespace.
 * 
 * Current implementation: Returns input wrapped in vector with warning.
 * Full recursive implementation deferred due to complexity.
 * 
 * @param input String to parse (positional arg 0)
 * @return Nested vector structure (currently stub)
 * 
 * OpenSCAD usage: parse_delimited_all(string) -> nested vector
 * Example (intended): parse_delimited_all("a {b [c] d} e") -> ["a ", ["{", "b ", ["[", "c"], " d"], " e"]
 * Note: Use parse_delimited() for single delimiter type parsing
 */
Value builtin_parse_delimited_all(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("parse_delimited_all", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "parse_delimited_all() requires a string argument");
    return Value::undefined.clone();
  }

  // This is a complex function that returns nested structure
  // For now, implementing a simplified version that handles one level of nesting
  // Full implementation would require recursive parsing
  
  std::string input = arguments[0]->toStrUtf8Wrapper().toString();
  VectorType result(arguments.session());
  
  // TODO: Full recursive implementation
  // For now, return the input wrapped in a vector as placeholder
  LOG(message_group::Warning, loc, arguments.documentRoot(),
      "parse_delimited_all() is not fully implemented yet");
  result.emplace_back(input);
  
  return std::move(result);
}

/**
 * @brief Parse key-value pairs from delimited string.
 * 
 * Splits string by item delimiter, then splits each item by pair delimiter.
 * Trims whitespace from keys and values.
 * 
 * @param input String containing key-value pairs (positional arg 0)
 * @param pair_delim Delimiter between key and value (positional arg 1, default "=")
 * @param item_delim Delimiter between pairs (positional arg 2, default ",")
 * @return Vector of [key, value] pairs: [[key1, value1], [key2, value2], ...]
 * 
 * OpenSCAD usage:
 *   parse_pairs(string) -> [[key, value], ...] using = and ,
 *   parse_pairs(string, pair_delim) -> [[key, value], ...] using custom pair delimiter
 *   parse_pairs(string, pair_delim, item_delim) -> [[key, value], ...] using both custom delimiters
 * Example: parse_pairs("width=100, height=200") returns [["width", "100"], ["height", "200"]]
 * Example: parse_pairs("a:1;b:2", ":", ";") returns [["a", "1"], ["b", "2"]]
 * Example: parse_pairs("x=10&y=20", "=", "&") returns [["x", "10"], ["y", "20"]]
 */
Value builtin_parse_pairs(Arguments arguments, const Location& loc)
{
  if (arguments.size() < 1 || arguments.size() > 3) {
    print_argCnt_warning("parse_pairs", arguments.size(), "1 to 3", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "parse_pairs() requires first argument to be a string");
    return Value::undefined.clone();
  }

  std::string input = arguments[0]->toStrUtf8Wrapper().toString();
  
  // Default delimiters
  std::string pair_delim = "=";
  std::string item_delim = ",";
  
  if (arguments.size() >= 2) {
    if (arguments[1]->type() != Value::Type::STRING) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "parse_pairs() requires second argument to be a string");
      return Value::undefined.clone();
    }
    pair_delim = arguments[1]->toStrUtf8Wrapper().toString();
  }
  
  if (arguments.size() >= 3) {
    if (arguments[2]->type() != Value::Type::STRING) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "parse_pairs() requires third argument to be a string");
      return Value::undefined.clone();
    }
    item_delim = arguments[2]->toStrUtf8Wrapper().toString();
  }

  VectorType result(arguments.session());
  
  // Split by item delimiter
  size_t start = 0;
  size_t end;
  
  while (start < input.length()) {
    end = input.find(item_delim, start);
    if (end == std::string::npos) {
      end = input.length();
    }
    
    std::string item = input.substr(start, end - start);
    
    // Trim whitespace from item
    size_t first = item.find_first_not_of(" \t\n\r");
    size_t last = item.find_last_not_of(" \t\n\r");
    
    if (first != std::string::npos && last != std::string::npos) {
      item = item.substr(first, last - first + 1);
      
      // Split by pair delimiter
      size_t pair_pos = item.find(pair_delim);
      if (pair_pos != std::string::npos) {
        std::string key = item.substr(0, pair_pos);
        std::string value = item.substr(pair_pos + pair_delim.length());
        
        // Trim key and value
        size_t key_first = key.find_first_not_of(" \t\n\r");
        size_t key_last = key.find_last_not_of(" \t\n\r");
        if (key_first != std::string::npos) {
          key = key.substr(key_first, key_last - key_first + 1);
        }
        
        size_t val_first = value.find_first_not_of(" \t\n\r");
        size_t val_last = value.find_last_not_of(" \t\n\r");
        if (val_first != std::string::npos) {
          value = value.substr(val_first, val_last - val_first + 1);
        }
        
        VectorType pair(arguments.session());
        pair.emplace_back(key);
        pair.emplace_back(value);
        result.emplace_back(std::move(pair));
      }
    }
    
    start = end + item_delim.length();
    if (end == input.length()) break;
  }
  
  return std::move(result);
}

// =============================================================================
// Registration
// =============================================================================

void register_builtin_parsing()
{
  Builtins::init("parse_delimited", new BuiltinFunction(&builtin_parse_delimited),
                 {
                   "parse_delimited(string, delims=\"{\", pos=1) -> string or vector",
                 });

  Builtins::init("parse_delimited_all", new BuiltinFunction(&builtin_parse_delimited_all),
                 {
                   "parse_delimited_all(string) -> nested vector",
                 });

  Builtins::init("parse_pairs", new BuiltinFunction(&builtin_parse_pairs),
                 {
                   "parse_pairs(string, pair_delim=\"=\", item_delim=\",\") -> [[key, value], ...]",
                 });
}
