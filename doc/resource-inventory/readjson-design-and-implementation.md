# Issue #6224: JSON Parse Errors Not Reported to User

## Design Document and Implementation Summary

This document summarizes the reasoning, choices, and results for the fix to GitHub issue #6224.

---

## Problem Statement

**Issue**: [#6224 - JSON parse errors not reported to user](https://github.com/openscad/openscad/issues/6224)

When JSON files (customizer parameter sets, color schemes) contained syntax errors, OpenSCAD would silently fail to load them without informing the user what went wrong or where the error was located.

Users had no way to know:

- That a JSON parse error occurred
- Which file had the error
- What line/column the error was on
- What the nature of the error was

---

## Design Goals

1. **Provide actionable error messages** with file:line:column format (matching compiler error conventions)
2. **Remove boost::property_tree dependency** for JSON parsing (it lacks error location reporting)
3. **Use Qt JSON classes** which are already available and provide parse error details
4. **Maintain backward compatibility** with existing JSON file formats
5. **Add unit tests** to verify error reporting works correctly
6. **Minimize code changes** to reduce risk of regressions

---

## Technical Decisions

### Why Qt JSON instead of boost::property_tree?

| Feature | boost::property_tree | Qt JSON |
|---------|---------------------|---------|
| Parse error location | ❌ No | ✅ Yes (byte offset) |
| Already a dependency | ✅ Yes | ✅ Yes |
| Type-safe values | ❌ No (strings only) | ✅ Yes |
| Array support | ⚠️ Awkward | ✅ Native |
| Error messages | ❌ Generic | ✅ Descriptive |

Qt JSON's `QJsonParseError` provides:

- Byte offset of the error
- Descriptive error message (e.g., "unterminated object", "missing value separator")

### Why not nlohmann/json?

The `import_json.cc` file already uses nlohmann/json for the `import("file.json")` function. We evaluated whether to unify on this library, but found:

1. **nlohmann/json already has excellent error reporting** - it provides line:column information and descriptive messages
2. **Different use case** - `import_json.cc` converts JSON to OpenSCAD `Value` types; customizer/colorscheme code works with structured settings
3. **Qt JSON integrates better** with the rest of the GUI code which already uses Qt types

**Decision**: Leave `import_json.cc` as-is (it already reports errors well) and use Qt JSON for customizer/colorscheme code.

### Error Message Format

Chose the standard compiler error format for familiarity:

```text
filename:line:column: error message
```

Example:

```text
/path/to/params.json:15:8: unterminated object
```

---

## Implementation Overview

### New Files Created

| File | Purpose |
|------|---------|
| `src/io/JsonReader.h` | Unified JSON reading utility with error info |
| `src/io/JsonReader.cc` | Implementation of JsonReader |
| `src/gui/EditorColorScheme.h` | Typed class for editor color schemes |
| `src/gui/EditorColorScheme.cc` | Implementation with Qt JSON parsing |
| `src/io/json_reader_test.cc` | Catch2 unit tests for JSON reading |
| `tests/data/json/*.json` | Test data files (valid and error cases) |

### Files Modified

| File | Changes |
|------|---------|
| `src/core/customizer/ParameterSet.cc` | Converted from boost::property_tree to Qt JSON via JsonReader |
| `src/core/customizer/ParameterSet.h` | Added JsonErrorInfo parameter to readFile() |
| `src/core/customizer/ParameterObject.cc` | Changed from ptree to QJsonValue for import/export |
| `src/core/customizer/ParameterObject.h` | Updated signatures, replaced boost::optional with std::optional |
| `src/glview/ColorMap.cc` | Uses JsonReader for render color scheme loading |
| `src/glview/ColorMap.h` | Minor interface updates |
| `src/gui/ScintillaEditor.cc` | Uses new EditorColorScheme class |
| `src/gui/ScintillaEditor.h` | Updated to use EditorColorScheme |
| `src/gui/parameter/ParameterWidget.cc` | Updated error reporting to use formatError() |
| `CMakeLists.txt` | Added new source files to build |

---

## Key Components

### JsonErrorInfo Structure

```cpp
struct JsonErrorInfo {
  std::string message;    // Error description
  std::string filename;   // File that had the error
  int line = 0;           // 1-based line number
  int column = 0;         // 1-based column number
  int offset = -1;        // Byte offset in file

  bool hasError() const;
  std::string formatError() const;  // Returns "file:line:col: message"
};
```

### JsonReader Utility

```cpp
class JsonReader {
public:
  static bool readFile(const fs::path& path, QJsonDocument& doc, JsonErrorInfo& error);
  static bool readObject(const fs::path& path, QJsonObject& obj, JsonErrorInfo& error);
  static bool readArray(const fs::path& path, QJsonArray& arr, JsonErrorInfo& error);
  static void offsetToLineColumn(const QByteArray& data, int offset, int& line, int& column);
};
```

### Line/Column Calculation

Qt JSON provides only a byte offset. We convert this to line:column by scanning the file content:

```cpp
void JsonReader::offsetToLineColumn(const QByteArray& data, int offset, int& line, int& column) {
  line = 1;
  column = 1;
  for (int i = 0; i < offset && i < data.size(); ++i) {
    if (data[i] == '\n') {
      ++line;
      column = 1;
    } else {
      ++column;
    }
  }
}
```

---

## Changes to boost Dependencies

### Removed from Customizer Code

- `boost::property_tree::ptree` → `QJsonObject` / `QJsonValue`
- `boost::optional` → `std::optional`
- `boost::algorithm::string` functions → `QString` methods

### Still Used Elsewhere

boost is still used extensively in other parts of OpenSCAD. This PR only removes it from the customizer/JSON code path.

---

## Unit Tests

### Test Coverage

18 new tests covering:

- Valid JSON file reading (objects, arrays, parameter sets, color schemes)
- Parse error detection with line/column reporting
- Various error types: unterminated objects, missing commas, unterminated strings, illegal numbers, etc.
- Error message formatting
- Integration with existing `tests/bad-json/customizer-all-bad.json` test file

### Running Tests

```bash
# Build tests
ninja OpenSCADUnitTests

# Run all JSON tests
./OpenSCADUnitTests "[json]"

# Run issue-specific tests
./OpenSCADUnitTests "[issue6224]"
```

### Test Results

```text
All tests passed (88 assertions in 18 test cases)
```

---

## Behavioral Changes

### Before (Issue #6224)

User loads a parameter set file with a syntax error:

- **Result**: Silent failure, parameters not loaded
- **User sees**: Nothing (or confusing behavior)

### After (This PR)

User loads a parameter set file with a syntax error:

- **Result**: Error dialog displayed
- **User sees**: `params.json:15:8: unterminated object`

---

## Compatibility Notes

### JSON File Format

No changes to the JSON file format. Existing valid files continue to work.

### API Changes

- `ParameterSets::readFile()` now takes a `JsonErrorInfo&` parameter
- `ParameterObject::importValue()` and `exportValue()` now use `QJsonValue` instead of `ptree`

These are internal APIs not exposed to users or scripts.

---

## Risk Assessment

### Low Risk

- Uses well-tested Qt JSON classes
- Extensive unit test coverage
- No changes to file formats
- Incremental conversion (one subsystem at a time)

### Potential Issues

- Other platforms need testing (only Windows/MSYS2 tested so far)
- Error message wording may need adjustment based on user feedback

---

## Future Considerations

1. **Consistent error reporting**: Other JSON-using code (e.g., if more is added) should use JsonReader
2. **Localization**: Error messages are currently in English; could be localized via Qt's translation system
3. **Further boost removal**: This pattern could be applied to remove other boost dependencies if desired

---

## References

- [GitHub Issue #6224](https://github.com/openscad/openscad/issues/6224)
- [Qt JSON Documentation](https://doc.qt.io/qt-5/json.html)
- [Catch2 Testing Framework](https://github.com/catchorg/Catch2)
