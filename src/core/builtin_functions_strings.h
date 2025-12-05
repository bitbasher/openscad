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

#pragma once

#include <string>
#include <vector>

/**
 * @brief Register string manipulation builtin functions with the OpenSCAD interpreter.
 *
 * Registers the following functions:
 * - upper(string) -> string: Convert string to uppercase (UTF-8 aware)
 * - lower(string) -> string: Convert string to lowercase (UTF-8 aware)
 * - trim(string) -> string: Remove leading and trailing whitespace
 * - ltrim(string) -> string: Remove leading whitespace
 * - rtrim(string) -> string: Remove trailing whitespace
 */
void register_builtin_strings();

// Utility functions exposed for unit testing

/**
 * @brief Convert a UTF-8 string to uppercase.
 * @param input The input string (UTF-8 encoded)
 * @return A new string with all characters converted to uppercase
 */
std::string string_upper(const std::string& input);

/**
 * @brief Convert a UTF-8 string to lowercase.
 * @param input The input string (UTF-8 encoded)
 * @return A new string with all characters converted to lowercase
 */
std::string string_lower(const std::string& input);

/**
 * @brief Remove leading and trailing whitespace from a string.
 * @param input The input string
 * @return A new string with leading and trailing whitespace removed
 */
std::string string_trim(const std::string& input);

/**
 * @brief Remove leading whitespace from a string.
 * @param input The input string
 * @return A new string with leading whitespace removed
 */
std::string string_ltrim(const std::string& input);

/**
 * @brief Remove trailing whitespace from a string.
 * @param input The input string
 * @return A new string with trailing whitespace removed
 */
std::string string_rtrim(const std::string& input);

/**
 * @brief Split a string by a delimiter.
 * @param input The input string
 * @param delimiter The delimiter string
 * @return A vector of strings
 */
std::vector<std::string> string_split(const std::string& input, const std::string& delimiter);

/**
 * @brief Join a vector of strings with a delimiter.
 * @param parts The vector of strings to join
 * @param delimiter The delimiter string
 * @return A single joined string
 */
std::string string_join(const std::vector<std::string>& parts, const std::string& delimiter);

/**
 * @brief Replace all occurrences of a substring.
 * @param input The input string
 * @param old_str The substring to find
 * @param new_str The replacement string
 * @return A new string with all occurrences replaced
 */
std::string string_replace(const std::string& input, const std::string& old_str, const std::string& new_str);

/**
 * @brief Check if a string starts with a prefix.
 * @param input The input string
 * @param prefix The prefix to check
 * @return true if input starts with prefix
 */
bool string_starts_with(const std::string& input, const std::string& prefix);

/**
 * @brief Check if a string ends with a suffix.
 * @param input The input string
 * @param suffix The suffix to check
 * @return true if input ends with suffix
 */
bool string_ends_with(const std::string& input, const std::string& suffix);

/**
 * @brief Check if a string contains a substring.
 * @param input The input string
 * @param substr The substring to find
 * @return true if input contains substr
 */
bool string_contains(const std::string& input, const std::string& substr);

/**
 * @brief Extract a substring (UTF-8 aware).
 * @param input The input string (UTF-8 encoded)
 * @param start The starting character index (0-based)
 * @param length The number of characters to extract (-1 for rest of string)
 * @return The extracted substring
 */
std::string string_substr(const std::string& input, long start, long length = -1);

/**
 * @brief Find the first index of a substring.
 * @param input The input string
 * @param substr The substring to find
 * @return The index of the first occurrence, or -1 if not found
 */
long string_index_of(const std::string& input, const std::string& substr);

/**
 * @brief Pad a string on the left to a specified length.
 * @param input The input string
 * @param length The target length
 * @param pad_char The character to pad with (default space)
 * @return The padded string
 */
std::string string_pad_left(const std::string& input, size_t length, char pad_char = ' ');

/**
 * @brief Pad a string on the right to a specified length.
 * @param input The input string
 * @param length The target length
 * @param pad_char The character to pad with (default space)
 * @return The padded string
 */
std::string string_pad_right(const std::string& input, size_t length, char pad_char = ' ');
