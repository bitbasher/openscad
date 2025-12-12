# OpenSCAD Customizer Comment Parser - Overview

## Entry Point
**File:** `src/core/customizer/ParameterObject.cc`

### Main Function
```cpp
ParameterObjects ParameterObjects::fromSourceFile(const SourceFile *sourceFile)
```
- Iterates through all assignments in the source file's scope
- Calls `ParameterObject::fromAssignment()` for each assignment
- Returns collection of parsed parameters

---

## Core Parser: `fromAssignment()`

**Function:** `ParameterObject::fromAssignment(const Assignment *assignment)`

### Step 1: Check for Parameter Annotation
```cpp
const Annotation *parameterAnnotation = assignment->annotation("Parameter");
if (!parameterAnnotation) {
  return nullptr;  // Not a customizer parameter
}
parameter = parameterAnnotation->getExpr().get();
```

The **"Parameter" annotation** comes from the **comment after the assignment**:
```scad
myValue = 10;  // [0:100]
//             ^^^^^^^^^^^ This becomes "Parameter" annotation
```

### Step 2: Extract Optional Metadata

**Description Annotation:**
```cpp
const Annotation *descriptionAnnotation = assignment->annotation("Description");
```
Comes from **comment before the assignment**:
```scad
// This is the description
myValue = 10;  // [0:100]
```

**Group Annotation:**
```cpp
const Annotation *groupAnnotation = assignment->annotation("Group");
```
Comes from **group header comment**:
```scad
/*[ Group Name ]*/
myValue = 10;  // [0:100]
```

### Step 3: Type Detection by Value

Parser examines the **assigned value** to determine parameter type:

#### Boolean Parameter
```cpp
if (expression->isBool()) {
  return std::make_unique<BoolParameter>(name, description, group, expression->toBool());
}
```
**Example:** `flag = true;  // Any comment`

#### Enum Parameter (Combo Box)
```cpp
if (expression->isDouble() || expression->isString()) {
  EnumValues values = parseEnumItems(parameter, key, value);
  if (!values.items.empty()) {
    return std::make_unique<EnumParameter>(...);
  }
}
```
**Examples:**
```scad
choice = 2;  // [0, 1, 2, 3, 4]
choice = "foo";  // [foo, bar, baz]
labeled = 10;  // [5:S, 10:L, 20:M]
```

#### String Parameter
```cpp
if (expression->isString()) {
  boost::optional<size_t> maximumSize = boost::none;
  const auto *maximumSizeExpression = dynamic_cast<const Literal *>(parameter);
  if (maximumSizeExpression && maximumSizeExpression->isDouble()) {
    maximumSize = (size_t)(maximumSizeExpression->toDouble());
  }
  return std::make_unique<StringParameter>(...);
}
```
**Examples:**
```scad
text = "hello";  // No constraint
text = "hello";  // [50]  // Max 50 chars
```

#### Number Parameter
```cpp
if (expression->isDouble()) {
  double value = expression->toDouble();
  NumericLimits limits = parseNumericLimits(parameter, {value});
  return std::make_unique<NumberParameter>(...);
}
```
**Examples:**
```scad
num = 10;  // No constraint
num = 10;  // [100]  // Max only [0:100]
num = 10;  // [5:50]  // Range [5:50]
num = 10;  // [0:5:100]  // Range with step [0:5:100]
```

#### Vector Parameter
```cpp
else if (const auto *expression = dynamic_cast<const Vector *>(valueExpression)) {
  if (expression->getChildren().size() < 1 || expression->getChildren().size() > 4) {
    return nullptr;  // Only 1-4 elements
  }

  std::vector<double> value;
  for (const auto& element : expression->getChildren()) {
    // All elements must be numeric
    value.push_back(item->toDouble());
  }

  NumericLimits limits = parseNumericLimits(parameter, value);
  return std::make_unique<VectorParameter>(...);
}
```
**Examples:**
```scad
pos = [0, 0];  // 2D vector
pos = [0, 0, 0];  // 3D vector
pos = [1, 2, 3, 4];  // 4D vector
pos = [0, 0, 0];  // [0:10]  // Each element limited [0:10]
```

---

## Helper Functions

### `parseEnumItems()`
**Purpose:** Parse list of allowed values for combo boxes

**Input Expression Types:**
1. **Simple list:** `[val1, val2, val3]`
2. **Labeled list:** `[[val1, "Label1"], [val2, "Label2"]]`

**Logic:**
```cpp
for each element in vector:
  if (element is Literal):
    if (isDouble): item = { value: number, key: stringified_number }
    if (isString): item = { value: string, key: string }
  else if (element is Vector with 2 children):
    item = { value: element[0], key: element[1] }
```

**Special Cases:**
- Single numeric element `[100]` → NOT an enum, treated as range maximum
- Mixed types allowed (strings and numbers in same list)

**Example Parsing:**
```scad
choice = 2;  // [0, 1, 2, 3]
// Becomes: items = [
//   {value: 0, key: "0"},
//   {value: 1, key: "1"},
//   {value: 2, key: "2"},
//   {value: 3, key: "3"}
// ]

choice = "foo";  // [foo, bar, baz]
// Becomes: items = [
//   {value: "foo", key: "foo"},
//   {value: "bar", key: "bar"},
//   {value: "baz", key: "baz"}
// ]

choice = 10;  // [5:Small, 10:Large, 20:XL]
// Becomes: items = [
//   {value: 5, key: "Small"},
//   {value: 10, key: "Large"},
//   {value: 20, key: "XL"}
// ]
```

### `parseNumericLimits()`
**Purpose:** Parse range constraints for numeric values

**Input Expression Types:**

1. **Single Literal (step only):**
```cpp
if (parameter is Literal && isDouble):
  output.step = value
```
**Example:** `num = 10;  // 5` → step = 5

2. **Vector with 1 element (maximum only):**
```cpp
if (parameter is Vector with 1 child):
  output.maximum = child
```
**Example:** `num = 10;  // [100]` → max = 100

3. **Range expression (min:max or min:step:max):**
```cpp
if (parameter is Range):
  output.minimum = range.begin
  output.maximum = range.end
  if (range.step):
    output.step = range.step
```
**Examples:**
```scad
num = 10;  // [5:50]        → min=5, max=50
num = 10;  // [0:5:100]     → min=0, step=5, max=100
```

**Value Expansion:**
```cpp
for each value in values:
  if (value < minimum): minimum = value
  if (value > maximum): maximum = value
```
Ensures the default value is always within range.

---

## Data Structures

### Expression Types (AST)
- **Literal:** Atomic values (bool, double, string)
- **Vector:** Array of expressions `[...]`
- **Range:** Range expression `[begin:step:end]`
- **Assignment:** Variable assignment statement

### Annotation
Represents comments parsed into structured data:
- **"Parameter"**: Comment after assignment (`// [...]`)
- **"Description"**: Comment before assignment
- **"Group"**: Group header comment (`/*[ ... ]*/`)

### Parameter Types Created
```cpp
BoolParameter(name, description, group, value)
EnumParameter(name, description, group, defaultIndex, items[])
StringParameter(name, description, group, value, maxSize)
NumberParameter(name, description, group, value, min, max, step)
VectorParameter(name, description, group, value[], min, max, step)
```

---

## Comment Syntax Summary

### Basic Structure
```scad
// Description comment (optional)
variableName = defaultValue;  // [constraint specification]
```

### Group Headers
```scad
/*[ Group Name ]*/
// All parameters below this belong to "Group Name"

/*[ Hidden ]*/
// Parameters below this are hidden from customizer
```

### Constraint Specifications

| Value Type | Constraint Format | Example | Result |
|------------|------------------|---------|--------|
| Boolean | (none) | `flag = true;` | BoolParameter (checkbox) |
| Number | No constraint | `x = 10;` | NumberParameter (spinbox) |
| Number | `[max]` | `x = 10; // [100]` | NumberParameter (0 to 100) |
| Number | `[min:max]` | `x = 10; // [5:50]` | NumberParameter (5 to 50, slider) |
| Number | `[min:step:max]` | `x = 10; // [0:5:100]` | NumberParameter (0-100, step 5, slider) |
| String | No constraint | `s = "hi";` | StringParameter (text field) |
| String | `[maxLen]` | `s = "hi"; // [50]` | StringParameter (max 50 chars) |
| Number/String | `[val1, val2, ...]` | `x = 2; // [0,1,2,3]` | EnumParameter (combo box) |
| Number/String | `[val1:label1, ...]` | `x = 10; // [5:S, 10:L]` | EnumParameter (labeled combo) |
| Vector | `[min:max]` | `v = [0,0,0]; // [0:10]` | VectorParameter (3 spinboxes, 0-10 each) |
| Vector | `[min:step:max]` | `v = [1,2]; // [0:0.5:10]` | VectorParameter (2 spinboxes, 0-10, step 0.5) |

---

## Parser Flow Diagram

```
Assignment Statement in SCAD
  ↓
Has "Parameter" Annotation? (comment after =)
  ↓ No → Skip (not a customizer parameter)
  ↓ Yes
  ↓
Extract metadata:
  - Description (comment before =)
  - Group (group header above)
  - Name (variable name)
  ↓
Check value type:
  ├─ isBool() → BoolParameter
  ├─ isDouble() AND has enum list → EnumParameter
  ├─ isDouble() → parseNumericLimits() → NumberParameter
  ├─ isString() AND has enum list → EnumParameter
  ├─ isString() → StringParameter
  └─ isVector() → parseNumericLimits() → VectorParameter
```

---

## Hexadecimal Number Support

### Implementation
Hex numbers (`0xFF` syntax) are now fully supported in customizer comments.

### Changes Made

**1. Comment Lexer (`src/core/customizer/comment_lexer.l`)**
Added hex number recognition:
```c
0[xX]{H}+ {
    // Parse hexadecimal number (skip "0x" prefix)
    unsigned long long ull = strtoull(yytext + 2, NULL, 16);
    comment_parserlval.num = (double)ull;
    return NUM;
}
```

**2. Annotation Class (`src/core/customizer/Annotation.h`)**
Added raw text storage to preserve original comment:
```cpp
class Annotation {
  std::string raw_text;  // New field
public:
  Annotation(std::string name, std::shared_ptr<Expression> expr, std::string raw_text = "");
  const std::string& getRawText() const { return raw_text; }
};
```

**3. Numeric Limits (`src/core/customizer/ParameterObject.cc`)**
Added hex detection:
```cpp
struct NumericLimits {
  bool displayAsHex = false;  // New field
};

static NumericLimits parseNumericLimits(..., const std::string& raw_text = "") {
  // ... existing parsing ...

  // Check if raw comment contains hex notation
  if (!raw_text.empty()) {
    if (raw_text.find("0x") != std::string::npos ||
        raw_text.find("0X") != std::string::npos) {
      output.displayAsHex = true;
    }
  }
  return output;
}
```

**4. NumberParameter (`src/core/customizer/ParameterObject.h`)**
Added displayAsHex field:
```cpp
class NumberParameter {
  bool displayAsHex;
public:
  NumberParameter(..., bool displayAsHex = false);
};
```

### Usage Examples
```scad
// Hex range
hexValue = 0xFF;  // [0x00:0xFF]

// Hex with step
address = 0x1000;  // [0x0000:0x0100:0xFFFF]

// Color codes (8-digit hex for RGBA)
color = 0xRRGGBBAA;  // [0x00000000:0xFFFFFFFF]
```

When the comment contains `0x` or `0X`, the parameter's `displayAsHex` flag is set to `true`, allowing the UI factory to create a `ParameterHexSpinBox` widget instead of the standard `ParameterSpinBox`.
