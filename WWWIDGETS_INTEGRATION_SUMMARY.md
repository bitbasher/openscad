# wwWidgets Integration Summary

## Overview

This document describes the integration of the wwWidgets library (specifically QwwColorComboBox and related widgets) into OpenSCAD's preferences dialog using a pre-built library deployment strategy instead of Git submodules.

**Status:** ✅ **COMPLETE** - OpenSCAD CMake and source integration ready for pre-built wwWidgets library
**Date:** December 11, 2024
**Integration Method:** Pre-built artifact deployment (clean, modular approach)

## Executive Summary

The wwWidgets library integration into OpenSCAD has been successfully configured using a pre-built artifact deployment strategy. OpenSCAD's build system is now ready to accept pre-built wwWidgets headers and libraries without requiring complex Git submodule setup. All CMake configuration and source code modifications are in place and verified.

### Current State
- ✅ CMakeLists.txt configured to import wwwidgets
- ✅ Preferences.cc includes qwwcolorcombobox.h
- ✅ Directory structure created and ready for artifacts
- ⏳ Awaiting wwWidgets pre-built artifacts (headers + library)

## Changes Made

### 1. CMakeLists.txt - Main Build Configuration

#### Location: `d:/repositories/openscad-master/CMakeLists.txt`

**Change 1: Added wwWidgets subdirectory import (line 427)**

```cmake
find_package(PkgConfig)

add_subdirectory(submodules)

# Import pre-built wwWidgets library (Qt color combo box widgets)
add_subdirectory(src/ext/wwwidgets)

# The submodules will set up mimalloc if needed, and we need to defer any
# target_link_libraries() calls until after that.
```

**Exact location verified:** Line 427 of CMakeLists.txt
**Type:** NEW lines added after `add_subdirectory(submodules)`

**Change 2: Added wwwidgets linking for Qt6 build (line 1617)**

```cmake
if (USE_QT6)
  if(NOT HEADLESS)
    target_link_libraries(OpenSCADLibInternal PUBLIC
      Qt6::Core Qt6::Core5Compat Qt6::Widgets Qt6::Multimedia Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Concurrent Qt6::Network Qt6::Svg
      ${QT6QSCINTILLA_LIBRARY} ${Qt6DBus_LIBRARIES} ${Qt6Gamepad_LIBRARIES}
      wwwidgets
    )
  endif()
```

**Exact location verified:** Line 1617 - `wwwidgets` added to Qt6 target_link_libraries
**Type:** NEW item added to existing target_link_libraries call

**Change 3: Added wwwidgets linking for Qt5 build (line 1629)**

```cmake
else()
  if(NOT HEADLESS)
    target_link_libraries(OpenSCADLibInternal PUBLIC
      Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::OpenGL Qt5::Concurrent Qt5::Network Qt5::Svg
      ${QT5QSCINTILLA_LIBRARY} ${Qt5DBus_LIBRARIES} ${Qt5Gamepad_LIBRARIES}
      wwwidgets
    )
  endif()
```

**Exact location verified:** Line 1629 - `wwwidgets` added to Qt5 target_link_libraries
**Type:** NEW item added to existing target_link_libraries call

### 2. Preferences.cc - Color Preferences UI

#### Location: `d:/repositories/openscad-master/src/gui/Preferences.cc`

**Added include directive (line 51)**

```cpp
#include <QRegularExpression>
#include "qwwcolorcombobox.h"
#include <QRegularExpressionValidator>
#include <QStatusBar>
```

**Exact location verified:** Line 51 of Preferences.cc
**Type:** NEW include statement for QwwColorComboBox widget
**Note:** Duplicate was removed; single include at line 51

This enables the use of QwwColorComboBox widget in the color preferences setup.

### 3. New Directory Structure for wwWidgets

#### Location: `d:/repositories/openscad-master/src/ext/wwwidgets/`

Created the following directory structure:
```
src/ext/wwwidgets/
├── CMakeLists.txt           # Build configuration for imported library
├── VERSION.txt              # Version tracking file
├── include/                 # Pre-built headers (to be populated)
│   └── qwwcolorcombobox.h
│   └── (other widget headers)
└── lib/                     # Pre-built libraries (to be populated)
    └── libwwwidgets.a
```

### 4. wwWidgets CMakeLists.txt

#### Location: `d:/repositories/openscad-master/src/ext/wwwidgets/CMakeLists.txt`

```cmake
# wwWidgets library integration
# Pre-built library from wwWidgets project (https://github.com/bitbasher/wwWidgets-1.0-qt5.git)
# Branch: port-to-msys-mingw64

# Ensure Qt version matches
if(NOT USE_QT6)
  message(WARNING "wwWidgets library is built for Qt6, but OpenSCAD is configured for Qt5. This may cause compatibility issues.")
endif()

# Define the wwwidgets imported library
add_library(wwwidgets STATIC IMPORTED GLOBAL)

# Set library paths
set(WWWIDGETS_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(WWWIDGETS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(wwwidgets PROPERTIES
  IMPORTED_LOCATION "${WWWIDGETS_LIB_DIR}/libwwwidgets.a"
  INTERFACE_INCLUDE_DIRECTORIES "${WWWIDGETS_INCLUDE_DIR}"
)

# Link Qt6 libraries that wwwidgets depends on
target_link_libraries(wwwidgets INTERFACE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
)

message(STATUS "wwWidgets: ${WWWIDGETS_INCLUDE_DIR}")
message(STATUS "wwWidgets library: ${WWWIDGETS_LIB_DIR}/libwwwidgets.a")
```

### 5. VERSION.txt Template

#### Location: `d:/repositories/openscad-master/src/ext/wwwidgets/VERSION.txt`

```
wwWidgets Pre-built Library Version Information
=================================================

Qt Version: 6.10.1
Build Date: YYYY-MM-DD
Source Commit: (wwWidgets commit hash)
Source Branch: port-to-msys-mingw64
Build Target: deploy-openscad
```

## Architecture Overview

### Deployment Strategy

Instead of using Git submodules (which had path resolution issues), this integration:

1. **Uses Pre-built Artifacts**: The wwWidgets library is built separately and deployed as pre-built artifacts
2. **Copy-based Deployment**: Headers and library are copied from the wwWidgets build output folder:
   - Source: `<wwWidgets repo>/deploy-openscad/include/` → Destination: `src/ext/wwwidgets/include/`
   - Source: `<wwWidgets repo>/deploy-openscad/lib/libwwwidgets.a` → Destination: `src/ext/wwwidgets/lib/`
3. **Version Tracking**: VERSION.txt stores metadata for tracking library source and compatibility

### Qt Version Configuration

- **Current Configuration**: USE_QT6=ON for all platforms (set in CMakeLists.txt line 45)
- **Reason**: MSYS2 Qt Creator only available for Qt6; Qt5 support removed to simplify builds
- **wwWidgets**: Also built with Qt6 to maintain consistency

### Build Process

1. **Configure**: CMake finds the wwWidgets IMPORTED library via `add_subdirectory(src/ext/wwwidgets)`
2. **Compile**: OpenSCAD compiles with wwWidgets headers included from `src/ext/wwwidgets/include/`
3. **Link**: OpenSCADLibInternal links against the pre-built `libwwwidgets.a` static library
4. **Validation**: CMakeLists checks Qt version match and warns if versions differ

## Integration Points

### Preferences Dialog
- **File**: `src/gui/Preferences.cc`
- **Widget**: QwwColorComboBox for intuitive color selection
- **Features**:
  - Color previews with RGB hex display
  - Pre-defined color palettes
  - Visual color picker integration

### Dependencies
- Qt6::Core, Qt6::Gui, Qt6::Widgets (linked through wwwidgets)
- C++17 standard (OpenSCAD requirement)

## Pending Tasks

### User Actions (wwWidgets side - separate window):
1. Build wwWidgets library with deploy-openscad target
2. Generate libwwwidgets.a static library
3. Generate header files with QwwColorComboBox interface

### Integration Tasks (OpenSCAD side):
1. Copy `deploy-openscad/include/*` to `src/ext/wwwidgets/include/`
2. Copy `deploy-openscad/lib/libwwwidgets.a` to `src/ext/wwwidgets/lib/`
3. Update VERSION.txt with source commit hash and build date
4. Test CMake configuration: `cmake .. -DUSE_QT6=ON`
5. Build OpenSCAD: `cmake --build .`
6. Verify no compilation errors related to wwWidgets headers
7. Test color preferences dialog functionality

## Testing Checklist

- [ ] CMake configuration succeeds
- [ ] libwwwidgets.a found at src/ext/wwwidgets/lib/libwwwidgets.a
- [ ] Qt6 version check passes (USE_QT6=ON matches library)
- [ ] OpenSCAD compilation completes without wwWidgets-related errors
- [ ] Preferences dialog loads without crashes
- [ ] Color selection widgets appear in preferences
- [ ] Color changes apply to 3D view

## Troubleshooting

### CMake Error: "libwwwidgets.a not found"
- Check that src/ext/wwwidgets/lib/libwwwidgets.a exists
- Verify wwWidgets build completed successfully
- Confirm deploy-openscad target was selected during build

### Compilation Error: "qwwcolorcombobox.h not found"
- Check that src/ext/wwwidgets/include/ contains header files
- Verify WWWIDGETS_INCLUDE_DIR is set correctly in CMakeLists.txt
- Ensure headers were copied from deploy-openscad output

### Qt Version Mismatch Warning
- Check CMakeLists.txt line 45: USE_QT6 setting
- Verify wwWidgets was built with matching Qt version (Qt6.10.1)
- Rebuild wwWidgets if Qt version changed

### Link Errors with wwwidgets symbols
- Check that libwwwidgets.a is a valid static library: `file src/ext/wwwidgets/lib/libwwwidgets.a`
- Verify MinGW compiler compatibility (MSYS2 MINGW64 used for both)
- Check OpenSCAD target_link_libraries includes wwwidgets

## Verification & PR Review Details

### Files Modified - Summary for Review

| File | Lines Changed | Type | Purpose |
|------|--------------|------|---------|
| `CMakeLists.txt` | 427, 1617, 1629 | 3 changes | Import wwwidgets library and link to OpenSCADLibInternal |
| `src/gui/Preferences.cc` | 51 | 1 change | Include QwwColorComboBox widget header |

### Complete Change List

#### CMakeLists.txt

**Change 1 - Line 427: Import wwWidgets IMPORTED library**
- **Before:**
  ```
  add_subdirectory(submodules)

  # The submodules will set up mimalloc...
  ```
- **After:**
  ```
  add_subdirectory(submodules)

  # Import pre-built wwWidgets library (Qt color combo box widgets)
  add_subdirectory(src/ext/wwwidgets)

  # The submodules will set up mimalloc...
  ```
- **Impact:** Loads wwWidgets CMakeLists.txt which defines IMPORTED library

**Change 2 - Line 1617: Add wwwidgets to Qt6 linking**
- **Before:**
  ```
  target_link_libraries(OpenSCADLibInternal PUBLIC
    Qt6::Core Qt6::Core5Compat Qt6::Widgets Qt6::Multimedia Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Concurrent Qt6::Network Qt6::Svg
    ${QT6QSCINTILLA_LIBRARY} ${Qt6DBus_LIBRARIES} ${Qt6Gamepad_LIBRARIES}
  )
  ```
- **After:**
  ```
  target_link_libraries(OpenSCADLibInternal PUBLIC
    Qt6::Core Qt6::Core5Compat Qt6::Widgets Qt6::Multimedia Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Concurrent Qt6::Network Qt6::Svg
    ${QT6QSCINTILLA_LIBRARY} ${Qt6DBus_LIBRARIES} ${Qt6Gamepad_LIBRARIES}
    wwwidgets
  )
  ```
- **Impact:** Links wwwidgets static library to OpenSCADLibInternal for Qt6 builds

**Change 3 - Line 1629: Add wwwidgets to Qt5 linking**
- **Before:**
  ```
  target_link_libraries(OpenSCADLibInternal PUBLIC
    Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::OpenGL Qt5::Concurrent Qt5::Network Qt5::Svg
    ${QT5QSCINTILLA_LIBRARY} ${Qt5DBus_LIBRARIES} ${Qt5Gamepad_LIBRARIES}
  )
  ```
- **After:**
  ```
  target_link_libraries(OpenSCADLibInternal PUBLIC
    Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::OpenGL Qt5::Concurrent Qt5::Network Qt5::Svg
    ${QT5QSCINTILLA_LIBRARY} ${Qt5DBus_LIBRARIES} ${Qt5Gamepad_LIBRARIES}
    wwwidgets
  )
  ```
- **Impact:** Links wwwidgets static library to OpenSCADLibInternal for Qt5 builds (fallback)

#### src/gui/Preferences.cc

**Change 1 - Line 51: Add QwwColorComboBox include**
- **Before:**
  ```cpp
  #include <QRegularExpression>
  #include <QRegularExpressionValidator>
  ```
- **After:**
  ```cpp
  #include <QRegularExpression>
  #include "qwwcolorcombobox.h"
  #include <QRegularExpressionValidator>
  ```
- **Impact:** Makes QwwColorComboBox widget available for use in color preferences

### Integration Verification

**All changes verified on December 11, 2024:**

✅ `add_subdirectory(src/ext/wwwidgets)` found at line 427
✅ `wwwidgets` linking found in Qt6 section at line 1617
✅ `wwwidgets` linking found in Qt5 section at line 1629
✅ `#include "qwwcolorcombobox.h"` found at line 51 in Preferences.cc
✅ No duplicate includes present
✅ Directory structure created: `src/ext/wwwidgets/` with CMakeLists.txt, VERSION.txt, include/, lib/

### Build System Impact

When wwWidgets artifacts are deployed:

1. **CMake Configuration Stage:**
   - Loads `src/ext/wwwidgets/CMakeLists.txt`
   - Creates IMPORTED library target `wwwidgets`
   - Sets IMPORTED_LOCATION to `src/ext/wwwidgets/lib/libwwwidgets.a`
   - Sets INTERFACE_INCLUDE_DIRECTORIES to `src/ext/wwwidgets/include`
   - Links Qt6::Core, Qt6::Gui, Qt6::Widgets (automatically)

2. **Compilation Stage:**
   - Preprocessor includes headers from `src/ext/wwwidgets/include`
   - Compiles Preferences.cc with QwwColorComboBox symbol definitions available

3. **Linking Stage:**
   - Linker resolves QwwColorComboBox symbols from `libwwwidgets.a`
   - OpenSCAD executable contains color widget functionality

### Pre-Build Checklist

Before deploying artifacts, verify:

```bash
# Verify files will be found
ls -la d:/repositories/openscad-master/src/ext/wwwidgets/CMakeLists.txt
ls -la d:/repositories/openscad-master/src/ext/wwwidgets/VERSION.txt
ls -d d:/repositories/openscad-master/src/ext/wwwidgets/include
ls -d d:/repositories/openscad-master/src/ext/wwwidgets/lib

# Verify CMakeLists changes
grep -n "add_subdirectory(src/ext/wwwidgets)" CMakeLists.txt
grep -c "wwwidgets" CMakeLists.txt
# Should show: 2 (Qt6 and Qt5 linking)

# Verify Preferences.cc changes
grep -n "qwwcolorcombobox.h" src/gui/Preferences.cc
# Should show: 51:#include "qwwcolorcombobox.h"
```

## References

- wwWidgets Project: https://github.com/bitbasher/wwWidgets-1.0-qt5.git
- Branch: port-to-msys-mingw64
- Qt Version: 6.10.1 (MSYS2 MINGW64)
- Compiler: g++ (MinGW64)
- Build System: CMake 3.25+
