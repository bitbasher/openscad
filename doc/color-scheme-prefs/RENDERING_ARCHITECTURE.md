# OpenSCAD Rendering Architecture and Color Scheme Management

**Author:** Documentation generated during live color scheme editing implementation
**Date:** December 2024
**Status:** Comprehensive architecture documentation

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Preferences System Architecture](#preferences-system-architecture)
3. [Rendering Pipeline Architecture](#rendering-pipeline-architecture)
4. [Color Scheme Propagation Flow](#color-scheme-propagation-flow)
5. [The Challenge: Live Color Updates](#the-challenge-live-color-updates)
6. [Solution: Session-Based Color Overrides](#solution-session-based-color-overrides)
7. [Implementation Details](#implementation-details)
8. [Future Considerations](#future-considerations)

---

## Executive Summary

OpenSCAD uses a sophisticated multi-backend rendering architecture that caches geometry in VBOs (Vertex Buffer Objects) for performance. The rendering system has three main paths:

- **OpenCSG Preview (F5)**: Real-time CSG preview using OpenCSG library
- **CGAL Backend (F6)**: Traditional robust geometry using CGAL
- **Manifold Backend (F6)**: Modern fast geometry using Manifold

Color schemes are loaded from JSON files and applied at VBO creation time, making live updates challenging. This document describes the architecture and the solution for enabling live color edits in the Preferences dialog.

---

## Preferences System Architecture

### Core Components

#### 1. Preferences Class Hierarchy

```
QDialog (Qt base)
    └── Preferences (src/gui/Preferences.h/cc)
        └── GlobalPreferences singleton accessor

Constructor:
  - Loads UI from preferences.ui (Qt Designer file)
  - Populates scheme choosers from ColorMap
  - Sets up signal/slot connections
  - Creates preview widgets
```

#### 2. Color Scheme Management Classes

**ColorMap** (`src/glview/ColorMap.h/cc`)
- Singleton managing all color schemes
- Enumerates schemes from disk (`color-schemes/render/*.json`)
- Default scheme: "Cornfield"
- Key methods:
  - `findColorScheme(name)` → Returns ColorScheme map
  - `findRenderColorScheme(name)` → Returns RenderColorScheme object
  - `getColor(scheme, RenderColor)` → Static color lookup with override support

**RenderColorScheme** (`src/glview/ColorMap.h`)
```cpp
class RenderColorScheme {
  std::string _name;
  fs::path _path;
  int _index;  // Sort order (0 = default/Cornfield)
  bool _show_in_gui;
  ColorScheme _color_scheme;  // map<RenderColor, Color4f>
  boost::property_tree::ptree _pt;  // JSON metadata
};
```

**ColorScheme Type**
```cpp
using ColorScheme = std::map<RenderColor, Color4f>;
```

**RenderColor Enum** (partial)
```cpp
enum class RenderColor {
  BACKGROUND_COLOR,
  OPENCSG_FACE_FRONT_COLOR,  // Preview faces
  OPENCSG_FACE_BACK_COLOR,   // Preview cutouts
  CGAL_FACE_FRONT_COLOR,     // F6 render faces
  CGAL_FACE_BACK_COLOR,      // F6 render back faces
  CGAL_EDGE_FRONT_COLOR,     // Edge colors
  AXES_COLOR,
  CROSSHAIR_COLOR,
  // ... more
};
```

#### 3. Preferences UI Structure (3D Colors Tab)

The 3D Colors tab (`src/gui/Preferences.cc`, populated in `populate3DColorTable()`):

```
QComboBox: combo3DColorScheme
    ├─ Populated from ColorMap::colorSchemeNames()
    └─ Signal: currentIndexChanged → on3DColorSchemeChanged()

QTableWidget: tableWidget3DColors
    ├─ Column 0: Color name (QTableWidgetItem, read-only)
    ├─ Column 1: RGB hex display (QLabel with colored border, in cellWidget)
    └─ Column 2: "Pick…" button (QPushButton in cellWidget)
        └─ Signal: clicked → onColor3DPickerClicked(row, colorKey, scheme, initialColor)

QLabel: label3DSchemePath
    └─ Shows current scheme name

QGroupBox: groupBox3DMetadata
    └─ QTextEdit: textEdit3DMetadata
        └─ Shows author, description, license, version from JSON
```

#### 4. Key Signal/Slot Connections

**In Preferences Constructor:**
```cpp
// Scheme chooser changed
connect(combo3DColorScheme, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &Preferences::on3DColorSchemeChanged);

// Color picker per-row lambda (created in populate3DColorTable)
connect(colorButton, &QPushButton::clicked, this,
        [this, row, renderColor, schemeName, qcolor]() {
          onColor3DPickerClicked(row, renderColor, schemeName, qcolor);
        });
```

**In MainWindow Constructor (`src/gui/MainWindow.cc:597`):**
```cpp
// Preferences signals to GLView
connect(GlobalPreferences::inst(), &Preferences::requestRedraw,
        this->qglview, QOverload<>::of(&QGLView::update));

connect(GlobalPreferences::inst(), &Preferences::colorSchemeChanged,
        this, &MainWindow::setColorScheme);
```

**Signal Chain for Scheme Switch:**
```
User selects scheme in combo → currentIndexChanged
    → Preferences::on3DColorSchemeChanged()
        → populate3DColorTable(newScheme)
            → RenderSettings::inst()->clearColorOverrides()
        → emit colorSchemeChanged(newScheme)
            → MainWindow::setColorScheme(scheme)
                → RenderSettings::inst()->colorscheme = scheme
                → qglview->setColorScheme(scheme)
                    → GLView::setColorScheme(ColorScheme&)
                        → this->colorscheme = &cs
                        → updateColorScheme()
                            → renderer->setColorScheme(*colorscheme)
```

---

## Rendering Pipeline Architecture

### Renderer Class Hierarchy

```
Renderer (abstract base, src/glview/Renderer.h/cc)
    ├─ colormap_: map<ColorMode, Color4f>
    ├─ colorscheme_: const ColorScheme*
    ├─ color_override_revision_: uint64_t (NEW)
    └─ Methods:
        ├─ setColorScheme(ColorScheme&) [virtual]
        ├─ getColorSchemeColor(ColorMode, Color4f&)
        ├─ getShaderColor(ColorMode, object_color, Color4f&)
        ├─ refreshColorSchemeIfDirty() [NEW]
        ├─ prepare(ShaderInfo*) [pure virtual]
        └─ draw(bool showedges, ShaderInfo*) [pure virtual]

    └─ VBORenderer (src/glview/VBORenderer.h/cc)
        ├─ geom_visit_mark_: geometry deduplication cache
        ├─ calcNumVertices(): size calculations
        └─ add_shader_pointers(): barycentric data for edge rendering

        ├─ OpenCSGRenderer (src/glview/preview/OpenCSGRenderer.h/cc)
        │   ├─ Purpose: Real-time CSG preview (F5)
        │   ├─ vertex_state_containers_: vector<OpenCSGVBOProduct>
        │   ├─ Uses: OpenCSG library depth buffer algorithm
        │   └─ Colors: OPENCSG_FACE_FRONT/BACK, CGAL_EDGE_FRONT/BACK
        │
        ├─ ThrownTogetherRenderer (src/glview/preview/ThrownTogetherRenderer.h/cc)
        │   ├─ Purpose: Simple overlay preview (View → Thrown Together)
        │   ├─ vertex_state_containers_: vector<VertexStateContainer>
        │   └─ Colors: MATERIAL, CUTOUT, HIGHLIGHT, BACKGROUND modes
        │
        ├─ PolySetRenderer (src/glview/PolySetRenderer.h/cc)
        │   ├─ Purpose: Manifold backend render (F6 with Manifold)
        │   ├─ polyset_vertex_state_containers_
        │   ├─ polygon_vertex_state_containers_
        │   └─ Colors: CGAL_FACE_FRONT/BACK, CGAL_EDGE_FRONT/BACK, _2D variants
        │
        └─ CGALRenderer (src/glview/cgal/CGALRenderer.h/cc)
            ├─ Purpose: CGAL backend render (F6 with CGAL)
            ├─ vertex_state_containers_
            ├─ polyhedrons_: vector<VBOPolyhedron>
            └─ Colors: CGAL_FACE_FRONT/BACK, CGAL_EDGE_FRONT/BACK, _2D variants
```

### ColorMode Enum (Renderer Internal)

```cpp
enum class ColorMode {
  MATERIAL,           // Normal object faces (preview)
  CUTOUT,            // Difference/subtraction faces
  HIGHLIGHT,         // Highlighted objects (F4/!)
  BACKGROUND,        // Background objects (%)
  MATERIAL_EDGES,    // Edge outlines
  CUTOUT_EDGES,
  HIGHLIGHT_EDGES,
  BACKGROUND_EDGES,
  CGAL_FACE_2D_COLOR, // 2D projection faces
  CGAL_EDGE_2D_COLOR, // 2D projection edges
  EMPTY_SPACE        // Unused
};
```

### Rendering Lifecycle

#### 1. OpenCSG Preview (F5)

```
User edits → F5 → MainWindow::actionRenderPreview()
    → CSGTreeEvaluator builds tree
    → OpenCSGRenderer created with CSGProducts
    → MainWindow::compileAndRender()
        → qglview->setRenderer(opencsgRenderer)
        → qglview->update()
            → GLView::paintGL()
                → renderer->prepare(edge_shader)
                    → OpenCSGRenderer::prepare()
                        → createCSGVBOProducts()
                            ├─ VBOBuilder allocates buffers
                            ├─ For each CSGObject:
                            │   ├─ getShaderColor(MATERIAL/CUTOUT, ...)
                            │   │   → Renderer::getColorSchemeColor()
                            │   │       → colormap_[ColorMode::MATERIAL]
                            │   │           (set by setColorScheme from ColorMap::getColor)
                            │   └─ vbo_builder.create_surface(polyset, color, ...)
                            └─ vbo_builder.createInterleavedVBOs()
                                → Upload to GPU
                → renderer->draw(showedges, edge_shader)
                    → OpenCSGRenderer::draw()
                        ├─ OpenCSG::render(primitives)  // depth buffer
                        └─ vertex_state->draw()          // color pass
```

**Key Insight:** Colors are "baked" into VBOs during `createCSGVBOProducts()`. VBOs are cached in `vertex_state_containers_` and only rebuilt if containers are empty.

#### 2. Manifold Render (F6) - UPDATED

**OLD BEHAVIOR (Before Live Color Edit Fix):**
```
User → F6 → MainWindow::actionRender()
    → GeometryEvaluator builds geometry
    → ManifoldGeometry::toPolySet()
        ├─ Assigns colors from scheme AT EVALUATION TIME
        └─ ps->colors.push_back(ColorMap::getColor(scheme, CGAL_FACE_FRONT_COLOR))
            └─ Colors embedded in PolySet, cached forever
    → PolySetRenderer created
    → Colors never updated even if scheme/overrides change
```

**NEW BEHAVIOR (With Live Color Edit Fix + Alpha Override Fix):**
```
User → F6 → MainWindow::actionRender()
    → GeometryEvaluator builds geometry
    → ManifoldGeometry::toPolySet()
        ├─ Only stores script-provided colors (from color=[...] in OpenSCAD)
        ├─ Defers scheme colors (CGAL_FACE_FRONT/BACK) to renderer
        └─ ps->colors = [] for normal geometry (no scheme colors stored)
    → PolySetRenderer created
    → PolySetRenderer::prepare()
      → Detects RenderSettings override revision changes; clears cached VBOs and reapplies scheme
      → PolySetRenderer::createPolySetStates()
        ├─ Splits faces into front vs. cutout/back (cutout tagged with color_index == -2)
        ├─ If script colors exist → uses getShaderColor() with script color and current scheme fallback
        └─ If no script colors → uses getColorSchemeColor() from CURRENT scheme/overrides
        └─ Builds VBOs with live scheme/overrides for each face group
    → PolySetRenderer::draw()
        → Renders with current scheme colors (updates on color picker changes)
```

**Critical Change:** `ManifoldGeometry::toPolySet()` now only embeds script-provided colors.
Scheme colors (CGAL_FACE_FRONT/BACK) are applied by `PolySetRenderer` at render time, enabling live updates.

  **Bug Discovered and Fixed (Faces Disappearing on Color Change):**
  - Root cause: Color overrides created from the Preferences picker had undefined alpha (`alpha = -1`), so applying an override could zero out face visibility.
  - Fix: `RenderSettings::setColorOverride` now forces `alpha = 1.0` if missing; Preferences passes RGBA with alpha=1.0 when setting overrides.
  - Impact: Changing “CGAL Face Front/Back” no longer makes faces disappear; edges remain visible and unaffected.

#### 3. CGAL Render (F6)

```
Similar to Manifold but:
    → CGALUtils::createPolySetFromGeometry()
    → CGALRenderer::prepare()
        ├─ createPolySetStates()
        └─ VBOPolyhedron construction
            → VBOPolyhedron(ColorScheme&) constructor
                → setColor(UNMARKED_FACET_COLOR, ColorMap::getColor(cs, CGAL_FACE_FRONT_COLOR))
```

### VBO (Vertex Buffer Object) Architecture

**VertexState** (`src/glview/VertexState.h`)
- Encapsulates GL state and draw call
- Holds VBO/EBO handles
- `glBegin()`: vector of setup lambdas (enable arrays, bind buffers)
- `glEnd()`: vector of cleanup lambdas
- `draw()`: execute begin → glDrawArrays/Elements → execute end

**VBOBuilder** (`src/glview/VBOBuilder.h/cc`)
- Helper for creating vertex data
- Methods:
  - `addSurfaceData()`: position, normal, color attributes
  - `addShaderData()`: barycentric coordinates for edge rendering
  - `create_surface(polyset, transform, color, ...)`: tessellate and buffer
  - `createInterleavedVBOs()`: upload to GPU, create VertexState objects

**VertexStateContainer**
- Manages lifetime of VBOs
- `states()`: vector of VertexState shared_ptrs
- Destroyed when renderer is recreated

---

## Color Scheme Propagation Flow

### Initial Application Startup

```
main()
    → MainWindow constructor
        → GlobalPreferences::inst()->apply_win()
            → Reads QSettings: "3dview/colorscheme"
        → setColorScheme(savedScheme)
            → RenderSettings::inst()->colorscheme = scheme
            → qglview->setColorScheme(scheme)
```

### User Changes Scheme in Preferences

```
Preferences Dialog → combo3DColorScheme changed
    → on3DColorSchemeChanged(index)
        ├─ schemeName = combo->itemText(index)
        ├─ populate3DColorTable(schemeName)
        │   ├─ Clear table
        │   ├─ RenderSettings::inst()->clearColorOverrides()  ← NEW
        │   ├─ For each color in scheme:
        │   │   └─ Create row with name, hex display, picker button
        │   └─ Update metadata
        └─ emit colorSchemeChanged(schemeName)
            → MainWindow::setColorScheme(scheme)
                ├─ RenderSettings::inst()->colorscheme = scheme
                └─ qglview->setColorScheme(scheme)
                    → GLView::setColorScheme(QString)
                        → colorscheme = ColorMap::inst()->findColorScheme(cs)
                        → updateColorScheme()
                            → if (renderer) renderer->setColorScheme(*colorscheme)
                                → Renderer::setColorScheme(ColorScheme&)
                                    ├─ colormap_[MATERIAL] = ColorMap::getColor(cs, OPENCSG_FACE_FRONT)
                                    ├─ colormap_[CUTOUT] = ColorMap::getColor(cs, OPENCSG_FACE_BACK)
                                    ├─ ... (populate all ColorModes from scheme)
                                    └─ colorscheme_ = &cs
```

### Critical Path: ColorMap::getColor() (MODIFIED)

**Location:** `src/glview/ColorMap.cc:200`

```cpp
Color4f ColorMap::getColor(const ColorScheme& cs, const RenderColor rc)
{
  // Session overrides take precedence (NEW)
  if (const auto override = RenderSettings::inst()->getColorOverride(rc)) {
    return *override;
  }

  // Fallback to scheme
  if (cs.count(rc)) return cs.at(rc);

  // Fallback to default scheme
  if (ColorMap::inst()->defaultColorScheme().count(rc))
    return ColorMap::inst()->defaultColorScheme().at(rc);

  // Last resort
  return {0, 0, 0, 127};
}
```

This static method is called from:
- Renderer::setColorScheme() (sets colormap_)
- OpenCSGRenderer::createCSGVBOProducts() (via getShaderColor)
- PolySetRenderer::createPolySetStates() (via getShaderColor)
- CGALRenderer::createPolySetStates() (via getColorSchemeColor)
- VBOPolyhedron constructor (CGAL)
- ManifoldGeometry::toPolySet() (Manifold conversion)
- GLView::paintGL() (axes, background, crosshairs)

### Where Colors Actually Get Used

1. **Immediate Mode (No VBO Caching)**
   - Background gradient: `GLView::paintGL()` → `ColorMap::getColor(BACKGROUND_COLOR)`
   - Axes: `GLView::showAxes()` → uses axes color from paintGL
   - Crosshairs: similar

2. **VBO-Cached Geometry (Problem Area)**
   - OpenCSG preview faces/edges
   - Manifold render geometry
   - CGAL render geometry
   - All built once, cached until renderer recreated

---

## The Challenge: Live Color Updates

### Why Existing Code Doesn't Support Live Edits

**Problem 1: VBO Color Baking**
- Colors are written into VBO buffers at creation time
- VBOs stay in GPU memory until explicitly destroyed
- Changing ColorMap has no effect on existing VBOs

**Problem 2: Renderer Lifecycle**
- Renderers created per-render operation
- OpenCSGRenderer: created on F5, lives until next F5 or F6
- PolySetRenderer: created on F6, lives until next F5 or F6
- No mechanism to invalidate cached geometry

**Problem 3: Scheme File Immutability**
- RenderColorScheme loaded from disk JSON
- No in-memory mutation without saving files
- Multiple scheme instances would conflict

**Problem 4: Signal Chain Incomplete**
- No signal from Preferences to trigger VBO rebuild
- `requestRedraw` signal exists but only calls `QGLView::update()`
- `update()` → `paintGL()` → `renderer->draw()` (no prepare, uses cached VBOs)

### Failed Approach: Direct Scheme Mutation

Initial attempt (caused corruption):
```cpp
// In onColor3DPickerClicked
RenderColorScheme* scheme = const_cast<RenderColorScheme*>(
    ColorMap::inst()->findRenderColorScheme(schemeName)
);
scheme->colorScheme()[colorKey] = newColor;  // MUTATES SHARED SCHEME
emit requestRedraw();
```

**Why This Failed:**
- Mutated the shared ColorScheme map
- VBOs already created with old colors
- Only new geometry (next F5/F6) would show new colors
- Switching schemes left orphaned edits
- Faces disappeared due to invalid color state

---

## Solution: Session-Based Color Overrides

### Design Philosophy

**Principle 1: Preserve Immutability**
- Scheme files remain unchanged on disk
- ColorMap scheme objects never mutated
- Edits are session-only (lost on restart)

**Principle 2: Transparent Override Layer**
- `ColorMap::getColor()` checks override first
- All existing code paths automatically pick up overrides
- No changes to renderer color lookup logic

**Principle 3: VBO Invalidation via Revision**
- Track when overrides change with revision counter
- Renderers check revision, invalidate cached VBOs if stale
- Next `prepare()` rebuilds VBOs with new colors

**Principle 4: Automatic Refresh**
- `requestRedraw` signal triggers `QGLView::update()`
- Renderers check revision in `prepare()` (called even for cached state)
- Stale VBOs cleared, forcing rebuild

### Implementation Components

#### 1. RenderSettings Extensions

**File:** `src/glview/RenderSettings.h`

```cpp
class RenderSettings
{
public:
  // Existing
  RenderBackend3D backend3D;
  unsigned int openCSGTermLimit;
  double far_gl_clip_limit;
  std::string colorscheme;

  // NEW: Session override API
  void setColorOverride(RenderColor rc, const Color4f& color);
  void clearColorOverrides();
  bool hasColorOverride(RenderColor rc) const;
  std::optional<Color4f> getColorOverride(RenderColor rc) const;
  std::uint64_t colorOverrideRevision() const { return color_override_revision_; }

private:
  // NEW: Override storage
  std::map<RenderColor, Color4f> color_overrides_;
  std::uint64_t color_override_revision_{0};
};
```

**File:** `src/glview/RenderSettings.cc`

```cpp
void RenderSettings::setColorOverride(RenderColor rc, const Color4f& color)
{
  color_overrides_[rc] = color;
  color_override_revision_++;  // Invalidate cached VBOs
}

void RenderSettings::clearColorOverrides()
{
  color_overrides_.clear();
  color_override_revision_++;  // Invalidate cached VBOs
}
```

#### 2. ColorMap::getColor() Integration

**File:** `src/glview/ColorMap.cc:200`

```cpp
Color4f ColorMap::getColor(const ColorScheme& cs, const RenderColor rc)
{
  // Check session overrides first
  if (const auto override = RenderSettings::inst()->getColorOverride(rc)) {
    return *override;
  }

  // Normal scheme lookup
  if (cs.count(rc)) return cs.at(rc);
  if (ColorMap::inst()->defaultColorScheme().count(rc))
    return ColorMap::inst()->defaultColorScheme().at(rc);
  return {0, 0, 0, 127};
}
```

#### 3. Renderer Base Revision Tracking

**File:** `src/glview/Renderer.h`

```cpp
class Renderer
{
protected:
  void refreshColorSchemeIfDirty();  // NEW

  std::map<ColorMode, Color4f> colormap_;
  const ColorScheme *colorscheme_{nullptr};
  std::uint64_t color_override_revision_{0};  // NEW: Last known revision
};
```

**File:** `src/glview/Renderer.cc`

```cpp
void Renderer::setColorScheme(const ColorScheme& cs)
{
  PRINTD("setColorScheme");
  // Populate colormap_ from scheme (via ColorMap::getColor with overrides)
  colormap_[ColorMode::MATERIAL] = ColorMap::getColor(cs, RenderColor::OPENCSG_FACE_FRONT_COLOR);
  // ... more ColorModes

  colorscheme_ = &cs;
  color_override_revision_ = RenderSettings::inst()->colorOverrideRevision();  // NEW
}

void Renderer::refreshColorSchemeIfDirty()  // NEW
{
  const auto current_revision = RenderSettings::inst()->colorOverrideRevision();
  if (colorscheme_ && current_revision != color_override_revision_) {
    setColorScheme(*colorscheme_);  // Re-populate colormap_ with overrides
  }
}
```

#### 4. VBO Invalidation in Renderers

**OpenCSGRenderer** (`src/glview/preview/OpenCSGRenderer.cc`)

```cpp
void OpenCSGRenderer::setColorScheme(const ColorScheme& cs)
{
  Renderer::setColorScheme(cs);
  vertex_state_containers_.clear();  // NEW: Invalidate cached VBOs
}

void OpenCSGRenderer::prepare(const ShaderUtils::ShaderInfo *shaderinfo)
{
  refreshColorSchemeIfDirty();  // NEW: Check revision, clear cache if stale

  if (vertex_state_containers_.empty()) {
    // Rebuild VBOs with current colors (including overrides)
    createCSGVBOProducts(...);
  }
}
```

**Similar changes in:**
- `PolySetRenderer::setColorScheme()` / `prepare()`
- `ThrownTogetherRenderer::setColorScheme()` / `prepare()`
- `CGALRenderer::prepare()` (no setColorScheme override needed, uses base)

#### 5. Preferences Integration - UPDATED

**File:** `src/gui/Preferences.cc`

**When User Switches Schemes:**
```cpp
void Preferences::populate3DColorTable(const QString& schemeName)
{
  tableWidget3DColors->setRowCount(0);

  const RenderColorScheme *scheme = ColorMap::inst()->findRenderColorScheme(schemeName.toStdString());
  if (!scheme) return;

  // CRITICAL: Clear transient color overrides when switching schemes
  // This prevents stale edits from a previous scheme showing up
  RenderSettings::inst()->clearColorOverrides();

  // ... populate table rows from scheme ...
  // Each row: Name | RGB Display | "Pick..." Button
}
```

**When User Edits a Color:**
```cpp
void Preferences::onColor3DPickerClicked(int row, RenderColor colorKey,
                                         const QString& schemeName,
                                         const QColor& initialColor)
{
  // Show color picker dialog
  const QColor newColor = QColorDialog::getColor(initialColor, this, tr("Select color"));
  if (!newColor.isValid()) return;

  // Step 1: Update UI immediately (user sees change in table)
  if (auto *cell = tableWidget3DColors->cellWidget(row, 1)) {
    cell->setStyleSheet(QString("border: 3px solid rgb(%1,%2,%3); background: white; padding: 2px;")
                          .arg(newColor.red()).arg(newColor.green()).arg(newColor.blue()));
  }
  if (auto *btn = qobject_cast<QPushButton *>(tableWidget3DColors->cellWidget(row, 2))) {
    btn->setStyleSheet(QString("background: rgb(%1,%2,%3);")
                         .arg(newColor.red()).arg(newColor.green()).arg(newColor.blue()));
  }

  // Step 2: Set session override (NEW - bumps revision counter)
  Color4f newColorValue(newColor.redF(), newColor.greenF(), newColor.blueF());
  RenderSettings::inst()->setColorOverride(colorKey, newColorValue);

  // Step 3: Trigger redraw (NEW - causes renderers to invalidate VBOs)
  emit requestRedraw();
}
```

**How This Triggers Renderer Updates:**

```
User picks new color in Preferences
    → onColor3DPickerClicked()
        ├─ Update table cell styling (UI feedback)
        ├─ RenderSettings::setColorOverride(colorKey, newColor)
        │   ├─ color_overrides_[colorKey] = newColor
        │   └─ color_override_revision_++
        └─ emit requestRedraw()
            → MainWindow slot receives signal
            → qglview->update()
                → QWidget::update() (Qt internal)
                    → schedules paintEvent()
                        → GLView::paintGL()
                            ├─ if (renderer) renderer->prepare(shaderinfo)
                            │   → refreshColorSchemeIfDirty()
                            │       ├─ Check if color_override_revision_ is stale
                            │       └─ If stale: setColorScheme(*colorscheme_)
                            │           ├─ Recalculate colormap_ with overrides
                            │           ├─ Renderer subclass clears VBO cache
                            │           └─ Next createVBO will use new colors
                            ├─ if (containers_.empty())
                            │   └─ createVBOProducts() / createPolySetStates()
                            │       ├─ For each object:
                            │       │   └─ getColorSchemeColor(MATERIAL)
                            │       │       → colormap_[MATERIAL]
                            │       │           → Now has override color!
                            │       └─ Upload new VBOs to GPU
                            └─ renderer->draw()
                                → Render with new colors
```

**PolySetRenderer Special Handling (F6 Renders):**

The key fix for F6 renders was in `src/glview/PolySetRenderer.cc`:

```cpp
void PolySetRenderer::createPolySetStates(const ShaderUtils::ShaderInfo *shaderinfo)
{
  // ... buffer allocation ...

  for (const auto& polyset : this->polysets_) {
    Color4f color;

    // Use script-provided color if available, otherwise use scheme color
    // This enables live Preferences edits to affect F6 renders
    if (!polyset->colors.empty() && !polyset->color_indices.empty()) {
      // Has script colors (from color=[...] in OpenSCAD script)
      color = polyset->colors[0];
      getShaderColor(ColorMode::MATERIAL, color, color);
    } else {
      // No script colors: use current scheme color (with session overrides)
      // This allows live editing in Preferences to update colors
      getColorSchemeColor(ColorMode::MATERIAL, color);
    }

    add_shader_pointers(vbo_builder, shaderinfo);
    vbo_builder.writeSurface();
    vbo_builder.create_surface(*polyset, Transform3d::Identity(), color, enable_barycentric, false);
  }

  vbo_builder.createInterleavedVBOs();
}
```

And in `src/geometry/manifold/ManifoldGeometry.cc::toPolySet()`:

```cpp
// CRITICAL CHANGE:
// Do NOT pre-assign scheme colors (CGAL_FACE_FRONT/BACK) here.
// This would embed colors at evaluation time, preventing live updates.
// Instead, only store script-provided colors (from color=[...] in OpenSCAD).
// Renderer will apply current scheme colors (with overrides) at render time.

std::map<uint32_t, int32_t> originalIDToColorIndex;

auto getColorIndex = [&](uint32_t originalID) -> int32_t {
  // Only store script-provided colors (from color=[...] in script)
  auto colorIt = originalIDToColor_.find(originalID);
  if (colorIt == originalIDToColor_.end()) {
    // No script color: return -1 so renderer uses scheme colors
    return -1;
  }

  const auto& color = colorIt->second;
  auto pair = colorToIndex.insert({color, ps->colors.size()});
  if (pair.second) {
    ps->colors.push_back(color);
  }
  // ... return color index ...
};
```

---

## Implementation Details

### Files Modified

#### Core Override System
1. **src/glview/RenderSettings.h**
   - Added `color_overrides_` map
   - Added `color_override_revision_` counter
   - Added override API methods

2. **src/glview/RenderSettings.cc**
   - Implemented override storage and revision bumping

3. **src/glview/ColorMap.cc**
   - Modified `getColor()` to check overrides first

#### Renderer Base Infrastructure
4. **src/glview/Renderer.h**
   - Added `color_override_revision_` tracking
   - Added `refreshColorSchemeIfDirty()` method

5. **src/glview/Renderer.cc**
   - Track revision in `setColorScheme()`
   - Implemented `refreshColorSchemeIfDirty()`

#### Renderer Subclasses (VBO Invalidation)
6. **src/glview/PolySetRenderer.cc**
   - Clear cached containers in `setColorScheme()`
   - Call `refreshColorSchemeIfDirty()` in `prepare()`

7. **src/glview/cgal/CGALRenderer.cc**
   - Call `refreshColorSchemeIfDirty()` in `prepare()`

8. **src/glview/preview/OpenCSGRenderer.h/cc**
   - Override `setColorScheme()` to clear containers
   - Call `refreshColorSchemeIfDirty()` in `prepare()`

9. **src/glview/preview/ThrownTogetherRenderer.h/cc**
   - Override `setColorScheme()` to clear containers
   - Call `refreshColorSchemeIfDirty()` in `prepare()`

#### Preferences Integration
10. **src/gui/Preferences.cc**
    - Clear overrides in `populate3DColorTable()` (scheme switch)
    - Set override and emit `requestRedraw` in `onColor3DPickerClicked()`

### Revision-Based Invalidation Flow

```
User clicks color picker
    → QColorDialog opens
    → User selects new color
    → onColor3DPickerClicked()
        ├─ Update table cell styling (immediate UI feedback)
        ├─ RenderSettings::setColorOverride(colorKey, newColor)
        │   ├─ color_overrides_[colorKey] = newColor
        │   └─ color_override_revision_++  // NOW = 5
        └─ emit requestRedraw()
            → MainWindow receives signal
            → qglview->update()
                → QGLView::paintGL()
                    → renderer->prepare(shaderinfo)
                        → refreshColorSchemeIfDirty()
                            ├─ current_revision = RenderSettings::inst()->colorOverrideRevision()  // 5
                            ├─ if (current_revision != color_override_revision_)  // 5 != 3 → TRUE
                            │   └─ setColorScheme(*colorscheme_)
                            │       ├─ colormap_[MATERIAL] = ColorMap::getColor(cs, OPENCSG_FACE_FRONT)
                            │       │   → Checks override first, returns newColor
                            │       └─ color_override_revision_ = current_revision  // NOW = 5
                            └─ if (vertex_state_containers_.empty())  // TRUE (cleared by setColorScheme)
                                └─ create...VBOProducts()
                                    └─ Builds new VBOs with override colors
                    → renderer->draw(showedges, shaderinfo)
                        → Draws new VBOs with updated colors
```

### Why This Works

**Automatic Propagation:**
- Every renderer calls `ColorMap::getColor()` during VBO creation
- `getColor()` checks overrides first
- No changes needed to individual color lookup sites

**Performance:**
- Revision check is O(1) comparison
- VBO rebuild only happens when colors change
- Normal redraws (rotate, zoom) use cached VBOs

**Correctness:**
- Clearing overrides on scheme switch prevents stale edits
- Revision ensures no missed updates
- All renderers (OpenCSG, Manifold, CGAL) treated uniformly

## Known Limitations and Future Enhancements

### Known Limitation: Script-Provided Colors in F6 Renders

**Current Behavior:**
OpenSCAD scripts can set object colors using `color([R, G, B, A])`:
```scad
color([1, 0, 0])
  cube([10, 10, 10]);
```

These colors appear correctly in:
- **F5 OpenCSG Preview** ✓ (colors rendered as specified)

But NOT in F6 renders:
- **F6 Manifold Render** ✗ (uses scheme color, ignores script color)
- **F6 CGAL Render** ✗ (uses scheme color, ignores script color)

**Why This Limitation Exists:**

In `src/geometry/manifold/ManifoldGeometry.cc::toPolySet()`, script-provided colors are stored in `originalIDToColor_`. During conversion, the code should embed these per-polygon into the PolySet. Currently:

```cpp
// MANIFOLD: Colors stored but not embedded in geometry
auto colorIt = originalIDToColor_.find(originalID);
if (colorIt == originalIDToColor_.end()) {
  return -1;  // No script color
}
// Script color found, but only if we explicitly embed it:
ps->colors.push_back(colorIt->second);
```

The renderer (`PolySetRenderer`) checks `polyset->colors` and uses it if available. The issue is that script colors are either:
1. Not stored in `ps->colors` (current implementation)
2. Or stored but then overridden by the scheme color lookup

**Solution for Future Implementation:**

To enable script colors in F6 renders:

1. **In ManifoldGeometry::toPolySet():**
   ```cpp
   // Store script colors per-polygon in color_indices
   ps->color_indices.reserve(ps->indices.size());

   for (each polygon) {
     uint32_t originalID = mesh.runOriginalID[run];
     auto colorIt = originalIDToColor_.find(originalID);

     if (colorIt != originalIDToColor_.end()) {
       // Script color exists: embed it
       int32_t colorIndex = ps->colors.size();
       ps->colors.push_back(colorIt->second);
       ps->color_indices.push_back(colorIndex);
     } else {
       // No script color: mark as -1 (renderer will use scheme)
       ps->color_indices.push_back(-1);
     }
   }
   ```

2. **In PolySetRenderer::createPolySetStates():**
   ```cpp
   // Per-polygon coloring instead of uniform
   for (size_t i = 0; i < polyset->indices.size(); ++i) {
     Color4f polygonColor;

     if (!polyset->color_indices.empty() && polyset->color_indices[i] >= 0) {
       // Use script-provided color for this polygon
       polygonColor = polyset->colors[polyset->color_indices[i]];
       getShaderColor(ColorMode::MATERIAL, polygonColor, polygonColor);
     } else {
       // Use scheme color (with overrides)
       getColorSchemeColor(ColorMode::MATERIAL, polygonColor);
     }

     // Write this polygon with its color
     vbo_builder.createPolygon(polyset->indices[i], polygonColor, ...);
   }
   ```

3. **Advantage of This Approach:**
   - Script colors take priority
   - Scheme colors provide fallback
   - Session overrides don't override script colors (expected behavior)
   - Live preference edits still work for uncolored geometry

4. **Alternative: Per-Object Colors (Simpler)**
   If per-polygon is complex, use per-object approach:
   ```cpp
   // Store "overall object color" from first script color encountered
   if (!originalIDToColor_.empty()) {
     ps->colors.push_back(originalIDToColor_.begin()->second);
   }
   // In PolySetRenderer: use ps->colors[0] if present
   ```

---

## Future Considerations

### Potential Enhancements

1. **Script-Provided Colors in F6 Renders (HIGH PRIORITY)**
   - Allow `color([R, G, B, A])` to work in F6 as it does in preview
   - See "Known Limitations" section above for implementation details
   - Would eliminate need to export to external tools for texturing

2. **Persistent Overrides**
   - Save overrides to QSettings per-scheme
   - Load on scheme activation
   - "Save as new scheme" button to export JSON

3. **Undo/Redo Stack**
   - Track override history
   - Allow reverting to previous colors
   - Integrate with Qt's QUndoStack

4. **Real-Time 3D Preview**
   - Embed mini GLView in Preferences
   - Show sample geometry with current colors
   - Update live as colors change

5. **Color Picker Improvements**
   - HSV/HSL pickers
   - Color wheel interface
   - Eyedropper from 3D view
   - Named color presets

6. **Batch Operations**
   - "Brighten all" / "Darken all"
   - Hue shift entire scheme
   - Contrast adjustment

7. **Scheme Diff Viewer**
   - Compare two schemes side-by-side
   - Highlight differences
   - Merge/cherry-pick colors

### Known Limitations

1. **Session-Only Edits**
   - Overrides lost on app restart
   - No persistence mechanism yet
   - Workaround: Screenshot or note RGB values

2. **No Editor Preview Impact**
   - ScintillaEditor uses separate EditorColorScheme
   - 3D color edits don't affect editor
   - Separate tab for editor schemes

3. **VBO Rebuild Cost**
   - Complex geometry takes time to rebuild
   - No incremental updates
   - Could batch updates (debounce signal)

4. **No Override Indicator**
   - Table doesn't show which colors are overridden
   - No visual distinction from scheme defaults
   - Could add icon or bold text

5. **Background/Axes Direct Draw**
   - These use immediate mode, update live already
   - Inconsistent with VBO-cached geometry behavior
   - But works correctly with overrides

### Testing Recommendations

**Manual Test Cases:**

1. **Live Update - OpenCSG Preview**
   - Open any .scad file
   - F5 to preview
   - Preferences → 3D Colors → Pick "CGAL Face Front"
   - Change color → OK
   - **Expected:** Preview faces update immediately (no F5 needed)

2. **Live Update - Manifold Render**
   - F6 to render with Manifold
   - Change "CGAL Face Front" color
   - **Expected:** Render updates immediately (no F6 needed)

3. **Scheme Switch Clears Overrides**
   - Set override on "Cornfield" scheme
   - Switch to "Tomorrow Night"
   - Switch back to "Cornfield"
   - **Expected:** Override lost, default color restored

4. **Multiple Overrides**
   - Set 3-4 different color overrides
   - **Expected:** All update simultaneously, no conflicts

5. **Edge Colors**
   - Set override on "CGAL Edge Front"
   - Enable "Show Edges" in View menu
   - **Expected:** Edge outlines update immediately

**Automated Test Cases (Future):**

```cpp
TEST_CASE("RenderSettings override revision tracking") {
  auto* rs = RenderSettings::inst();
  auto rev1 = rs->colorOverrideRevision();

  rs->setColorOverride(RenderColor::CGAL_FACE_FRONT_COLOR, Color4f(1, 0, 0));
  auto rev2 = rs->colorOverrideRevision();
  REQUIRE(rev2 > rev1);

  rs->clearColorOverrides();
  auto rev3 = rs->colorOverrideRevision();
  REQUIRE(rev3 > rev2);
}

TEST_CASE("ColorMap::getColor respects overrides") {
  auto* rs = RenderSettings::inst();
  rs->clearColorOverrides();

  auto scheme = ColorMap::inst()->defaultColorScheme();
  auto defaultColor = ColorMap::getColor(scheme, RenderColor::CGAL_FACE_FRONT_COLOR);

  Color4f override(1, 0, 0);
  rs->setColorOverride(RenderColor::CGAL_FACE_FRONT_COLOR, override);

  auto overriddenColor = ColorMap::getColor(scheme, RenderColor::CGAL_FACE_FRONT_COLOR);
  REQUIRE(overriddenColor == override);
  REQUIRE(overriddenColor != defaultColor);
}
```

---

## Edge Rendering Architecture: CGAL vs Manifold

### Current State (December 2024)

Edge rendering differs significantly between the CGAL and Manifold backends:

#### CGAL Backend (F6 with CGAL)

**Architecture:**
- Uses `VBOPolyhedron` class (src/glview/cgal/VBOPolyhedron.h)
- Explicit edge rendering via `draw(Edge_iterator)` method
- Edge colors set at polyhedron construction time
- Two edge color indices:
  - `MARKED_EDGE_COLOR` → mapped to `CGAL_EDGE_BACK_COLOR`
  - `UNMARKED_EDGE_COLOR` → mapped to `CGAL_EDGE_FRONT_COLOR`

**VBOPolyhedron Constructor:**
```cpp
VBOPolyhedron(const ColorScheme& cs) {
  setColor(CGALColorIndex::MARKED_EDGE_COLOR,
           ColorMap::getColor(cs, RenderColor::CGAL_EDGE_BACK_COLOR));
  setColor(CGALColorIndex::UNMARKED_EDGE_COLOR,
           ColorMap::getColor(cs, RenderColor::CGAL_EDGE_FRONT_COLOR));
}
```

**Edge Drawing:**
```cpp
void draw(Edge_iterator e, VBOBuilder& vbo_builder) const {
  const Double_point p = e->source(), q = e->target();
  const CGAL::Color c = getEdgeColor(e);
  const Color4f color(c.red(), c.green(), c.blue());
  vbo_builder.createVertex({Vector3d(p.x(), p.y(), p.z())}, {}, color, 0, 0, true);
  vbo_builder.createVertex({Vector3d(q.x(), q.y(), q.z())}, {}, color, 0, 1, true);
}
```

**Override Support:**
- ✅ **FIXED:** CGALRenderer now detects override revision changes in `prepare()`
- Clears `polyhedrons_` cache when overrides change
- Recreates VBOPolyhedrons with new colors via ColorMap::getColor()
- Edge colors update immediately when CGAL_EDGE_FRONT/BACK overrides are set

**Code Location:**
- src/glview/cgal/CGALRenderer.cc (prepare method)
- src/glview/cgal/VBOPolyhedron.h (constructor and draw)

#### Manifold Backend (F6 with Manifold)

**Architecture:**
- Uses `PolySetRenderer` class (src/glview/PolySetRenderer.cc)
- **Shader-based edge rendering** via ViewEdges.frag/vert
- No explicit edge geometry creation
- Edge colors are **derived from face colors** by the shader

**Edge Shader Algorithm (shaders/ViewEdges.frag):**
```glsl
void main(void) {
  // Lighten the face color for edges
  vec4 color_edge = vec4((color.rgb + vec3(1))/2, 1.0);

  // Mix between edge color and face color based on barycentric coords
  gl_FragColor = mix(color_edge, vec4(color.rgb * shading, color.a), edgeFactor());
}
```

**Key Insight:**
The shader receives only the **face color** and automatically generates edge color by:
1. Adding vec3(1) to face RGB (lightens color)
2. Dividing by 2 (brings back to [0,1] range)
3. Mixing with face color based on proximity to edge (barycentric interpolation)

**Current Limitation:**
- ❌ **EDGE OVERRIDES NOT SUPPORTED:** CGAL_EDGE_FRONT_COLOR and CGAL_EDGE_BACK_COLOR preferences have no effect
- Edge colors are always auto-derived from face colors
- No separate edge color uniform passed to shader
- No way to independently control edge colors via preferences

**Visual Effect:**
- Front face edges: Automatically lightened version of CGAL_FACE_FRONT_COLOR
- Back face edges: Automatically lightened version of CGAL_FACE_BACK_COLOR
- User cannot override edge colors independently

### Problem Statement

Users expect `CGAL_EDGE_FRONT_COLOR` and `CGAL_EDGE_BACK_COLOR` preferences to control edge colors in both CGAL and Manifold backends. Currently:
- ✅ CGAL backend: Edge colors respect overrides (after fix)
- ❌ Manifold backend: Edge colors ignore overrides (auto-derived from face colors)

### Solution Approaches

#### Case 1: Modify Shader to Support Explicit Edge Colors (Recommended)

**Implementation Plan:**

1. **Update Shader Uniforms:**
   - Add uniform vec4 `edgeColorFront` to ViewEdges.frag
   - Add uniform vec4 `edgeColorBack` to ViewEdges.frag
   - Pass cutout flag to fragment shader to select appropriate edge color

2. **Modify VBO Data:**
   - Add per-vertex attribute indicating front vs back face
   - Current VBO: position, normal, color, barycentric, shading
   - New VBO: position, normal, color, barycentric, shading, **isCutout**

3. **Update PolySetRenderer:**
   - Set shader uniforms in `drawPolySets()` method:
     ```cpp
     if (enable_shader) {
       Color4f edgeFront, edgeBack;
       getColorSchemeColor(ColorMode::MATERIAL_EDGES, edgeFront);
       getColorSchemeColor(ColorMode::CUTOUT_EDGES, edgeBack);
       glUniform4fv(shader_edge_front_loc, 1, edgeFront.data());
       glUniform4fv(shader_edge_back_loc, 1, edgeBack.data());
     }
     ```

4. **Update Fragment Shader Logic:**
   ```glsl
   uniform vec4 edgeColorFront;
   uniform vec4 edgeColorBack;
   varying float isCutout;

   void main(void) {
     // Select edge color based on face type
     vec4 color_edge = mix(edgeColorFront, edgeColorBack, isCutout);

     // Mix between explicit edge color and face color
     gl_FragColor = mix(color_edge, vec4(color.rgb * shading, color.a), edgeFactor());
   }
   ```

5. **Update VBOBuilder:**
   - Extend `create_surface()` to accept and store cutout flag per-triangle
   - Add cutout flag to vertex attributes when barycentric is enabled
   - Update shader attribute binding in VBOUtils

**Advantages:**
- ✅ User gets full control over edge colors
- ✅ Consistent behavior across CGAL and Manifold backends
- ✅ Preferences CGAL_EDGE_FRONT/BACK become functional
- ✅ Overrides work immediately with existing revision tracking

**Disadvantages:**
- More complex shader
- Additional per-vertex data (1 float per vertex)
- Requires VBO rebuild when edge colors change (already handled by revision tracking)

**Estimated Effort:** ~2-3 hours
**Files Modified:** 4-5 (shader files, PolySetRenderer.cc, VBOBuilder.cc/h, ShaderUtils.cc)

#### Case 2: Document Current Limitation (Not Recommended)

**Implementation Plan:**

1. **Update Documentation:**
   - Add note in Preferences UI tooltip: "Edge colors only apply to CGAL backend (F6)"
   - Update this architecture doc with limitation
   - Add comment in PolySetRenderer about shader-derived edge colors

2. **Disable Edge Color Pickers for Manifold:**
   - Detect active backend (requires plumbing backend state to Preferences)
   - Gray out edge color pickers when Manifold is active
   - Show explanatory tooltip

**Advantages:**
- ✅ No code changes to rendering pipeline
- ✅ Zero risk of regression

**Disadvantages:**
- ❌ Inconsistent user experience between backends
- ❌ Preferences UI has non-functional controls
- ❌ User cannot achieve desired visual style with Manifold
- ❌ Confusing behavior (why do some color pickers work and others don't?)

**Estimated Effort:** ~30 minutes
**User Impact:** Negative - feature appears broken

### Recommendation

**Implement Case 1** (Modify Shader for Explicit Edge Colors)

**Reasoning:**
1. Users expect color preferences to work consistently
2. Shader modification is straightforward and low-risk
3. Existing revision tracking infrastructure handles cache invalidation
4. Better user experience and consistent backend behavior
5. Opens door for future edge styling features (thickness, style, etc.)

**Next Steps:**
1. Commit current state with tag "faceColorPrefsKnownGood"
2. Implement shader modifications for explicit edge colors
3. Test with both CGAL and Manifold backends
4. Verify override system works for all color types
5. Update UI testing checklist

---

## Conclusion

This implementation enables live color scheme editing in OpenSCAD's Preferences dialog without breaking the existing architecture. The key insights were:

1. **Colors are cached in VBOs**, requiring invalidation for updates
2. **Renderers live across multiple redraws**, requiring state tracking
3. **Scheme immutability is desirable**, requiring an override layer
4. **Revision tracking is efficient**, enabling automatic invalidation

The solution is minimal, transparent, and extends naturally to future features like persistent overrides and batch operations. The rendering architecture documentation here serves as a foundation for understanding how OpenSCAD's multi-backend system works and how color data flows through the pipeline.

**Total Lines of Code Added:** ~100
**Files Modified:** 10
**Core Concepts:** Session overrides, revision tracking, VBO invalidation
**User Impact:** Live color editing in Preferences (previously impossible)
