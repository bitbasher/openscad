# Color Scheme Preferences - Design Intent and Implementation Log
**Date Range:** Initial session through December 9, 2025
**Branch:** `prefs-color-update`
**Primary Goal:** Separate color scheme preferences for 3D View rendering and Editor syntax highlighting

---

## Features Implemented

### 1. Separate Color Scheme Panels
**Objective:** Create distinct UI panels for 3D View and Editor color schemes to avoid confusion.

**Discussion Points:**
- Initial implementation had a single panel that mixed both scheme types
- User requested separate panels with clear visual distinction
- Schemes needed to be filtered by type (render vs editor)

**Implementation:**
- Created two separate panels in Preferences dialog
- 3D View panel: Shows rendering color schemes from `color-schemes/render/*.json`
- Editor panel: Shows syntax highlighting schemes from `color-schemes/editor/*.json`
- Each panel has:
  - Scheme list (QListWidget)
  - Color table (QTableWidget with 3 columns)
  - Preview area (QTextEdit for metadata)
  - Header showing current scheme path/name

**Conclusion:** ✅ Complete - Schemes properly separated and filtered by type

---

### 2. Color Scheme Tables
**Objective:** Display color scheme details in an organized, readable table format.

**Discussion Points:**
- Initial implementation showed index column - removed as unnecessary
- Color column sizing needed to be square (width = row height)
- RGB values needed better formatting for readability
- Table needed to stretch to reasonable width but not too wide

**Implementation:**
- 3-column layout:
  1. **Name:** Color/element name (ResizeToContents)
  2. **Color:** Visual color patch with no text (Fixed width = row height)
  3. **RGB:** Hex values formatted as "rrr ggg bbb" (Stretch mode)
- Hidden vertical header (row numbers)
- Set maximum width to 400px
- Set minimum width to 300px

**Conclusion:** ✅ Complete - Tables display cleanly with proper spacing and sizing

---

### 3. Color Scheme Persistence
**Objective:** Save and restore user's selected color schemes across sessions.

**Discussion Points:**
- Initial bug: Editor scheme not persisting due to wrong settings key
- Settings keys needed to match what other parts of the app expect

**Implementation:**
- 3D View scheme: Saved to `3dview/colorscheme`
- Editor scheme: Saved to `editor/syntaxhighlight` (fixed from incorrect `editor/colorscheme`)
- Both schemes load on preferences dialog open
- Schemes apply immediately when changed

**Bug Fixes:**
- Fixed editor scheme persistence - was using wrong key `editor/colorscheme` instead of `editor/syntaxhighlight`
- Verified schemes persist and load correctly across app restarts

**Conclusion:** ✅ Complete - Both schemes persist correctly

---

### 4. Editor Scheme Static Cache
**Objective:** Prevent crashes from dangling pointers when enumerating editor color schemes.

**Discussion Points:**
- App crashed when switching editor schemes repeatedly
- Root cause: EditorColorScheme objects destroyed after findEditorColorScheme() returned
- Needed static storage to keep scheme objects alive

**Implementation:**
- Added static `colorscheme_set_t` cache in `ScintillaEditor.cc`
- Modified `findEditorColorScheme()` and `enumerateColorSchemeNames()` to use static cache
- Schemes persist in memory for lifetime of application

**Conclusion:** ✅ Complete - No more crashes when switching schemes

---

### 5. Scheme Header Styling
**Objective:** Create visually distinct headers for each color scheme panel.

**Discussion Points:**
- Initial design had color indicator dots - removed as redundant
- Headers should show scheme name/path clearly
- Visual distinction between 3D and Editor panels via border colors
- Border thickness increased from 3px to 10px for better visibility

**Implementation:**
- White background for both headers
- Centered, bold scheme name/path text
- Thick bottom border:
  - 3D View: 10px solid blue (#3498db)
  - Editor: 10px solid green (#2ecc71)
- 8px padding for spacing
- Removed color indicator label widgets

**Deferred:**
- Dynamic border color changes based on scheme colors
- Reason: No good "signature color" in schemes; would require adding metadata

**Conclusion:** ✅ Complete - Clean, professional headers with static colored borders

---

### 6. EditorColorScheme Path Access
**Objective:** Display the file path of the selected editor color scheme in the header.

**Discussion Points:**
- `EditorColorScheme` class had private `path` member with no getter
- Need to show users which file provides the scheme
- Variable naming conflict: member `path` conflicted with method `path()`

**Implementation:**
- Renamed member variable from `path` to `schemeFilePath`
- Added public `const fs::path& path() const` getter method
- Updated Preferences.cc to display editor scheme file path
- 3D View shows scheme name only (RenderColorScheme::path() is private)

**Conclusion:** ✅ Complete - Editor scheme path displayed; 3D scheme shows name

---

### 7. Tri-State View Control Checkboxes
**Objective:** Allow users to control startup state of 3D view icons without losing their in-session preferences.

**Discussion Points:**
- Original checkboxes were simple on/off, which forced settings at startup
- Users needed option to preserve their last session's view state
- Three states required:
  1. **Checked:** Force icon ON at startup
  2. **Unchecked:** Force icon OFF at startup
  3. **Partial (gray):** Don't force - use last user setting

**Implementation:**
- Made three checkboxes tri-state:
  - Show Axes
  - Show Scale Markers
  - Show Edges
- Added tooltips explaining the three states
- Changed signal handlers from `toggled(bool)` to `stateChanged(int)`
- Settings store int values: 0=force off, 1=use last (default), 2=force on
- Updated `MainWindow::loadViewSettings()` to respect tri-state preferences
- Default to PartiallyChecked (1) for new users

**Settings Keys:**
- Preferences: `3dview/showAxes`, `3dview/showScaleMarkers`, `3dview/showEdges`
- Runtime state: `view/showAxes`, `view/showScaleProportional`, `view/showEdges`

**Conclusion:** ✅ Complete - Users can force startup state or preserve session preferences

---

### 8. Tri-State Projection Setting
**Objective:** Add "Don't Force" option to projection preference to preserve user's last projection mode.

**Discussion Points:**
- Similar to view control checkboxes, projection needed a non-forcing option
- ComboBox approach better than checkbox for three mutually exclusive states
- Clear labeling needed: "Don't Force" rather than "Use Last Setting"

**Implementation:**
- Added "Don't Force" as first item in projection combobox
- Order: 0=Don't Force, 1=Perspective, 2=Orthogonal
- Added tooltip explaining three options
- Updated handlers to save index instead of string
- Updated `MainWindow::loadViewSettings()` to respect preference
- Defaults to "Don't Force" (index 0) for new users

**Settings Keys:**
- Preference: `3dview/projection` (stores int index)
- Runtime state: `view/orthogonalProjection` (stores bool)

**Conclusion:** ✅ Complete - Projection preference preserves user choice by default

---

## Technical Issues Resolved

### Issue 1: Dangling Pointer Crash
**Symptom:** App crashes when switching editor schemes repeatedly
**Root Cause:** EditorColorScheme objects destroyed after enumeration
**Solution:** Static cache keeps schemes alive for app lifetime
**Files:** `src/gui/ScintillaEditor.cc`, `src/gui/ScintillaEditor.h`

### Issue 2: Editor Scheme Not Persisting
**Symptom:** Editor color scheme not loading on app restart
**Root Cause:** Using wrong settings key `editor/colorscheme` instead of `editor/syntaxhighlight`
**Solution:** Fixed settings key in Preferences.cc (2 locations)
**Files:** `src/gui/Preferences.cc`

### Issue 3: Table Layout Issues
**Symptom:** Index column showing, RGB column too narrow, table not stretching
**Root Cause:** Default resize modes and missing size constraints
**Solution:** Hide vertical header, set column modes, add min/max widths
**Files:** `src/gui/Preferences.cc`

### Issue 4: Scheme List Mixing Types
**Symptom:** 3D view schemes appearing in editor list and vice versa
**Root Cause:** Single enumeration function for all schemes
**Solution:** Separate enumeration for render vs editor schemes
**Files:** `src/gui/Preferences.cc`

### Issue 5: Member Variable Name Conflict
**Symptom:** Compiler error: `path` member conflicts with `path()` method
**Root Cause:** C++ doesn't allow member and method with same name
**Solution:** Renamed member to `schemeFilePath`
**Files:** `src/gui/ScintillaEditor.h`, `src/gui/ScintillaEditor.cc`

### Issue 6: Tri-State Default Values
**Symptom:** New users would get Unchecked (0) by default
**Root Cause:** QSettings returns 0 for missing keys
**Solution:** Use `.value(key, 1)` to default to PartiallyChecked
**Files:** `src/gui/Preferences.cc`, `src/gui/MainWindow.cc`

---

## Architecture Notes

### Color Scheme Interface Inconsistency
**Issue:** `RenderColorScheme` and `EditorColorScheme` have different interfaces

**Differences:**
- **RenderColorScheme::path():**
  - Returns: `std::string`
  - Visibility: Private
  - Access: Cannot retrieve path from outside class

- **EditorColorScheme::path():**
  - Returns: `const fs::path&`
  - Visibility: Public
  - Access: Can retrieve path via public method

**Impact:**
- 3D View header shows scheme name only (path inaccessible)
- Editor header shows full file path
- Inconsistent user experience

**Recommendation:** Refactor both classes to implement consistent interface in future project

---

## Items Deferred to Resource Inventory Project

### 1. Better CGAL Color Names
**Description:** Improve naming of CGAL-related colors in 3D view schemes
**Reason:** Names like "CGAL_FACE_2D_COLOR" are technical; need user-friendly names
**Scope:** Requires color scheme JSON file updates and display name mapping
**Status:** Explicitly deferred by user: "we will have to leave item 2 for another project"

### 2. Signature Colors and Icons for Schemes
**Description:** Add icon and signature color metadata to each color scheme
**Purpose:** Visual identification and dynamic header coloring
**Implementation Would Include:**
- Add `"icon"` and `"signatureColor"` fields to scheme JSON files
- Display icon next to scheme name in lists
- Use signature color for header border (instead of static blue/green)
- Provide visual variety and better scheme recognition

**Current Workaround:** Static border colors (blue for 3D, green for Editor)

### 3. Unified Color Scheme Interface
**Description:** Refactor `RenderColorScheme` and `EditorColorScheme` to share common interface
**Benefits:**
- Consistent path access (both public, same return type)
- Consistent metadata access methods
- Easier code maintenance
- Could enable shared rendering/display code

**Proposed Interface:**
```cpp
class ColorSchemeBase {
  virtual const fs::path& path() const = 0;
  virtual const QString& name() const = 0;
  virtual int index() const = 0;
  virtual bool valid() const = 0;
  // ... other common methods
};
```

---

## Items Still TODO for Color Scheme Preferences

### 1. 3D View Preview Panel
**Status:** ⏳ Not Started
**Description:** Live preview of 3D rendering with selected color scheme
**Current State:** Preview area exists in UI but shows metadata text only
**Proposed Implementation:**
- Small 3D viewport rendering a simple test object
- Apply selected color scheme to preview
- Show grid, axes, edges based on scheme colors
- Update in real-time when scheme changes

**Challenges:**
- Need to create minimal 3D context
- Requires rendering pipeline initialization
- May impact performance on slower machines
- Need test geometry that showcases all color categories

**Priority:** Medium - Nice to have, not critical for functionality

### 2. Scheme Import/Export
**Status:** 💭 Not Planned Yet
**Description:** Allow users to import/export custom color schemes
**Use Cases:**
- Share custom schemes with other users
- Backup personal schemes
- Install community-created schemes

**Implementation Ideas:**
- File chooser dialog to select JSON file
- Validation of JSON structure
- Copy to appropriate directory (render/ or editor/)
- Refresh scheme list after import

### 3. Live Scheme Editing
**Status:** 💭 Future Enhancement
**Description:** Edit color values directly in preferences dialog
**Scope:**
- Color picker for each color entry
- Live preview of changes
- Save/Save As functionality
- Revert to default option

**Challenges:**
- Complex UI for color editing
- Need to handle scheme file writing
- Validation and error handling
- Undo/redo for editing session

### 4. Scheme Validation and Error Reporting
**Status:** 💭 Not Planned Yet
**Description:** Better handling of malformed or incomplete scheme files
**Current State:** Errors logged but not shown to user
**Improvements:**
- Show warning icon for invalid schemes
- Tooltip explaining validation errors
- Option to view detailed error log
- Graceful fallback to default scheme

---

## Files Modified

### UI Files
- `src/gui/Preferences.ui`
  - Added separate 3D View and Editor color scheme panels
  - Made checkboxes tri-state (Show Axes, Show Scale Markers, Show Edges)
  - Added tooltips to tri-state controls
  - Added "Don't Force" option to projection combobox with tooltip
  - Styled headers with colored borders

### Source Files
- `src/gui/Preferences.cc`
  - Implemented `populate3DColorTable()` for 3D view schemes
  - Implemented `populateEditorColorTable()` for editor schemes
  - Added tri-state checkbox handlers (`stateChanged` instead of `toggled`)
  - Updated scheme loading/saving logic
  - Fixed editor scheme persistence (correct settings key)
  - Added projection tri-state handling

- `src/gui/Preferences.h`
  - Updated signal handler declarations for tri-state checkboxes

- `src/gui/ScintillaEditor.h`
  - Renamed `path` member to `schemeFilePath` to avoid conflict
  - Added public `path()` getter method

- `src/gui/ScintillaEditor.cc`
  - Implemented static cache for EditorColorScheme objects
  - Updated constructor to use `schemeFilePath`
  - Implemented `path()` getter returning `schemeFilePath`

- `src/gui/MainWindow.cc`
  - Updated `loadViewSettings()` to respect tri-state preferences
  - Added logic for axes, scale markers, edges tri-state handling
  - Added projection tri-state handling

---

## Settings Schema

### 3D View Settings
| Key | Type | Values | Default | Purpose |
|-----|------|--------|---------|---------|
| `3dview/colorscheme` | QString | Scheme name | "Cornfield" | Selected rendering color scheme |
| `3dview/showAxes` | int | 0/1/2 | 1 | Force axes: 0=off, 1=no force, 2=on |
| `3dview/showScaleMarkers` | int | 0/1/2 | 1 | Force scale markers: 0=off, 1=no force, 2=on |
| `3dview/showEdges` | int | 0/1/2 | 1 | Force edges: 0=off, 1=no force, 2=on |
| `3dview/projection` | int | 0/1/2 | 0 | Force projection: 0=no force, 1=perspective, 2=orthogonal |

### Editor Settings
| Key | Type | Values | Default | Purpose |
|-----|------|--------|---------|---------|
| `editor/syntaxhighlight` | QString | Scheme name | "For Light Background" | Selected syntax highlighting scheme |

### Runtime View State (separate from preferences)
| Key | Type | Purpose |
|-----|------|---------|
| `view/showAxes` | bool | Last session state for axes visibility |
| `view/showScaleProportional` | bool | Last session state for scale markers |
| `view/showEdges` | bool | Last session state for edges visibility |
| `view/orthogonalProjection` | bool | Last session state for projection mode |

---

## Translation Support

All user-facing strings in the UI are translatable via Qt's translation system:
- Strings in `.ui` files are extracted by `lupdate`
- Tooltips will appear in translation files (`.po` format)
- Existing locale support: cs, de, es, fr, hy, it, ka, pl, pt_BR, ru, tr, uk, zh_CN, zh_TW
- New strings will need translation in future translation update

---

## Testing Performed

### Functional Testing
- ✅ 3D view scheme changes apply to viewport
- ✅ Editor scheme changes apply to main editor and preview
- ✅ Schemes persist across app restarts
- ✅ No crashes when switching schemes repeatedly
- ✅ Tables display correctly with proper column sizing
- ✅ Scheme lists show only appropriate schemes (no mixing)
- ✅ Tri-state checkboxes work correctly (checked/unchecked/partial)
- ✅ Tri-state defaults to partial for new users
- ✅ Projection setting respects "Don't Force" option

### UI/UX Testing
- ✅ Headers display with colored borders
- ✅ Color patches render correctly in tables
- ✅ RGB hex values formatted clearly
- ✅ Tooltips appear on hover for tri-state controls
- ✅ Tables stretch to appropriate width

### Regression Testing
- ✅ Existing preferences still work
- ✅ Other 3D view settings unaffected
- ✅ Editor settings unaffected

---

## Known Limitations

1. **3D View Scheme Path Not Displayed**
   - RenderColorScheme::path() is private
   - Cannot show file path in header (only name)
   - Would require changes to ColorMap.h interface

2. **No Live 3D Preview**
   - Preview area shows metadata only
   - Cannot preview colors on actual 3D geometry

3. **Static Header Border Colors**
   - Blue for 3D, green for Editor
   - Not derived from scheme colors
   - Requires signature color metadata to implement

4. **Metadata Display is Basic**
   - Plain text format only
   - Could benefit from formatted display
   - No validation of metadata content

---

## Build Information

- **Build System:** CMake with MSYS2 MinGW64
- **Compiler:** GCC 15.2.0
- **Parallel Build:** `-j4` required (higher parallelism causes issues)
- **Qt Version:** Qt5 (QScintilla for editor)
- **Platform:** Windows (MSYS2 environment)

---

## Next Session Priorities

1. Implement 3D view preview panel (if desired)
2. Test with various color schemes for edge cases
3. Consider scheme import/export functionality
4. Review and prepare for PR submission
5. Update user documentation
6. Consider adding unit tests for scheme loading/persistence

---

## PR Preparation Notes

### Commits Should Include
- Separate color scheme UI panels
- Tri-state view control preferences
- Editor scheme persistence fix
- Static cache crash fix
- Header styling
- EditorColorScheme path() getter

### Documentation to Update
- User manual: New color scheme preferences
- Developer docs: Tri-state preference pattern
- Release notes: New features and bug fixes

### Review Checklist
- Code follows OpenSCAD style guidelines
- All compiler warnings addressed
- No memory leaks introduced
- Translation strings properly marked
- Settings backward compatible
- User experience tested with multiple schemes

---

**Document Created:** December 9, 2025
**Session Duration:** Full implementation session
**Status:** Feature complete except 3D preview panel (deferred)
