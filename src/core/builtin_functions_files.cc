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

#include "core/builtin_functions_files.h"

#include "core/Arguments.h"
#include "core/Builtins.h"
#include "core/function.h"
#include "core/Parameters.h"
#include "core/Value.h"
#include "platform/PlatformUtils.h"
#include "utils/printutils.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

// =============================================================================
// Utility Functions
// =============================================================================

bool is_hidden_file(const fs::path& path)
{
  std::string filename = path.filename().string();
  
  // Unix-style hidden files (dot prefix)
  if (!filename.empty() && filename[0] == '.') {
    return true;
  }

#ifdef _WIN32
  // Windows hidden/system attribute check
  try {
    DWORD attrs = GetFileAttributesW(path.wstring().c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
      if (attrs & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
        return true;
      }
    }
  } catch (...) {
    // If we can't check attributes, assume not hidden
  }
#endif

  return false;
}

bool is_path_accessible(const fs::path& path)
{
  // Check if any component of the path is hidden
  fs::path current = path;
  while (current.has_filename()) {
    if (is_hidden_file(current)) {
      return false;
    }
    if (!current.has_parent_path() || current.parent_path() == current) {
      break;
    }
    current = current.parent_path();
  }

  // Check if we have read permission
  std::error_code ec;
  auto status = fs::status(path, ec);
  if (ec) {
    return false;
  }

  // Check if path exists
  if (!fs::exists(status)) {
    return false;
  }

  return true;
}

// Replace the resolve_path function with this version:

fs::path resolve_path(const std::string& path, const std::string& base_dir, const std::string& fallback_dir)
{
  fs::path p = fs::u8path(path);
  
  // If already absolute, normalize and return
  if (p.is_absolute()) {
    std::error_code ec;
    fs::path normalized = fs::weakly_canonical(p, ec);
    return ec ? p : normalized;
  }
  
  // Use base_dir if available, otherwise fallback_dir
  std::string base = base_dir.empty() ? fallback_dir : base_dir;
  if (base.empty()) {
    // Last resort: use current working directory
    std::error_code ec;
    fs::path abs = fs::absolute(p, ec);
    if (!ec) {
      fs::path normalized = fs::weakly_canonical(abs, ec);
      return ec ? abs : normalized;
    }
    return p;
  }
  
  fs::path combined = fs::path(base) / p;
  
  // Normalize the path (resolve . and ..)
  std::error_code ec;
  fs::path normalized = fs::weakly_canonical(combined, ec);
  return ec ? combined : normalized;
}

bool file_exists_check(const fs::path& path)
{
  std::error_code ec;
  bool exists = fs::exists(path, ec);
  if (ec) {
    return false;
  }
  return exists && is_path_accessible(path);
}

std::optional<std::string> read_text_file(const fs::path& path)
{
  if (!is_path_accessible(path)) {
    return std::nullopt;
  }

  std::error_code ec;
  if (!fs::is_regular_file(path, ec) || ec) {
    return std::nullopt;
  }

  std::ifstream file(path, std::ios::in | std::ios::binary); // FIXME std::ios is not available in openscad
  if (!file.is_open()) {
    return std::nullopt;
  }

  std::ostringstream contents;
  contents << file.rdbuf();
  
  if (file.bad()) {
    return std::nullopt;
  }

  return contents.str();
}

std::optional<std::vector<std::string>> read_lines_file(const fs::path& path)
{
  if (!is_path_accessible(path)) {
    return std::nullopt;
  }

  std::error_code ec;
  if (!fs::is_regular_file(path, ec) || ec) {
    return std::nullopt;
  }

  std::ifstream file(path);
  if (!file.is_open()) {
    return std::nullopt;
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }

  if (file.bad()) {
    return std::nullopt;
  }

  return lines;
}

std::optional<std::vector<std::string>> read_directory(const fs::path& path)
{
  if (!is_path_accessible(path)) {
    return std::nullopt;
  }

  std::error_code ec;
  if (!fs::is_directory(path, ec) || ec) {
    return std::nullopt;
  }

  std::vector<std::string> entries;
  
  for (const auto& entry : fs::directory_iterator(path, ec)) {
    if (ec) {
      return std::nullopt;
    }
    
    // Skip hidden files
    if (is_hidden_file(entry.path())) {
      continue;
    }
    
    entries.push_back(entry.path().filename().string());
  }

  // Sort entries for consistent ordering
  std::sort(entries.begin(), entries.end());

  return entries;
}


std::string get_absolute_path(const std::string& path, const std::string& base_dir, const std::string& fallback_dir)
{
  fs::path resolved = resolve_path(path, base_dir, fallback_dir);
  return resolved.string();
}

std::optional<std::string> get_canonical_path(const std::string& path, const std::string& base_dir, const std::string& fallback_dir)
{
  fs::path resolved = resolve_path(path, base_dir, fallback_dir);
  
  std::error_code ec;
  fs::path canonical = fs::canonical(resolved, ec);
  if (ec) {
    return std::nullopt;
  }
  
  if (!is_path_accessible(canonical)) {
    return std::nullopt;
  }
  
  return canonical.string();
}

// =============================================================================
// Builtin Function Implementations
// =============================================================================

static Value builtin_file_exists(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("file_exists", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "file_exists() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path resolved = resolve_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  return Value{file_exists_check(resolved)};
}

static Value builtin_read_text(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("read_text", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "read_text() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path resolved = resolve_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  auto contents = read_text_file(resolved);
  if (!contents) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "read_text(): Cannot read file '%1$s'", path_str);
    return Value::undefined.clone();
  }
  
  return Value{*contents};
}

static Value builtin_read_lines(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("read_lines", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "read_lines() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path resolved = resolve_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  auto lines = read_lines_file(resolved);
  if (!lines) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "read_lines(): Cannot read file '%1$s'", path_str);
    return Value::undefined.clone();
  }
  
  VectorType result(arguments.session());
  for (const auto& line : *lines) {
    result.emplace_back(line);
  }
  return Value{std::move(result)};
}

static Value builtin_read_dir(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("read_dir", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "read_dir() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path resolved = resolve_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  auto entries = read_directory(resolved);
  if (!entries) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "read_dir(): Cannot read directory '%1$s'", path_str);
    return Value::undefined.clone();
  }
  
  VectorType result(arguments.session());
  for (const auto& entry : *entries) {
    result.emplace_back(entry);
  }
  return Value{std::move(result)};
}

static Value builtin_absolute_path(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("absolute_path", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "absolute_path() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  std::string result = get_absolute_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  return Value{result};
}

static Value builtin_canonical_path(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("canonical_path", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "canonical_path() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  auto result = get_canonical_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  if (!result) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "canonical_path(): Path does not exist or is not accessible '%1$s'", path_str);
    return Value::undefined.clone();
  }
  
  return Value{*result};
}


// Add these builtin function implementations before the register_builtin_files() function

static Value builtin_is_file(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("is_file", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "is_file() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path resolved = resolve_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  std::error_code ec;
  bool result = fs::is_regular_file(resolved, ec) && !ec && is_path_accessible(resolved);
  return Value{result};
}

static Value builtin_is_dir(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("is_dir", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "is_dir() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path resolved = resolve_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  std::error_code ec;
  bool result = fs::is_directory(resolved, ec) && !ec && is_path_accessible(resolved);
  return Value{result};
}

static Value builtin_file_size(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("file_size", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "file_size() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path resolved = resolve_path(path_str, arguments.documentRoot(), PlatformUtils::userDocumentsPath());
  
  if (!is_path_accessible(resolved)) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "file_size(): Cannot access file '%1$s'", path_str);
    return Value::undefined.clone();
  }
  
  std::error_code ec;
  auto size = fs::file_size(resolved, ec);
  if (ec) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "file_size(): Cannot get size of '%1$s'", path_str);
    return Value::undefined.clone();
  }
  
  return Value{static_cast<double>(size)};
}

static Value builtin_basename(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("basename", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "basename() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path p = fs::u8path(path_str);
  
  return Value{p.filename().string()};
}

static Value builtin_dirname(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("dirname", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "dirname() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path p = fs::u8path(path_str);
  
  return Value{p.parent_path().string()};
}

static Value builtin_file_extension(Arguments arguments, const Location& loc)
{
  if (arguments.size() != 1) {
    print_argCnt_warning("file_extension", arguments.size(), "1", loc, arguments.documentRoot());
    return Value::undefined.clone();
  }

  if (arguments[0]->type() != Value::Type::STRING) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "file_extension() requires a string argument, got %1$s", arguments[0]->typeName());
    return Value::undefined.clone();
  }

  std::string path_str = arguments[0]->toStrUtf8Wrapper().toString();
  fs::path p = fs::u8path(path_str);
  
  std::string ext = p.extension().string();
  // Remove leading dot if present
  if (!ext.empty() && ext[0] == '.') {
    ext = ext.substr(1);
  }
  
  return Value{ext};
}


// =============================================================================
// Registration
// =============================================================================

void register_builtin_files()
{
  Builtins::init("file_exists", new BuiltinFunction(&builtin_file_exists),
                 {
                   "file_exists(path) -> bool",
                 });

  Builtins::init("is_file", new BuiltinFunction(&builtin_is_file),
                 {
                   "is_file(path) -> bool",
                 });

  Builtins::init("is_dir", new BuiltinFunction(&builtin_is_dir),
                 {
                   "is_dir(path) -> bool",
                 });

  Builtins::init("file_size", new BuiltinFunction(&builtin_file_size),
                 {
                   "file_size(path) -> number",
                 });

  Builtins::init("basename", new BuiltinFunction(&builtin_basename),
                 {
                   "basename(path) -> string",
                 });

  Builtins::init("dirname", new BuiltinFunction(&builtin_dirname),
                 {
                   "dirname(path) -> string",
                 });

  Builtins::init("file_extension", new BuiltinFunction(&builtin_file_extension),
                 {
                   "file_extension(path) -> string",
                 });

  Builtins::init("read_text", new BuiltinFunction(&builtin_read_text),
                 {
                   "read_text(path) -> string",
                 });

  Builtins::init("read_lines", new BuiltinFunction(&builtin_read_lines),
                 {
                   "read_lines(path) -> vector",
                 });

  Builtins::init("read_dir", new BuiltinFunction(&builtin_read_dir),
                 {
                   "read_dir(path) -> vector",
                 });

  Builtins::init("absolute_path", new BuiltinFunction(&builtin_absolute_path),
                 {
                   "absolute_path(path) -> string",
                 });

  Builtins::init("canonical_path", new BuiltinFunction(&builtin_canonical_path),
                 {
                   "canonical_path(path) -> string",
                 });
}