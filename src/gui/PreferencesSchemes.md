# Color Scheme Implementation Notes

## Color Scheme Architecture

OpenSCAD has **two separate color scheme systems**:

### 1. RenderColorScheme (3D View)

- **Location**: `color-schemes/render/` directory
- **Class**: `RenderColorScheme` in `src/glview/ColorMap.h`
- **Manager**: `ColorMap` singleton
- **Purpose**: Colors for 3D rendering (background, faces, edges, axes, etc.)

### 2. EditorColorScheme (Editor)

- **Location**: `color-schemes/editor/` directory
- **Class**: `EditorColorScheme` in `src/gui/ScintillaEditor.h`
- **Manager**: `ScintillaEditor::enumerateColorSchemes()`
- **Purpose**: Syntax highlighting colors for the code editor

## Loading Process

Both systems enumerate color schemes at startup:

### RenderColorScheme Loading

```cpp
// ColorMap::enumerateColorSchemes()
colorscheme_set_t result_set;
auto *defaultColorScheme = new RenderColorScheme(); // Built-in Cornfield
result_set.insert(...);
enumerateColorSchemesInPath(result_set, PlatformUtils::resourceBasePath());
enumerateColorSchemesInPath(result_set, PlatformUtils::userConfigPath());
```

Searches in:

- Built-in default (Cornfield)
- `{resourceBasePath}/color-schemes/render/*.json`
- `{userConfigPath}/color-schemes/render/*.json`

### EditorColorScheme Loading

```cpp
// ScintillaEditor::enumerateColorSchemes()
colorscheme_set_t result_set;
enumerateColorSchemesInPath(result_set, PlatformUtils::resourceBasePath());
enumerateColorSchemesInPath(result_set, PlatformUtils::userConfigPath());
```

Searches in:

- `{resourceBasePath}/color-schemes/editor/*.json`
- `{userConfigPath}/color-schemes/editor/*.json`

## Data Structures

### RenderColorScheme

```cpp
class RenderColorScheme {
  fs::path _path;
  boost::property_tree::ptree pt;      // Full JSON data
  std::string _name;
  int _index;
  bool _show_in_gui;
  ColorScheme _color_scheme;           // map<RenderColor, Color4f>

public:
  const std::string& name() const;
  int index() const;
  bool valid() const;
  bool showInGui() const;
  ColorScheme& colorScheme();
  const boost::property_tree::ptree& propertyTree() const;  // Access to full JSON
};
```

### EditorColorScheme

```cpp
class EditorColorScheme {
  fs::path path;
  boost::property_tree::ptree pt;      // Full JSON data
  QString _name;
  int _index;

public:
  const QString& name() const;
  int index() const;
  bool valid() const;
  const boost::property_tree::ptree& propertyTree() const;  // Access to full JSON
};
```

## RenderColor Enum Values

```cpp
enum class RenderColor {
  BACKGROUND_COLOR,
  BACKGROUND_STOP_COLOR,
  AXES_COLOR,
  OPENCSG_FACE_FRONT_COLOR,
  OPENCSG_FACE_BACK_COLOR,
  CGAL_FACE_FRONT_COLOR,
  CGAL_FACE_2D_COLOR,
  CGAL_FACE_BACK_COLOR,
  CGAL_EDGE_FRONT_COLOR,
  CGAL_EDGE_BACK_COLOR,
  CGAL_EDGE_2D_COLOR,
  CROSSHAIR_COLOR
};
```

## Accessing Color Schemes

### Current ColorMap API

```cpp
// Returns only the color map, not the full RenderColorScheme object
const ColorScheme* findColorScheme(const std::string& name) const;

// Returns list of scheme names
std::list<std::string> colorSchemeNames(bool guiOnly = false) const;
```

### Problem

The `colorSchemeSet` member is **private** in ColorMap, so we can't directly access the `RenderColorScheme` objects to get:

- The property tree (for metadata display)
- The file path (for display in header)
- Other scheme properties

### Solution (Option A)

Add a new public method to ColorMap:

```cpp
const RenderColorScheme* findRenderColorScheme(const std::string& name) const;
```

This will allow Preferences to:

1. Get the full RenderColorScheme object by name
2. Access `propertyTree()` for metadata
3. Access the path and other properties
4. Iterate through the color scheme to populate the color table

## Implementation Plan

### Phase 1: Add ColorMap accessor ✓

- Add `findRenderColorScheme()` method to ColorMap.h
- Implement in ColorMap.cc

### Phase 2: Populate Color Tables

- Helper function to convert RenderColor enum to display strings
- Populate 3D color table from RenderColorScheme
- Populate Editor color table from EditorColorScheme property tree

### Phase 3: Display Metadata

- Extract metadata from property tree
- Display in metadata text widget
- Show/hide metadata GroupBox based on availability

### Phase 4: Display Scheme Info

- Show file path in header label
- Update color indicator with representative color from scheme

## Property Tree Structure Example

Example JSON structure for color schemes:

```json
{
  "name": "Cornfield",
  "index": 1000,
  "show-in-gui": true,
  "author": "Optional Author Name",
  "description": "Optional description",
  "background": [255, 255, 229],
  "axes-color": [0, 0, 0],
  "opencsg-face-front": [249, 215, 44],
  ...
}
```

Metadata fields to extract:

- author
- description
- license
- version
- Any other custom fields

## Current Status

✅ UI structure complete (Preferences.ui)
✅ Basic C++ infrastructure (slots, handlers)
✅ Settings storage (QSettings integration)
✅ Color scheme lists populated

🔄 In Progress:

- Adding ColorMap accessor method
- Implementing color table population
- Implementing metadata display

⏳ TODO:

- Button box actions (Add, Import, Export, Save)
- Preview panels (3D view and editor mock-ups)
- Color editing functionality
- Connecting settings to actual application behavior
