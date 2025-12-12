# OpenSCAD Parameter Widgets - Library Architecture Study

## Overview

The parameter widgets subsystem (src/gui/parameter/) forms a well-structured, extensible framework for creating parameterized interfaces. This architecture is production-ready for library extraction with minimal refactoring.

## Core Architecture

### Hierarchy & Class Relationships

```cpp
QWidget (Qt Framework)
├── ParameterWidget
│   ├── Manages parameter sets and SCAD files
│   ├── Orchestrates parameter discovery
│   ├── Delegates widget creation to factory method
│   ├── Manages auto-preview and preview timing
│   └── Signals parameter changes to parent
│
├── ParameterVirtualWidget (Abstract Base)
│   ├── Pure virtual: setValue()
│   ├── Virtual: valueApplied()
│   ├── Signals: changed(bool immediate)
│   └── Base implementations:
│       ├── ParameterCheckBox (BoolParameter)
│       ├── ParameterComboBox (EnumParameter)
│       ├── ParameterSlider (NumberParameter with range)
│       ├── ParameterSpinBox (NumberParameter without range)
│       ├── ParameterText (StringParameter)
│       └── ParameterVector (VectorParameter)
│
├── ParameterDescriptionWidget
│   ├── Renders parameter metadata
│   ├── Supports 4 description styles
│   └── Formats parameter names (underscores→spaces)
│
└── GroupWidget
    ├── Collapsible container
    ├── Expandable/collapsible state management
    └── Content layout organization
```

## Class Responsibilities

### 1. ParameterWidget (Container & Orchestrator)

**File:** ParameterWidget.h/cc, ParameterWidget.ui

**Role:** Top-level container managing the entire parameterization system

**Key Members:**

```cpp
ParameterSets sets;  // JSON preset storage
std::string source;  // Current SCAD source code
ParameterObjects parameters;  // Parsed parameters from source
std::map<ParameterObject*, std::vector<ParameterVirtualWidget*>> widgets;
QTimer autoPreviewTimer;
bool modified;
```

**Key Methods:**

- `readFile(QString)` - Load preset JSON file
- `saveFile(QString)` - Persist presets to JSON
- `setParameters(SourceFile, string)` - Initialize from SCAD source
- `applyParameters(SourceFile)` - Apply current values back to SCAD
- `createParameterWidget()` - Factory method (parameter type → widget)
- `getParameterGroups()` - Group parameters by category

**Signal Flow:**

```cpp
Preset Changed → onSetChanged() → loadSet()
               ↓
Parameter Modified → parameterModified() → autoPreview(timeout)
               ↓
autoPreviewTimer fires → emitParametersChanged() → [signal to parent]
```

**Slots (Public Interface):**

- `setModified(bool)` - Mark preset as dirty
- `setFontFamilySize(QString, uint)` - Apply font settings
- `autoPreview(bool immediate)` - Trigger preview with optional timeout

### 2. ParameterVirtualWidget (Abstract Base)

**File:** ParameterVirtualWidget.h/cc

**Role:** Abstract interface for all parameter input widgets

**Core Contract:**

```cpp
class ParameterVirtualWidget : public QWidget {
  virtual void setValue() = 0;  // Sync value from parameter object to UI
  virtual void valueApplied() {} // Notify after preview update
  signals: void changed(bool immediate);
};
```

**Subclasses Pattern:**
Each subclass follows identical pattern:

1. Store typed parameter pointer (e.g., `BoolParameter *`)
2. Implement `setValue()` to read from parameter & update UI
3. Connect UI change signals to slots
4. Emit `changed()` signal with `immediate` flag

**Immediate vs. Delayed Changes:**

- **Immediate:** Checkbox, ComboBox (don't need `valueApplied()`)
- **Delayed:** Slider, SpinBox, Text, Vector (track `lastSent` vs `lastApplied`)

**Change Tracking Pattern:**

```cpp
boost::optional<T> lastSent;     // Value we last sent to preview
boost::optional<T> lastApplied;  // Value confirmed by preview update
// Use valueApplied() to sync lastApplied = lastSent
// Prevents redundant change signals
```

**Static Utility Methods:**

```cpp
static int decimalsRequired(double value);
static int decimalsRequired(const std::vector<double>& values);
// Calculate minimum decimal places to represent a value
```

### 3. ParameterDescriptionWidget

**File:** ParameterVirtualWidget.h/cc (defined in same file)

**Role:** Render parameter metadata and description

**Description Styles (enum DescriptionStyle):**

1. **ShowDetails** - Description below parameter name
2. **Inline** - Description on same line after parameter (with " - ")
3. **HideDetails** - Description hidden, shown in tooltip
4. **DescriptionOnly** - Only description text (parameter name hidden)

**Features:**

- Converts underscores to spaces in parameter names
- Handles empty descriptions gracefully
- Supports dynamic style switching

### 4. GroupWidget

**File:** GroupWidget.h/cc

**Role:** Collapsible container for parameter groups

**Features:**

```cpp
QGridLayout mainLayout;
QToolButton toggleButton;     // Expand/collapse control
QWidget contentArea;
QVBoxLayout contentLayout;    // Content container
```

**Key Methods:**

- `addWidget(QWidget*)` - Add widget to group
- `setExpanded(bool)` - Programmatic expand/collapse
- `isExpanded() const`
- `title() const`

**Visual Structure:**

```text
┌─────────────────────────┐
│ ▼ Group Title [▼ button]│
├─────────────────────────┤
│ Widget 1                │
│ Widget 2                │
│ Widget 3                │
└─────────────────────────┘
```

## Concrete Parameter Widget Implementations

### ParameterCheckBox

**Binds:** BoolParameter (true/false)
**Control:** QCheckBox
**Change Mode:** Immediate

```cpp
void setValue() override;     // Read parameter.value() → setCheckState()
protected slots:
  void onChanged();           // Emit changed(true) immediately
```

### ParameterComboBox

**Binds:** EnumParameter (enumerated values)
**Control:** QComboBox with preset options
**Change Mode:** Immediate

```cpp
void setValue() override;     // Set current index from parameter.value()
protected slots:
  void onChanged(int index);  // Emit changed(true) immediately
```

### ParameterSlider

**Binds:** NumberParameter with range constraints
**Controls:** QSlider + QDoubleSpinBox (dual input)
**Change Mode:** Delayed with tracking

```cpp
void setValue() override;                 // Sync slider & spinbox
void valueApplied() override;             // Update lastApplied
protected slots:
  void onSliderPressed/Released/Moved();  // Range feedback with delayed emit
  void onSpinBoxChanged/EditingFinished();// Immediate on value change
```

**Logic:**

- Slider movement: emit delayed (waits for release)
- Spinbox edit: emit immediate (on finished)
- Tracks `lastSent`/`lastApplied` to avoid redundant signals

### ParameterSpinBox

**Binds:** NumberParameter (unbounded or with step)
**Control:** QDoubleSpinBox
**Change Mode:** Delayed

```cpp
void setValue() override;           // Set spinbox value from parameter
void valueApplied() override;       // Sync lastApplied
protected slots:
  void onChanged(double);           // Emit delayed (with timeout)
  void onEditingFinished();         // Mark as sent
```

### ParameterText

**Binds:** StringParameter
**Control:** QLineEdit (or QTextEdit variant)
**Change Mode:** Delayed

```cpp
void setValue() override;           // Set text from parameter
void valueApplied() override;       // Sync lastApplied
protected slots:
  void onEdit(const QString&);      // Live updating
  void onEditingFinished();         // Finalize on focus loss
```

**Tracking:**

```cpp
boost::optional<std::string> lastSent;
boost::optional<std::string> lastApplied;
```

### ParameterVector

**Binds:** VectorParameter (multi-component [x, y, z, ...])
**Controls:** Array of QDoubleSpinBox (one per component)
**Change Mode:** Delayed

```cpp
void setValue() override;           // Update all spinboxes
void valueApplied() override;       // Sync all lastApplied
protected slots:
  void onChanged();                 // Any spinbox change → delayed emit
  void onEditingFinished();         // Mark all as sent
```

**Storage:**

```cpp
std::vector<QDoubleSpinBox*> spinboxes;  // One per dimension
std::vector<double> lastSent;             // Track per-component
std::vector<double> lastApplied;
```

## Factory Pattern & Widget Creation

**ParameterWidget::createParameterWidget()**

```text
Input: ParameterObject* → [Type check via virtual methods]
       ↓
Determine Type ──→ BoolParameter → ParameterCheckBox
       │      ├─→ EnumParameter → ParameterComboBox
       │      ├─→ NumberParameter (with range) → ParameterSlider
       │      ├─→ NumberParameter (no range) → ParameterSpinBox
       │      ├─→ StringParameter → ParameterText
       │      └─→ VectorParameter → ParameterVector
       ↓
Create Description Widget with chosen style
       ↓
Return: ParameterVirtualWidget*
```

## Data Flow

### Reading (SCAD → UI)

```text
1. ParameterWidget::setParameters(source)
   ├─ ParameterObjects::fromSourceFile(source)
   └─ rebuildWidgets()

2. rebuildWidgets()
   ├─ For each ParameterObject in groups:
   ├─ createParameterWidget(ParameterObject)
   │  └─ [Factory creates typed widget]
   └─ Connect widget.changed() → parameterModified()

3. loadSet(index)
   ├─ Load preset values from JSON
   ├─ For each widget in widgets map:
   └─ widget→setValue()
```

### Writing (UI → SCAD)

```text
1. User changes widget value
   ↓
2. Widget emits changed(immediate)
   ↓
3. ParameterWidget::parameterModified(immediate)
   ├─ If immediate=false, start autoPreviewTimer
   └─ If immediate=true, call emitParametersChanged()

4. autoPreviewTimer fires (1s delay)
   ↓
5. ParameterWidget::emitParametersChanged()
   ├─ For each widget: widget→valueApplied()
   ├─ Emit parametersChanged() signal
   └─ Parent applies changes to SCAD model
```

### Saving Presets

```text
1. User modifies parameter set name/creates new set
   ↓
2. ParameterWidget detects modification
   ├─ setModified(true)
   └─ Emit modificationChanged() signal

3. On close/save:
   ├─ cleanSets() - Remove empty sets
   └─ sets.writeFile(json_path)
```

## Key Design Patterns

### 1. Strategy Pattern (ParameterVirtualWidget)

- Abstract base defines interface
- Concrete widgets implement type-specific behavior
- Polymorphic behavior through virtual functions

### 2. Factory Pattern (createParameterWidget)

- Central factory determines widget type from parameter
- Encapsulates type-specific instantiation logic
- Single point of change for adding new parameter types

### 3. Observer Pattern (Qt Signals/Slots)

- Widgets emit `changed()` on user input
- Parent listens and coordinates preview updates
- Decouples widgets from parent logic

### 4. State Pattern (ParameterWidget with sets)

- Multiple parameter sets stored in memory
- Switch between sets via preset selector
- Apply/revert without modifying SCAD file

### 5. Template Method (Widget Implementation)

- All concrete widgets follow identical structure:

  ```text
  Constructor → Setup UI → Connect signals
  setValue() → Read parameter → Update UI
  onXxxx() → Capture change → Emit signal
  ```

## Dependencies & Integrations

### Internal Dependencies

```text
ParameterWidget
├─ ParameterVirtualWidget (inherited by all widgets)
├─ ParameterCheckBox/ComboBox/Slider/SpinBox/Text/Vector
├─ GroupWidget
├─ ParameterDescriptionWidget
└─ [Core parameter objects]
    ├─ ParameterObject (base)
    ├─ BoolParameter
    ├─ EnumParameter
    ├─ NumberParameter
    ├─ StringParameter
    └─ VectorParameter

Core dependencies (in core/customizer/):
├─ ParameterObject.h - Base parameter definition
├─ ParameterSet.h - JSON preset storage/loading
└─ SourceFile - SCAD source representation
```

### UI Framework Dependencies

```cpp
Qt5/Qt6
├─ QWidget - All UI widgets inherit
├─ QCheckBox - Boolean input
├─ QComboBox - Enumeration input
├─ QSlider + QDoubleSpinBox - Numeric range input
├─ QLineEdit - String input
├─ QGridLayout, QVBoxLayout - Layout management
├─ QToolButton - Collapse/expand toggle
└─ Qt Signals/Slots mechanism
```

## Extensibility Points

### Adding a New Parameter Type

1. Create `ParameterXxxType` in core (if needed)
2. Create `ParameterXxx : public ParameterVirtualWidget`
3. Implement `setValue()` and signal handling
4. Add case to `createParameterWidget()` factory
5. Create `ParameterXxx.ui` file with Qt Designer

### Customizing Behavior

- **Change preview delay:** Modify `autoPreviewTimer.setInterval()`
- **Change description style:** Preset in `createParameterWidget()` or add UI control
- **Change group behavior:** Subclass `GroupWidget`
- **Custom validation:** Override `setValue()` with validation logic

## Strengths as a Library

✓ **Well-defined interfaces** - Abstract base + concrete implementations
✓ **Minimal external coupling** - Core dependencies are stable
✓ **Reusable patterns** - Widget structure is consistent and learnable
✓ **Type-safe** - Template-free, works with polymorphism
✓ **Composable** - Widgets combine into groups into containers
✓ **Signals-based** - Parent-agnostic, easy integration points
✓ **Visual composition** - UI files (.ui) separate from logic
✓ **State management** - Built-in preset system

## Refactoring Notes for Library Extraction

### Low Priority

- [ ] Extract parameter types to separate header
- [ ] Create ParameterWidgetFactory class (instead of method)
- [ ] Add documentation comments to public interfaces

### Medium Priority

- [ ] Decouple from OpenSCAD Preferences (currently uses GlobalPreferences)
- [ ] Extract JSON serialization to pluggable backend
- [ ] Add unit tests for each widget type

### High Priority

- [ ] Namespace the library (e.g., `openscad::parameters::`)
- [ ] Define stable public API version
- [ ] Create library CMakeLists.txt with proper exports
- [ ] Document parameter type requirements/contracts

## Summary

The parameter widgets subsystem is a **well-architected, modular framework** suitable for:

- ✓ Extraction as standalone library
- ✓ Usage in other Qt applications
- ✓ Educational reference for Qt widget patterns
- ✓ Plugin extension for new parameter types

The architecture cleanly separates:

- **Container logic** (ParameterWidget) - preset management, widget orchestration
- **Widget logic** (ParameterVirtualWidget subclasses) - type-specific UI behavior
- **Presentation** (GroupWidget, ParameterDescriptionWidget) - layout and styling

With minimal refactoring, this could serve as a reusable "Parameterized Dialog Framework" library for any Qt application needing dynamic parameter configuration.

---

## SCAD Script Integration & Customizer Context

### Complete Three-Tier Architecture

The parameter widgets are the **UI presentation layer** of a three-tier system:

```cpp

┌─────────────────────────────────────────────────────────┐
│ 1. SCAD Script Layer (Input - Comment Annotations)     │
│    ├─ Variable declarations with inline comments       │
│    ├─ Comment syntax defines widget spec               │
│    ├─ Group headers: /*[ group name ]*/                │
│    └─ Parser stops at first local scope                │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│ 2. Parameter Object Layer (Data Model)                 │
│    ├─ ParameterObjects::fromSourceFile() parses        │
│    ├─ Creates typed ParameterObject instances          │
│    │  ├─ BoolParameter (true/false)                    │
│    │  ├─ NumberParameter (numeric ± range)             │
│    │  ├─ EnumParameter ([list] or [val:label])         │
│    │  ├─ StringParameter                               │
│    │  └─ VectorParameter (2-4 elements w/ constraints) │
│    └─ Each stores: name, value, constraints, desc      │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│ 3. Widget Layer (UI - Documented Above)                │
│    ├─ createParameterWidget() factory                  │
│    ├─ Widget renders UI based on constraints           │
│    ├─ User changes → parameter value updated           │
│    └─ Change triggers recompile (if auto-preview ON)   │
└─────────────────────────────────────────────────────────┘
```

### Customizer Declaration Syntax

Variables become customizer parameters via **comment annotations**:

```scad
//Description
variable = value; // [Parameter Widget Spec]

// Group header comment defines collapsible sections
/*[ Group Name ]*/

/*[ Combo Boxes ]*/
// select a number
Numbers = 2; // [0, 1, 2, 3, 4]

// select a string (strings don't need quotes in comment specs)
Strings = "foo"; // [foo, bar, baz]

// labeled combo box for numbers
Labeled_Values = 10; // [5:S, 10:L, 20:M, 30:XL]

// labeled combo box for strings
Labeled_Strings = "S"; // [S:Small, M:Medium, L:Large]

/*[ Sliders ]*/
// slider widget for number
slider = 34; // [10:100]

// step slider for number
stepSlider = 2; // [0:5:100]  // min:step:max

// Vector with shared constraints (2-4 elements)
position = [0, 0, 0]; // [0:1:10]  // all elements limited to [0,1,10]
```

### Comment Spec Syntax Rules

**Type Inference:**

- Variable's **assigned value determines type**
- `= true/false` → BoolParameter
- `= 42` → NumberParameter
- `= "text"` → StringParameter
- `= [x, y]` or `[x, y, z]` or `[x, y, z, w]` → VectorParameter

**Constraint Specifications:**

| Comment Pattern | Widget Type | Meaning |
|----------------|-------------|---------|
| `// [min:max]` | Slider | Range from min to max, step = 1 |
| `// [min:step:max]` | Slider | Range with explicit step |
| `// [val1, val2, ...]` | ComboBox | Enumeration (numeric or string) |
| `// [val1:label1, val2:label2]` | ComboBox | Labeled enumeration |
| (no spec) | SpinBox or Text | Unbounded numeric or string |

**String Literals in Comments:**

- Strings **don't need quotes** in comment specs
- Parser treats anything between delimiters as string
- If all numeric digits → number, else → string
- Mixed type vectors allowed: `[fred, 33, wilma, 32]`

**Vector Constraints:**

- Single range spec `[0:1:10]` applies to **all elements**
- All elements get individual spinboxes with same limits
- Vector length fixed by initial value (2, 3, or 4 elements only)

### Parsing Scope Rules

**Parser reads declarations until first local scope:**

- Function definition → stops
- Module definition → stops
- Module invocation → stops
- Expression statement → stops

Only **top-level variable assignments** with comment specs become customizer parameters.

### Recompile-on-Change Behavior

Unlike typical GUI forms, customizer changes trigger **full script recompilation**:

1. User modifies widget value
2. Widget emits `changed(immediate)` signal
3. ParameterWidget updates ParameterObject value
4. If auto-preview enabled:
   - Immediate change → compile now
   - Delayed change → wait 1s (autoPreviewTimer)
5. New values written back to SCAD variable assignments
6. Entire script recompiles with new values
7. 3D view updates with recompiled model

This is **not real-time** - each change is a discrete compile cycle.

## Extension for Color & New Object Types

### Adding Color Parameter Support

**Design Goal:** Enable color pickers in customizer using QwwColorComboBox

**Option 1: String-Based (Quickest)**

Treat colors as StringParameter with special tag:

```scad
myColor = "#FF0000";  // [color]
// or with palette
myColor = "#FF0000";  // [color:#FF0000, #00FF00, #0000FF]
```

**Parser extension:**

- Detect `[color...]` in comment
- Create StringParameter with color constraint metadata
- Factory recognizes color constraint → creates ParameterColor widget

**Option 2: New ColorParameter Type (Cleanest)**

Create dedicated parameter type:

```cpp
// In core/customizer/ParameterObject.h
class ColorParameter : public ParameterObject {
public:
  Color value;  // Internal representation (RGB/RGBA)
  std::vector<Color> presets;  // Optional palette
  bool includeAlpha;

  std::string toScadValue() override;  // Serialize to SCAD syntax
  void fromScadValue(const std::string&) override;
};
```

```cpp
// In gui/parameter/ParameterColor.h
class ParameterColor : public ParameterVirtualWidget, Ui::ParameterColor {
  Q_OBJECT
public:
  ParameterColor(QWidget *parent, ColorParameter *parameter,
                 DescriptionStyle descriptionStyle);
  void setValue() override;
  void valueApplied() override;

protected slots:
  void onColorChanged(const QColor& color);

private:
  ColorParameter *parameter;
  QwwColorComboBox *colorCombo;  // Your integrated widget!
  boost::optional<Color> lastSent;
  boost::optional<Color> lastApplied;
};
```

**Comment Syntax Options:**

```scad
// Hex notation
myColor = "#FF0000";  // [color]
myColor = "#FF0000";  // [color:#FF0000, #00FF00, #0000FF]

// Named colors
myColor = "red";  // [color:red, green, blue, yellow]

// RGB vector treated as color
myColor = [255, 0, 0];  // [color:rgb] or [color:0-255]

// RGBA with alpha
myColor = [1.0, 0.0, 0.0, 0.5];  // [color:rgba:0-1]
```

### Implementation Path

**Step 1: Locate Parser Code**

Find `ParameterObjects::fromSourceFile()` implementation:

- Likely in `core/customizer/ParameterObject.cc` or `ParameterObjects.cc`
- Study existing type detection logic
- Identify where comment specs are parsed

**Step 2: Define Color Syntax**

Choose syntax convention:

- Decide on representation (hex string vs vector vs both)
- Define palette specification syntax
- Consider alpha channel support

**Step 3: Extend Parser**

```cpp
// Pseudocode for parser extension
if (commentSpec.startsWith("[color")) {
  // Parse color constraints/presets
  auto colorParam = new ColorParameter(varName, initialValue);

  // Extract palette if specified: [color:#FF0000, #00FF00]
  colorParam->presets = parseColorList(commentSpec);

  parameters.push_back(colorParam);
  continue;
}
```

## Step 4: Create ParameterColor Widget

```cpp
ParameterColor::ParameterColor(QWidget *parent, ColorParameter *param,
                               DescriptionStyle style)
  : ParameterVirtualWidget(parent, param), parameter(param) {

  setupUi(this);

  // Initialize QwwColorComboBox
  colorCombo = new QwwColorComboBox(this);
  layout()->addWidget(colorCombo);

  // Add preset colors if specified
  for (const Color& preset : param->presets) {
    colorCombo->addColor(qColorFromScadColor(preset));
  }

  // Setup description widget
  descriptionWidget->setDescription(param, style);

  // Connect signals
  connect(colorCombo, &QwwColorComboBox::colorChanged,
          this, &ParameterColor::onColorChanged);

  setValue();
}

void ParameterColor::setValue() {
  QColor qtColor = qColorFromScadColor(parameter->value);
  colorCombo->setCurrentColor(qtColor);
}

void ParameterColor::onColorChanged(const QColor& color) {
  parameter->value = scadColorFromQColor(color);
  lastSent = parameter->value;
  emit changed(true);  // Colors are immediate change
}
```

**Step 5: Extend Factory**

```cpp
// In ParameterWidget.cc
ParameterVirtualWidget* ParameterWidget::createParameterWidget(
  ParameterObject *parameter, DescriptionStyle descriptionStyle) {

  // ... existing type checks ...

  if (auto colorParam = dynamic_cast<ColorParameter*>(parameter)) {
    return new ParameterColor(this, colorParam, descriptionStyle);
  }

  // ... rest of factory ...
}
```

**Step 6: Test & Iterate**

```scad
// Test script
/*[ Colors ]*/
// Basic color picker
fillColor = "#FF0000";  // [color]

// Color with palette
borderColor = "#000000";  // [color:#FF0000, #00FF00, #0000FF, #000000]

// Named colors
textColor = "black";  // [color:red, green, blue, black, white]
```

### Pattern for Additional Object Types

The color extension establishes a pattern for **any new object type**:

1. **Define comment syntax** - e.g., `// [object:type:constraints]`
2. **Create ParameterObject subclass** - Store object-specific data
3. **Extend parser** - Recognize syntax → instantiate typed parameter
4. **Create ParameterVirtualWidget subclass** - Appropriate Qt control(s)
5. **Add factory case** - Map parameter type → widget type
6. **Implement serialization** - Convert between SCAD syntax ↔ internal representation

**Example: File/Path Objects**

```scad
inputFile = "";  // [file:*.stl,*.obj]
outputPath = "./";  // [directory]
```

**Example: Complex Objects**

```scad
transform = {position: [0,0,0], rotation: [0,0,0], scale: 1.0};  // [transform]
// Would need custom widget with position vector, rotation vector, scale spinbox
```

### Integration with QwwColorComboBox

Your wwWidgets integration is **perfectly timed** for this:

- QwwColorComboBox provides the color picker dialog
- Supports preset color palettes (maps to comment spec)
- Returns QColor objects (easy conversion)
- Visual preview with color swatch
- Emits signals on change (fits ParameterVirtualWidget pattern)

The ParameterColor widget will be a **thin wrapper** around QwwColorComboBox, handling:

- SCAD color format ↔ QColor conversion
- Integration with ParameterVirtualWidget protocol
- Description widget layout
- Change signaling to parent

### Next Steps

**For Color Implementation:**

1. Locate parser code (`ParameterObjects::fromSourceFile()`)
2. Design color syntax (string hex vs vector, with/without alpha)
3. Create ColorParameter class or extend StringParameter
4. Implement ParameterColor widget with QwwColorComboBox
5. Extend parser to recognize `[color...]` syntax
6. Add factory case in createParameterWidget()
7. Test with sample SCAD scripts

**For General Object Extension:**

- Use color implementation as template
- Define object-specific comment syntax
- Create appropriate parameter/widget classes
- Follow established pattern for consistency

This customizer extension maintains the **declarative, comment-driven** paradigm while adding rich object support.
