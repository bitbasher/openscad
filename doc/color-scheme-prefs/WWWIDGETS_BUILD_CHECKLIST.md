# wwWidgets Integration - Remaining Steps

## Summary of Completed Work

✅ **OpenSCAD CMake Configuration Updated**

- Added `add_subdirectory(src/ext/wwwidgets)` to main CMakeLists.txt (line 426)
- Added `wwwidgets` to Qt6 target_link_libraries (line 1617)
- Added `wwwidgets` to Qt5 target_link_libraries (line 1629)

✅ **Preferences.cc Updated**

- Added `#include "qwwcolorcombobox.h"` (line 51)

✅ **Directory Structure Created**

- `src/ext/wwwidgets/CMakeLists.txt` - IMPORTED library configuration
- `src/ext/wwwidgets/include/` - Ready for headers
- `src/ext/wwwidgets/lib/` - Ready for libwwwidgets.a
- `src/ext/wwwidgets/VERSION.txt` - Metadata tracking

✅ **Documentation Created**

- `WWWIDGETS_INTEGRATION_SUMMARY.md` - Complete integration guide

## Next Steps - Build and Deploy

### Step 1: Build wwWidgets Library (Separate Window)

In the wwWidgets repository window, build the deploy-openscad target:

```bash
cd /path/to/wwWidgets-repo
mkdir -p build && cd build
cmake .. -DUSE_QT6=ON -DCMAKE_BUILD_TYPE=Release
cmake --build . --target deploy-openscad
```

Expected output location:

```text
build/deploy-openscad/
├── include/
│   ├── qwwcolorcombobox.h
│   ├── qwwwidgetlibrary.h
│   └── (other widget headers)
└── lib/
    ├── libwwwidgets.a
    └── (possibly other libraries)
```

### Step 2: Copy Pre-built Artifacts

Once the build completes, copy the deploy-openscad output to OpenSCAD:

From wwWidgets build directory:

```bash
# Copy headers
cp -r build/deploy-openscad/include/* \
  d:/repositories/openscad-master/src/ext/wwwidgets/include/

# Copy library
cp build/deploy-openscad/lib/libwwwidgets.a \
  d:/repositories/openscad-master/src/ext/wwwidgets/lib/
```

Alternatively in PowerShell (Windows):

```powershell
# Copy headers
Copy-Item -Path ".\deploy-openscad\include\*" `
  -Destination "d:\repositories\openscad-master\src\ext\wwwidgets\include\" `
  -Recurse -Force

# Copy library
Copy-Item -Path ".\deploy-openscad\lib\libwwwidgets.a" `
  -Destination "d:\repositories\openscad-master\src\ext\wwwidgets\lib\" `
  -Force
```

### Step 3: Update VERSION.txt

Edit `src/ext/wwwidgets/VERSION.txt`:

```text
wwWidgets Pre-built Library Version Information
=================================================

Qt Version: 6.10.1
Build Date: 2025-01-XX
Source Commit: [wwWidgets commit hash - get from 'git rev-parse HEAD']
Source Branch: port-to-msys-mingw64
Build Target: deploy-openscad
```

### Step 4: Configure OpenSCAD Build

In the OpenSCAD build directory (or create new):

```bash
cd d:/repositories/openscad-master/build
cmake .. -DUSE_QT6=ON -DCMAKE_BUILD_TYPE=Release
```

Expected CMake output should show:

```text
-- wwWidgets: d:/repositories/openscad-master/src/ext/wwwidgets/include
-- wwWidgets library: d:/repositories/openscad-master/src/ext/wwwidgets/lib/libwwwidgets.a
```

### Step 5: Build OpenSCAD

```bash
cmake --build . -j 4
```

Expected behavior:

- No compilation errors related to qwwcolorcombobox.h
- No undefined reference errors from wwwidgets library symbols
- libwwwidgets.a is found and linked successfully

### Step 6: Test

Run the built OpenSCAD executable:

```bash
./OpenSCAD
# or on Windows: OpenSCAD.exe
```

Then:

1. Open Edit → Preferences
2. Navigate to 3D View or color preferences
3. Verify color selection widgets appear
4. Test color changes in the 3D view

## Verification Checklist

Before building, verify the setup:

```bash
# Check directory contents
ls -la d:/repositories/openscad-master/src/ext/wwwidgets/
ls -la d:/repositories/openscad-master/src/ext/wwwidgets/include/
ls -la d:/repositories/openscad-master/src/ext/wwwidgets/lib/

# Should show:
# - CMakeLists.txt (exists)
# - VERSION.txt (exists and populated)
# - include/qwwcolorcombobox.h (will be added after copy)
# - lib/libwwwidgets.a (will be added after copy)
```

Check CMakeLists.txt integration:

```bash
grep -n "add_subdirectory(src/ext/wwwidgets)" CMakeLists.txt
# Should show: 426:add_subdirectory(src/ext/wwwidgets)

grep -n "wwwidgets" CMakeLists.txt
# Should show multiple lines with wwwidgets linking
```

Check Preferences.cc includes:

```bash
grep -n "qwwcolorcombobox.h" src/gui/Preferences.cc
# Should show: 51:#include "qwwcolorcombobox.h"
```

## Troubleshooting Guide

### Issue: CMake cannot find libwwwidgets.a

**Solution:**

1. Verify file exists: `ls -l src/ext/wwwidgets/lib/libwwwidgets.a`
2. Check it's a valid library: `file src/ext/wwwidgets/lib/libwwwidgets.a`
   - Should show: "data" (binary archive) or similar
3. Ensure wwWidgets build completed successfully
4. Re-run CMake cache clean: `rm -rf build && mkdir build && cd build && cmake ..`

### Issue: Compilation error "qwwcolorcombobox.h: No such file or directory"

**Solution:**

1. Verify headers were copied: `ls src/ext/wwwidgets/include/`
2. Check CMakeLists.txt INTERFACE_INCLUDE_DIRECTORIES is set
3. Ensure wwWidgets deploy-openscad target built with headers
4. Verify no typos in header names

### Issue: Link error "undefined reference to `QwwColorComboBox::...`"

**Solution:**

1. Verify library file: `file src/ext/wwwidgets/lib/libwwwidgets.a`
2. Check library contains expected symbols: `nm src/ext/wwwidgets/lib/libwwwidgets.a | grep -i color`
3. Ensure libwwwidgets.a was built for same compiler (MinGW64)
4. Verify wwWidgets uses same Qt version (Qt6.10.1)

### Issue: Qt version mismatch warning

**Solution:**

- Check CMakeLists.txt line 45: `set(USE_QT6 ON)`
- Verify wwWidgets was built with: `cmake .. -DUSE_QT6=ON`
- Both must use Qt6 for consistency

## Files Modified

1. **d:/repositories/openscad-master/CMakeLists.txt**
   - Line 426: Added `add_subdirectory(src/ext/wwwidgets)`
   - Line 1617: Added `wwwidgets` to Qt6 target_link_libraries
   - Line 1629: Added `wwwidgets` to Qt5 target_link_libraries

2. **d:/repositories/openscad-master/src/gui/Preferences.cc**
   - Line 51: Added `#include "qwwcolorcombobox.h"`

3. **d:/repositories/openscad-master/src/ext/wwwidgets/** (New directory)
   - CMakeLists.txt
   - VERSION.txt
   - include/ (to be populated)
   - lib/ (to be populated)

## Integration Timeline

| Step | Task | Status | Owner |
|------|------|--------|-------|
| 1 | Build wwWidgets deploy-openscad | Pending | User (separate window) |
| 2 | Copy headers to include/ | Pending | User |
| 3 | Copy libwwwidgets.a to lib/ | Pending | User |
| 4 | Update VERSION.txt | Pending | User |
| 5 | Configure OpenSCAD build | Ready | User/Agent |
| 6 | Build OpenSCAD | Ready | User |
| 7 | Test color preferences | Ready | User |

## Contact & Support

For issues or questions during the build process:

1. Check WWWIDGETS_INTEGRATION_SUMMARY.md for detailed architecture
2. Review CMake output for specific error messages
3. Verify file paths match your system setup
4. Ensure MSYS2 MINGW64 environment is properly configured
