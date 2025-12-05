/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2024 Clifford Wolf <clifford@clifford.at> and
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

#include "core/builtin_functions_strings.h"

#include "core/Arguments.h"
#include "core/Builtins.h"
#include "core/function.h"
#include "core/Parameters.h"
#include "core/Value.h"
#include "utils/printutils.h"

#include <glib.h>
#include <boost/algorithm/string.hpp>
#include <vector>

// =============================================================================
// Utility Functions (UTF-8 aware using GLib)
// =============================================================================

std::string string_upper(const std::string& input)
{
  if (input.empty()) {
    return input;
  }

  // GLib's g_utf8_strup handles full Unicode case conversion
  gchar* upper = g_utf8_strup(input.c_str(), -1);
  std::string result(upper);
  g_free(upper);
  return result;
}

std::string string_lower(const std::string& input)
{
  if (input.empty()) {
    return input;
  }

  // GLib's g_utf8_strdown handles full Unicode case conversion
  gchar* lower = g_utf8_strdown(input.c_str(), -1);
  std::string result(lower);
  g_free(lower);
  return result;
}

std::string string_trim(const std::string& input)
{
  // Boost's trim_copy returns a new string with leading/trailing whitespace removed
  return boost::algorithm::trim_copy(input);
}

std::string string_ltrim(const std::string& input)
{
  // Boost's trim_left_copy returns a new string with leading whitespace removed
  return boost::algorithm::trim_left_copy(input);
}

std::string string_rtrim(const std::string& input)
{
  // Boost's trim_right_copy returns a new string with trailing whitespace removed
  return boost::algorithm::trim_right_copy(input);
}

std::vector<std::string> string_split(const std::string& input, const std::string& delimiter)
{
  std::vector<std::string> result;
  if (input.empty()) {
    return result;
  }
  if (delimiter.empty()) {
    // Split into individual UTF-8 characters
    const char* p = input.c_str();
    const char* end = p + input.size();
    while (p < end) {
      const char* next = g_utf8_next_char(p);
      result.emplace_back(p, next - p);
      p = next;
    }
    return result;
  }
  boost::algorithm::split(result, input, boost::algorithm::is_any_of(delimiter));
  return result;
}

std::string string_join(const std::vector<std::string>& parts, const std::string& delimiter)
{
  return boost::algorithm::join(parts, delimiter);
}

std::string string_replace(const std::string& input, const std::string& old_str, const std::string& new_str)
{
  if (old_str.empty()) {
    return input;
  }
  return boost::algorithm::replace_all_copy(input, old_str, new_str);
}

bool string_starts_with(const std::string& input, const std::string& prefix)
{
  return boost::algorithm::starts_with(input, prefix);
}

bool string_ends_with(const std::string& input, const std::string& suffix)
{
  return boost::algorithm::ends_with(input, suffix);
}

bool string_contains(const std::string& input, const std::string& substr)
{
  return boost::algorithm::contains(input, substr);
}

std::string string_substr(const std::string& input, long start, long length)
{
  if (input.empty()) {
    return input;
  }

  // Get UTF-8 string length
  glong utf8_len = g_utf8_strlen(input.c_str(), -1);
  
  // Handle negative start (count from end)
  if (start < 0) {
    start = utf8_len + start;
  }
  
  // Clamp start to valid range
  if (start < 0) {
    start = 0;
  }
  if (start >= utf8_len) {
    return "";
  }

  // Calculate actual length
  glong actual_len = (length < 0) ? (utf8_len - start) : length;
  if (start + actual_len > utf8_len) {
    actual_len = utf8_len - start;
  }

  // Find start position in bytes
  const gchar* start_ptr = g_utf8_offset_to_pointer(input.c_str(), start);
  // Find end position in bytes
  const gchar* end_ptr = g_utf8_offset_to_pointer(start_ptr, actual_len);

  return std::string(start_ptr, end_ptr - start_ptr);
}

long string_index_of(const std::string& input, const std::string& substr)
{
  if (substr.empty()) {
    return 0;
  }
  
  const char* found = strstr(input.c_str(), substr.c_str());
  if (!found) {
    return -1;
  }
  
  // Count UTF-8 characters before the found position
  return g_utf8_strlen(input.c_str(), found - input.c_str());
}

std::string string_pad_left(const std::string& input, size_t length, char pad_char)
{
  glong utf8_len = g_utf8_strlen(input.c_str(), -1);
  if (static_cast<size_t>(utf8_len) >= length) {
    return input;
  }
  return std::string(length - utf8_len, pad_char) + input;
}

std::string string_pad_right(const std::string& input, size_t length, char pad_char)
{
  glong utf8_len = g_utf8_strlen(input.c_str(), -1);
  if (static_cast<size_t>(utf8_len) >= length) {
    return input;
  }
  return input + std::string(length - utf8_len, pad_char);
}

// =============================================================================
// Builtin Function Implementations
// =============================================================================

static Value builtin_upper(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("upper", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "upper() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_upper(arguments[0]->toStrUtf8Wrapper().toString())};
}

static Value builtin_lower(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("lower", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "lower() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_lower(arguments[0]->toStrUtf8Wrapper().toString())};
}

static Value builtin_trim(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("trim", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "trim() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_trim(arguments[0]->toStrUtf8Wrapper().toString())};
}

static Value builtin_ltrim(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("ltrim", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "ltrim() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_ltrim(arguments[0]->toStrUtf8Wrapper().toString())};
}

static Value builtin_rtrim(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("rtrim", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "rtrim() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_rtrim(arguments[0]->toStrUtf8Wrapper().toString())};
}

static Value builtin_split(Arguments arguments, const Location& loc)
{
  if (arguments.size() < 1 || arguments.size() > 2) {
    print_argCnt_warning("split", arguments.size(), "1 or 2", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "split() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string delimiter = "";
  if (arguments.size() == 2) {
    if (arguments[1]->type() != Value::Type::STRING) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "split() requires a string as delimiter, got %1$s", arguments[1]->typeName());
      return Value::undefined.clone();
    }
    delimiter = arguments[1]->toStrUtf8Wrapper().toString();
  }

  std::vector<std::string> parts = string_split(arguments[0]->toStrUtf8Wrapper().toString(), delimiter);
  
  VectorType result(arguments.session());
  for (const auto& part : parts) {
    result.emplace_back(part);
  }
  return Value{std::move(result)};
}

static Value builtin_join(Arguments arguments, const Location& loc)
{
  if (arguments.size() < 1 || arguments.size() > 2) {
    print_argCnt_warning("join", arguments.size(), "1 or 2", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::VECTOR) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "join() requires a vector as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string delimiter = "";
  if (arguments.size() == 2) {
    if (arguments[1]->type() != Value::Type::STRING) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "join() requires a string as delimiter, got %1$s", arguments[1]->typeName());
      return Value::undefined.clone();
    }
    delimiter = arguments[1]->toStrUtf8Wrapper().toString();
  }

  const VectorType& vec = arguments[0]->toVector();
  std::vector<std::string> parts;
  parts.reserve(vec.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    parts.push_back(vec[i].toString());
  }
  
  return Value{string_join(parts, delimiter)};
}

static Value builtin_replace(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 3) {
    print_argCnt_warning("replace", arguments.size(), "3", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "replace() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "replace() requires a string as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[2]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "replace() requires a string as third argument, got %1$s", arguments[2]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_replace(
    arguments[0]->toStrUtf8Wrapper().toString(),
    arguments[1]->toStrUtf8Wrapper().toString(),
    arguments[2]->toStrUtf8Wrapper().toString()
  )};
}

static Value builtin_starts_with(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 2) {
    print_argCnt_warning("starts_with", arguments.size(), "2", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "starts_with() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "starts_with() requires a string as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_starts_with(
    arguments[0]->toStrUtf8Wrapper().toString(),
    arguments[1]->toStrUtf8Wrapper().toString()
  )};
}

static Value builtin_ends_with(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 2) {
    print_argCnt_warning("ends_with", arguments.size(), "2", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "ends_with() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "ends_with() requires a string as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_ends_with(
    arguments[0]->toStrUtf8Wrapper().toString(),
    arguments[1]->toStrUtf8Wrapper().toString()
  )};
}

static Value builtin_contains(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 2) {
    print_argCnt_warning("contains", arguments.size(), "2", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "contains() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "contains() requires a string as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }

  return Value{string_contains(
    arguments[0]->toStrUtf8Wrapper().toString(),
    arguments[1]->toStrUtf8Wrapper().toString()
  )};
}

static Value builtin_substr(Arguments arguments, const Location& loc)
{
  if (arguments.size() < 2 || arguments.size() > 3) {
    print_argCnt_warning("substr", arguments.size(), "2 or 3", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "substr() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::NUMBER) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "substr() requires a number as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }

  long start = static_cast<long>(arguments[1]->toDouble());
  long length = -1;
  
  if (arguments.size() == 3) {
    if (arguments[2]->type() != Value::Type::NUMBER) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "substr() requires a number as third argument, got %1$s", arguments[2]->typeName());
      return Value::undefined.clone();
    }
    length = static_cast<long>(arguments[2]->toDouble());
  }

  return Value{string_substr(arguments[0]->toStrUtf8Wrapper().toString(), start, length)};
}

static Value builtin_index_of(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 2) {
    print_argCnt_warning("index_of", arguments.size(), "2", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "index_of() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "index_of() requires a string as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }

  return Value{static_cast<double>(string_index_of(
    arguments[0]->toStrUtf8Wrapper().toString(),
    arguments[1]->toStrUtf8Wrapper().toString()
  ))};
}

static Value builtin_pad_left(Arguments arguments, const Location& loc)
{
  if (arguments.size() < 2 || arguments.size() > 3) {
    print_argCnt_warning("pad_left", arguments.size(), "2 or 3", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "pad_left() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::NUMBER) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "pad_left() requires a number as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }

  size_t length = static_cast<size_t>(arguments[1]->toDouble());
  char pad_char = ' ';
  
  if (arguments.size() == 3) {
    if (arguments[2]->type() != Value::Type::STRING) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "pad_left() requires a string as third argument, got %1$s", arguments[2]->typeName());
      return Value::undefined.clone();
    }
    std::string pad_str = arguments[2]->toStrUtf8Wrapper().toString();
    if (!pad_str.empty()) {
      pad_char = pad_str[0];
    }
  }

  return Value{string_pad_left(arguments[0]->toStrUtf8Wrapper().toString(), length, pad_char)};
}

static Value builtin_pad_right(Arguments arguments, const Location& loc)
{
  if (arguments.size() < 2 || arguments.size() > 3) {
    print_argCnt_warning("pad_right", arguments.size(), "2 or 3", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "pad_right() requires a string as first argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }
  if (arguments[1]->type() != Value::Type::NUMBER) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "pad_right() requires a number as second argument, got %1$s", arguments[1]->typeName());
    return Value::undefined.clone();
  }

  size_t length = static_cast<size_t>(arguments[1]->toDouble());
  char pad_char = ' ';
  
  if (arguments.size() == 3) {
    if (arguments[2]->type() != Value::Type::STRING) {
      LOG(message_group::Warning, loc, arguments.documentRoot(),
          "pad_right() requires a string as third argument, got %1$s", arguments[2]->typeName());
      return Value::undefined.clone();
    }
    std::string pad_str = arguments[2]->toStrUtf8Wrapper().toString();
    if (!pad_str.empty()) {
      pad_char = pad_str[0];
    }
  }

  return Value{string_pad_right(arguments[0]->toStrUtf8Wrapper().toString(), length, pad_char)};
}

// =============================================================================
// Registration
// =============================================================================

void register_builtin_strings()
{
  Builtins::init("upper", new BuiltinFunction(&builtin_upper),
                 {
                   "upper(string) -> string",
                 });

  Builtins::init("lower", new BuiltinFunction(&builtin_lower),
                 {
                   "lower(string) -> string",
                 });

  Builtins::init("trim", new BuiltinFunction(&builtin_trim),
                 {
                   "trim(string) -> string",
                 });

  Builtins::init("ltrim", new BuiltinFunction(&builtin_ltrim),
                 {
                   "ltrim(string) -> string",
                 });

  Builtins::init("rtrim", new BuiltinFunction(&builtin_rtrim),
                 {
                   "rtrim(string) -> string",
                 });

  Builtins::init("split", new BuiltinFunction(&builtin_split),
                 {
                   "split(string) -> vector",
                   "split(string, delimiter) -> vector",
                 });

  Builtins::init("join", new BuiltinFunction(&builtin_join),
                 {
                   "join(vector) -> string",
                   "join(vector, delimiter) -> string",
                 });

  Builtins::init("replace", new BuiltinFunction(&builtin_replace),
                 {
                   "replace(string, old, new) -> string",
                 });

  Builtins::init("starts_with", new BuiltinFunction(&builtin_starts_with),
                 {
                   "starts_with(string, prefix) -> bool",
                 });

  Builtins::init("ends_with", new BuiltinFunction(&builtin_ends_with),
                 {
                   "ends_with(string, suffix) -> bool",
                 });

  Builtins::init("contains", new BuiltinFunction(&builtin_contains),
                 {
                   "contains(string, substring) -> bool",
                 });

  Builtins::init("substr", new BuiltinFunction(&builtin_substr),
                 {
                   "substr(string, start) -> string",
                   "substr(string, start, length) -> string",
                 });

  Builtins::init("index_of", new BuiltinFunction(&builtin_index_of),
                 {
                   "index_of(string, substring) -> number",
                 });

  Builtins::init("pad_left", new BuiltinFunction(&builtin_pad_left),
                 {
                   "pad_left(string, length) -> string",
                   "pad_left(string, length, char) -> string",
                 });

  Builtins::init("pad_right", new BuiltinFunction(&builtin_pad_right),
                 {
                   "pad_right(string, length) -> string",
                   "pad_right(string, length, char) -> string",
                 });
}
