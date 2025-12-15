# OpenSCAD Standard Library Development

This folder contains development work for adding standard library functions to OpenSCAD.

## Project Overview

This project adds string manipulation and file system functions to OpenSCAD as built-in functions.
The goal is to provide commonly needed functionality that is currently missing from the language.

## Implementation Status

### String Functions ✅ Complete

All string functions are implemented and tested in `src/core/builtin_functions_strings.cc`.

| Function | Signature | Description | Status |
|----------|-----------|-------------|--------|
| `lower` | `lower(string) -> string` | Convert string to lowercase | ✅ |
| `upper` | `upper(string) -> string` | Convert string to uppercase | ✅ |
| `trim` | `trim(string) -> string` | Remove whitespace from both ends | ✅ |
| `ltrim` | `ltrim(string) -> string` | Remove whitespace from left | ✅ |
| `rtrim` | `rtrim(string) -> string` | Remove whitespace from right | ✅ |
| `split` | `split(string, delimiter) -> vector` | Split string by delimiter | ✅ |
| `join` | `join(vector, separator) -> string` | Join vector elements with separator | ✅ |
| `replace` | `replace(string, search, replacement) -> string` | Replace all occurrences | ✅ |
| `starts_with` | `starts_with(string, prefix) -> bool` | Check if string starts with prefix | ✅ |
| `ends_with` | `ends_with(string, suffix) -> bool` | Check if string ends with suffix | ✅ |
| `contains` | `contains(string, substring) -> bool` | Check if string contains substring | ✅ |
| `substr` | `substr(string, start, [length]) -> string` | Extract substring | ✅ |
| `index_of` | `index_of(string, substring, [start]) -> number` | Find substring position | ✅ |
| `pad_left` | `pad_left(string, width, [char]) -> string` | Pad string on the left | ✅ |
| `pad_right` | `pad_right(string, width, [char]) -> string` | Pad string on the right | ✅ |

**Features:**
- Full UTF-8 support for all functions
- Proper error handling with warnings for invalid arguments
- Returns `undef` for invalid inputs (consistent with OpenSCAD conventions)

### File Functions ✅ Complete

All file functions are implemented and tested in `src/core/builtin_functions_files.cc`.

| Function | Signature | Description | Status |
|----------|-----------|-------------|--------|
| `file_exists` | `file_exists(path) -> bool` | Check if file/directory exists | ✅ |
| `is_file` | `is_file(path) -> bool` | Check if path is a regular file | ✅ |
| `is_dir` | `is_dir(path) -> bool` | Check if path is a directory | ✅ |
| `file_size` | `file_size(path) -> number` | Get file size in bytes | ✅ |
| `basename` | `basename(path) -> string` | Get filename from path | ✅ |
| `dirname` | `dirname(path) -> string` | Get directory from path | ✅ |
| `file_extension` | `file_extension(path) -> string` | Get file extension (without dot) | ✅ |
| `read_text` | `read_text(path) -> string` | Read entire file as string | ✅ |
| `read_lines` | `read_lines(path) -> vector` | Read file as vector of lines | ✅ |
| `read_dir` | `read_dir(path) -> vector` | List directory contents | ✅ |
| `absolute_path` | `absolute_path(path) -> string` | Convert to absolute path | ✅ |
| `canonical_path` | `canonical_path(path) -> string` | Get canonical (normalized) path | ✅ |

**Features:**
- Path resolution relative to the .scad file location
- Security: hidden files (dot-prefix on Unix, hidden attribute on Windows) are not accessible
- Cross-platform support (Windows and Unix)
- Proper error handling with warnings for inaccessible paths
- UTF-8 path and content support

### Test Results

#### String Functions Test Output
```
ECHO: "lower basic:", true
ECHO: "upper basic:", true
ECHO: "trim both:", true
ECHO: "split comma:", true
ECHO: "join comma:", true
ECHO: "replace single:", true
ECHO: "starts_with true:", true
ECHO: "ends_with true:", true
ECHO: "contains true:", true
ECHO: "substr basic:", true
ECHO: "index_of found:", true
ECHO: "pad_left space:", true
ECHO: "pad_right space:", true
... (all 50+ assertions pass)
```

#### File Functions Test Output
```
ECHO: "absolute_path .:", "C:\...\tests\data\scad\functions-files"
ECHO: "file_exists this file:", true
ECHO: "file_exists nonexistent:", false
ECHO: "is_file this file:", true
ECHO: "is_file testdirectory:", false
ECHO: "is_dir testdirectory:", true
ECHO: "file_size readtesttext.txt:", 160
ECHO: "basename /path/to/file.scad:", "file.scad"
ECHO: "dirname /path/to/file.scad:", "/path/to"
ECHO: "file_extension model.stl:", "stl"
ECHO: "read_dir .:", ["files-functions-debug.scad", "files-functions-tests.json", ...]
ECHO: "read_text readtesttext.txt:", "This is a test file...äöü ñ 日本語 emoji: 🎉..."
ECHO: "read_lines readtestlines.txt:", ["Line 1: First line", "Line 2: Second line", ...]
```

## Files Modified/Added

### New Source Files
- `src/core/builtin_functions_strings.cc` - String function implementations
- `src/core/builtin_functions_strings.h` - String function header
- `src/core/builtin_functions_files.cc` - File function implementations
- `src/core/builtin_functions_files.h` - File function header

### Modified Files
- `CMakeLists.txt` - Added new source files to CORE_SOURCES (around line 1035):
  ```cmake
  src/core/builtin_functions.cc
  src/core/builtin_functions_strings.cc
  src/core/builtin_functions_files.cc
  ```

- `src/core/Builtins.cc` - Added extern declarations and registration calls:
  ```cpp
  extern void register_builtin_strings();
  extern void register_builtin_files();
  
  // In Builtins::initialize():
  register_builtin_strings();
  register_builtin_files();
  ```

### Test Files
- `tests/data/scad/misc/string-functions-tests.scad` - Comprehensive string function tests
- `tests/data/scad/functions-files/files-functions-tests.scad` - File function tests
- `tests/data/scad/functions-files/files-functions-debug.scad` - Debug/diagnostic tests
- `tests/data/scad/functions-files/readtesttext.txt` - Test data for read_text()
- `tests/data/scad/functions-files/readtestlines.txt` - Test data for read_lines()
- `tests/data/scad/functions-files/sub-folder/` - Relative path testing
- `tests/data/scad/functions-files/testdirectory/` - Directory type testing

## Building

Use the UCRT64 environment in MSYS2:

```bash
# Start MSYS2 UCRT64 shell
cd ~/repos/openscad-stdlib
mkdir build-ucrt
cd build-ucrt
cmake .. -G"MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DEXPERIMENTAL=ON
cmake --build . -j4
```

## Testing

### Run String Function Tests
```bash
./openscad.exe -o test.csg ../tests/data/scad/misc/string-functions-tests.scad
```

### Run File Function Tests
```bash
./openscad.exe -o test.csg ../tests/data/scad/functions-files/files-functions-tests.scad
```

### Interactive Testing
```bash
./openscad.exe ../tests/data/scad/functions-files/files-functions-tests.scad
# Press F5 to run preview
```

## Unit Tests (Future Work)

The `unit-tests/` subfolder contains C++ unit tests that are currently not integrated
into the build. These were disabled due to toolchain issues with test discovery.

### Files
- `unit-tests/builtin_functions_strings_test.cc` - Catch2 unit tests for string utilities

### CMake Integration (Disabled)
To re-enable unit tests in the future, add to CMakeLists.txt:

```cmake
# String function unit tests
set(STDLIB_TEST_SOURCES
  tests/stdlib-dev/unit-tests/builtin_functions_strings_test.cc
)

target_sources(OpenSCADUnitTests PRIVATE ${STDLIB_TEST_SOURCES})

# Note: catch_discover_tests was disabled due to execution issues
# catch_discover_tests(OpenSCADUnitTests)
```

## Architecture Notes

### Function Registration Pattern
OpenSCAD uses a registration pattern for built-in functions:

1. Implement the function as `static Value builtin_funcname(Arguments arguments, const Location& loc)`
2. Create a registration function `void register_builtin_category()`
3. In the registration function, call `Builtins::init("funcname", new BuiltinFunction(&builtin_funcname), {...})`
4. Declare the registration function as `extern` in `Builtins.cc`
5. Call the registration function in `Builtins::initialize()`

### Value Types
OpenSCAD values are handled through the `Value` class:
- `Value::Type::STRING` - String values
- `Value::Type::NUMBER` - Numeric values  
- `Value::Type::BOOL` - Boolean values
- `Value::Type::VECTOR` - Array/list values
- `Value::Type::UNDEFINED` - Undefined value

### Error Handling
- Use `LOG(message_group::Warning, ...)` for warnings
- Return `Value::undefined.clone()` for error cases
- Use `print_argCnt_warning()` for argument count errors

## Future Enhancements

Potential additions:
- `write_text(path, content)` - Write string to file (security considerations)
- `parse_json(string)` - Parse JSON string to OpenSCAD values
- `to_json(value)` - Convert OpenSCAD value to JSON string
- `regex_match(string, pattern)` - Regular expression matching
- `regex_replace(string, pattern, replacement)` - Regular expression replacement
- `format(template, ...)` - String formatting with placeholders