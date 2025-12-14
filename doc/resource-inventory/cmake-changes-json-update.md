# CMakeLists.txt Changes for Issue #6224

## Summary

The CMake changes for this PR are minimal and straightforward. Only two new source files were added to the build.

## Changes Made

### New Source Files Added

```cmake
# In CORE_SOURCES (around line 1095):
src/io/JsonReader.cc

# In GUI_SOURCES (around line 1309):
src/gui/EditorColorScheme.cc
```

### No Other CMake Changes

- No new dependencies added
- No new find_package() calls
- No changes to compiler flags
- No changes to link libraries
- No platform-specific changes

## Platform Considerations

### All Platforms
- Both new files use only Qt JSON classes (`QJsonDocument`, `QJsonObject`, `QJsonArray`, `QJsonValue`, `QJsonParseError`) which are part of QtCore
- Qt is already a required dependency on all platforms
- No additional libraries needed

### Cross-Compilation (MXE)
- No impact - Qt JSON is part of QtCore which is already included

### WebAssembly (Emscripten)
- No impact - Qt JSON is part of QtCore which is already included

### macOS
- No impact - uses same Qt JSON classes

### Windows (MSYS2/MinGW)
- Tested successfully on this platform
- No additional configuration needed

## Whitespace-Only Changes

The diff shows some whitespace changes (trailing spaces removed) in unrelated sections. These are cosmetic and do not affect functionality:
- Line 79: `WASM_BUILD_TYPE` description
- Lines 297, 309, 328-329: Emscripten section formatting
- Line 1114: Python sources formatting
- Line 1572: macOS bundle copy command

These may have been introduced by the editor and can be reverted if desired without affecting the functionality of this PR.

## Unit Tests

The unit test file `src/io/json_reader_test.cc` is automatically picked up by the existing CMake glob pattern:

```cmake
file(GLOB_RECURSE TEST_SOURCES
  "src/*_test.cc"
)
```

No changes were needed to support unit tests.

## Verification

The build was tested successfully on Windows (MSYS2/MinGW64) with:
- Full application build: `ninja openscad`
- Unit test build: `ninja OpenSCADUnitTests`
- All 21 tests passing (including 18 new JSON tests)
