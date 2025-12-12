# ParameterHexSpinBox - Hexadecimal Number Widget

## Overview
`ParameterHexSpinBox` is a customizer parameter widget that displays and edits numeric values in hexadecimal format. It follows the same pattern as other parameter widgets and integrates with the factory system.

## Files Created
- `src/gui/parameter/ParameterHexSpinBox.h` - Header with widget declarations
- `src/gui/parameter/ParameterHexSpinBox.cc` - Implementation
- `src/gui/parameter/ParameterHexSpinBox.ui` - Qt Designer UI definition
- `CMakeLists.txt` - Updated with new files

## Components

### HexDoubleSpinBox
Custom `QDoubleSpinBox` subclass that:
- Displays values with "0x" prefix in uppercase hex
- Accepts hex input (0-9, A-F, case-insensitive)
- Validates hex strings
- Stores values as doubles (SCAD-compatible 64-bit floats)
- Auto-calculates zero-padding width based on maximum value

**Key Methods:**
```cpp
QString textFromValue(double val) const override;
  // Converts double → "0xFF" format

double valueFromText(const QString& text) const override;
  // Parses "0xFF" → double

QValidator::State validate(QString& input, int& pos) const override;
  // Validates hex input (0-9, A-F only)

void setHexWidth(int width);
  // Set minimum digit count for zero-padding
```

### ParameterHexSpinBox
Main widget class extending `ParameterVirtualWidget`:
- Uses `HexDoubleSpinBox` for display/input
- Implements delayed change tracking (`lastSent`/`lastApplied`)
- Emits `changed(bool immediate)` signal on value changes
- Supports range constraints from `NumberParameter`

## Usage in SCAD Scripts

### Comment Syntax (Proposed)
```scad
// Hex value with range
myValue = 0xFF;  // [0x00:0xFF]

// Hex value with step
counter = 0x80;  // [0x00:0x10:0xFF]

// 32-bit value
address = 0x1000;  // [0x0000:0xFFFF]
```

## Integration Status

### ✅ Completed
- Widget implementation (HexDoubleSpinBox + ParameterHexSpinBox)
- UI definition (.ui file)
- CMakeLists.txt updated
- Change tracking (delayed/immediate) matching ParameterSpinBox
- Signal compatibility with existing widgets

### ⏳ Pending (Future Work)
1. **Parser Extension** - Recognize hex syntax in comments:
   - Detect `0x` prefix in constraint ranges
   - Flag NumberParameter as "displayAsHex"
   - Parse hex min/max/step values

2. **Factory Integration** - Add to `createParameterWidget()`:
   ```cpp
   if (parameter->displayAsHex) {
     return new ParameterHexSpinBox(parent, numberParam, descriptionStyle);
   }
   ```

3. **ParameterObject Extension** - Add hex display flag:
   ```cpp
   class NumberParameter : public ParameterObject {
     // ...existing fields...
     bool displayAsHex = false;  // NEW
   };
   ```

## Technical Details

### Value Storage
- Internally stores as `double` (64-bit float)
- Converts to/from `int64_t` for hex display
- Range: Safe for integers up to 2^53 (double precision limit)
- Default max: 0xFFFFFFFF (32-bit)

### Hex Width Calculation
Auto-calculates minimum digits based on maximum value:
```cpp
int hexWidth = ceil(log2(max + 1) / 4.0);  // bits → hex digits
hexWidth = std::max(2, hexWidth);           // Minimum 2 digits
```

Examples:
- max = 0xFF → width = 2 (displays "0x00" - "0xFF")
- max = 0xFFF → width = 3 (displays "0x000" - "0xFFF")
- max = 0xFFFFFFFF → width = 8 (displays "0x00000000" - "0xFFFFFFFF")

### Signal Behavior
Matches `ParameterSpinBox`:
- **`onChanged(double)`** → emits `changed(false)` (delayed, waits for timer)
- **`onEditingFinished()`** → emits `changed(true)` (immediate on focus loss/Enter)
- Prevents redundant signals via `lastSent`/`lastApplied` tracking

### Validation
Input validator accepts:
- Hex digits: 0-9, A-F, a-f
- Prefix: 0x or 0X (optional during typing)
- Returns `Intermediate` if empty or out of range
- Returns `Invalid` if non-hex characters present
- Returns `Acceptable` if valid hex within range

## Example Integration

### In ParameterWidget Factory
```cpp
ParameterVirtualWidget* ParameterWidget::createParameterWidget(
  ParameterObject *parameter, DescriptionStyle descriptionStyle) {

  if (auto numberParam = dynamic_cast<NumberParameter*>(parameter)) {
    // Check for hex display flag (future)
    if (numberParam->displayAsHex) {
      return new ParameterHexSpinBox(this, numberParam, descriptionStyle);
    }

    // Existing logic for slider/spinbox...
  }
  // ...rest of factory...
}
```

### In Parser (Conceptual)
```cpp
// When parsing: myValue = 0xFF; // [0x00:0xFF]
if (min.startsWith("0x") || max.startsWith("0x")) {
  numberParam->displayAsHex = true;
  numberParam->minimum = parseHex(min);
  numberParam->maximum = parseHex(max);
}
```

## Testing Notes
- Test with various ranges: 0x00-0xFF, 0x0000-0xFFFF, 0x00000000-0xFFFFFFFF
- Verify step increment/decrement works correctly
- Check keyboard input validation (reject non-hex chars)
- Verify mousewheel and arrow key behavior
- Test focus/blur triggers correct signals
- Ensure values persist across customizer preset changes

## Compatibility
- Uses existing `NumberParameter` (no new parameter type needed)
- Signals compatible with `ParameterWidget` preview system
- Follows established `ParameterVirtualWidget` contract
- No changes required to SCAD compiler/evaluator
