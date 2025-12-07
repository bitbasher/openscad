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
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

/**
 * @brief Register file manipulation builtin functions with the OpenSCAD interpreter.
 *
 * Registers the following functions:
 * - file_exists(path) -> bool: Check if file or directory exists
 * - read_text(path) -> string: Read file contents as text
 * - read_lines(path) -> vector: Read file as vector of lines
 * - read_dir(path) -> vector: List files in a directory
 * - absolute_path(path) -> string: Get absolute path
 * - canonical_path(path) -> string: Get canonical path (resolved symlinks)
 *
 * Error handling: On failure, functions emit a warning via LOG() and return
 * undef (represented by std::nullopt from utility functions).
 */
void register_builtin_files();

// Utility functions exposed for unit testing
// These return std::optional - empty means failure (caller should emit warning)

/**
 * @brief Check if a path is accessible (not hidden, not system, user has permission).
 * @param path The path to check
 * @return true if the path is accessible, false with reason set if blocked
 */
bool is_path_accessible(const fs::path& path);

/**
 * @brief Check if a filename represents a hidden file.
 * @param path The path to check
 * @return true if the file is hidden
 */
bool is_hidden_file(const fs::path& path);

/**
 * @brief Resolve a relative path against a base directory.
 * @param path The path to resolve (may be relative or absolute)
 * @param base_dir The base directory for relative paths
 * @param fallback_dir Fallback directory if base_dir is empty (for unsaved files)
 * @return The resolved absolute path
 */
fs::path resolve_path(const std::string& path, const std::string& base_dir, const std::string& fallback_dir);

/**
 * @brief Check if a file or directory exists and is accessible.
 * @param path The path to check
 * @return true if exists and accessible, false otherwise
 */
bool file_exists_check(const fs::path& path);

/**
 * @brief Read entire file contents as a string.
 * @param path The file path
 * @return File contents, or std::nullopt on error
 */
std::optional<std::string> read_text_file(const fs::path& path);

/**
 * @brief Read file contents as a vector of lines.
 * @param path The file path
 * @return Vector of lines, or std::nullopt on error
 */
std::optional<std::vector<std::string>> read_lines_file(const fs::path& path);

/**
 * @brief List files in a directory (excluding hidden/system files).
 * @param path The directory path
 * @return Vector of filenames, or std::nullopt on error
 */
std::optional<std::vector<std::string>> read_directory(const fs::path& path);

/**
 * @brief Get the absolute path for a given path.
 * @param path The path to resolve
 * @param base_dir The base directory for relative paths
 * @param fallback_dir Fallback directory if base_dir is empty
 * @return The absolute path as a string
 */
std::string get_absolute_path(const std::string& path, const std::string& base_dir, const std::string& fallback_dir);

/**
 * @brief Get the canonical path for a given path (resolved symlinks, no . or ..).
 * @param path The path to resolve
 * @param base_dir The base directory for relative paths
 * @param fallback_dir Fallback directory if base_dir is empty
 * @return The canonical path, or std::nullopt if path doesn't exist
 */
std::optional<std::string> get_canonical_path(const std::string& path, const std::string& base_dir, const std::string& fallback_dir);

// Add these to the existing utility section (after the existing declarations):

/**
 * @brief List files in a directory with optional type filtering.
 * @param path The directory path
 * @param filter "filesdirs" (default), "fileonly", or "dironly"
 * @return Vector of filenames, or std::nullopt on error
 */
std::optional<std::vector<std::string>> read_directory(const fs::path& path, const std::string& filter = "filesdirs");

/**
 * @brief Check if a string matches a glob pattern (supports *, ?, [abc]).
 * @param text The string to match
 * @param pattern The glob pattern
 * @return true if text matches pattern
 */
bool glob_match(const std::string& text, const std::string& pattern);

/**
 * @brief Filter a vector of strings by glob pattern.
 * @param strings Vector of strings to filter
 * @param pattern Glob pattern to match
 * @return Vector of matching strings
 */
std::vector<std::string> glob_filter(const std::vector<std::string>& strings, const std::string& pattern);

