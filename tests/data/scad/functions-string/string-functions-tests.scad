// Tests for string manipulation builtin functions
// Note: Run with --enable=all to test object type handling

// =============================================================================
// lower() tests - including non-string input handling
// =============================================================================

echo("=== lower() tests ===");

// Valid string inputs
echo("lower basic:", lower("HELLO") == "hello");
echo("lower mixed:", lower("HeLLo WoRLd") == "hello world");
echo("lower already:", lower("hello") == "hello");
echo("lower empty:", lower("") == "");

// UTF-8 characters
echo(lower("ΑΒΓ"));        // Greek uppercase
echo("lower utf8:", lower("ПРИВЕТ") == "привет");

// Empty and special characters
echo(lower(""));
echo(lower("HELLO123!@#"));
echo(lower("TEST_CASE-NAME"));

// Error cases - should return undef with warning
echo(lower());             // No arguments
echo(lower("a", "b"));     // Too many arguments

// Non-string inputs should return undef
echo("lower number:", is_undef(lower(42)));
echo("lower bool:", is_undef(lower(true)));
echo("lower vector:", is_undef(lower([1, 2, 3])));
echo("lower range:", is_undef(lower([0:10])));
echo("lower undef:", is_undef(lower(undef)));
test_obj = object(name="fred", age=12);
echo("lower object:", is_undef(lower(test_obj)));

// =============================================================================
// Test: upper() - Convert string to uppercase
// =============================================================================

echo("upper basic:", upper("hello") == "HELLO");
echo("upper mixed:", upper("HeLLo WoRLd") == "HELLO WORLD");
echo("upper utf8:", upper("привет") == "ПРИВЕТ");

// Empty and special characters
echo(upper(""));
echo(upper("hello123!@#"));
echo(upper("test_case-name"));

// UTF-8 characters
echo(upper("straße"));     // German: ß -> SS
echo(upper("αβγ"));        // Greek lowercase
echo(upper("привет"));     // Cyrillic lowercase

// Error cases - should return undef with warning
echo(upper());             // No arguments
echo(upper(123));          // Wrong type
echo(upper("a", "b"));     // Too many arguments

// =============================================================================
// trim() tests
// =============================================================================

echo("=== trim() tests ===");

echo("trim both:", trim("  hello  ") == "hello");
echo("trim left:", trim("  hello") == "hello");
echo("trim right:", trim("hello  ") == "hello");
echo("trim none:", trim("hello") == "hello");
echo("trim empty:", trim("") == "");
echo("escaped chars:", trim("\t\nhello\r\n"));
// Error cases
echo(trim());              // No arguments

// =============================================================================
// Test: ltrim() - Remove leading whitespace only
// =============================================================================

echo("=== ltrim() tests ===");

echo("ltrim:", ltrim("  hello  ") == "hello  ");
echo(ltrim("  hello"));
echo(ltrim("  hello  "));
echo(ltrim("hello  "));
echo(ltrim("   "));
echo(ltrim(""));

// Error cases
echo(ltrim());             // No arguments
echo(ltrim(123));          // Wrong type

// =============================================================================
// Test: rtrim() - Remove trailing whitespace only
// =============================================================================

echo("=== rtrim() tests ===");

echo("rtrim:", rtrim("  hello  ") == "  hello");
echo(rtrim("hello  "));
echo(rtrim("  hello  "));
echo(rtrim("  hello"));
echo(rtrim("   "));
echo(rtrim(""));

// Error cases
echo(rtrim());             // No arguments
echo(rtrim(123));          // Wrong type


// ===========================================================================
// Combination tests
// ===========================================================================

echo("=== combination tests ===");

// Chain operations
echo(upper(trim("  hello  ")));
echo(lower(trim("  HELLO  ")));
echo(trim(upper("  hello  ")));

// With variables
s = "  Mixed Case  ";
echo(upper(s));
echo(lower(s));
echo(trim(s));
echo(upper(trim(s)));
echo(lower(trim(s)));

// =============================================================================
// split() tests
// =============================================================================

echo("split comma:", split("a,b,c", ",") == ["a", "b", "c"]);
echo("split space:", split("hello world", " ") == ["hello", "world"]);
echo("split chars:", split("abc", "") == ["a", "b", "c"]);
echo("split utf8:", split("αβγ", "") == ["α", "β", "γ"]);

// =============================================================================
// join() tests
// =============================================================================

echo("join comma:", join(["a", "b", "c"], ",") == "a,b,c");
echo("join space:", join(["hello", "world"], " ") == "hello world");
echo("join space:", join(["hello", "world"]) == "hello world");
echo("join empty:", join(["a", "b", "c"], "") == "abc");

// =============================================================================
// replace() tests
// =============================================================================

echo("replace single:", replace("hello world", "world", "there") == "hello there");
echo("replace multi:", replace("hello hello", "hello", "hi") == "hi hi");
echo("replace none:", replace("hello", "x", "y") == "hello");
echo("replace delete:", replace("hello world", " world", "") == "hello");

// =============================================================================
// starts_with() / ends_with() / contains() tests
// =============================================================================

echo("starts_with true:", starts_with("hello world", "hello") == true);
echo("starts_with false:", starts_with("hello world", "world") == false);
echo("ends_with true:", ends_with("hello world", "world") == true);
echo("ends_with false:", ends_with("hello world", "hello") == false);
echo("contains true:", contains("hello world", "lo wo") == true);
echo("contains false:", contains("hello world", "xyz") == false);

// =============================================================================
// substr() tests
// =============================================================================

echo("substr basic:", substr("hello world", 0, 5) == "hello");
echo("substr end:", substr("hello world", 6) == "world");
echo("substr negative:", substr("hello world", -5) == "world");
echo("substr utf8:", substr("αβγδ", 1, 2) == "βγ");

// =============================================================================
// index_of() tests
// =============================================================================

echo("index_of found:", index_of("hello world", "world") == 6);
echo("index_of start:", index_of("hello world", "hello") == 0);
echo("index_of not found:", index_of("hello world", "xyz") == -1);
echo("index_of utf8:", index_of("αβγδ", "γ") == 2);

// =============================================================================
// pad_left() / pad_right() tests
// =============================================================================

echo("pad_left space:", pad_left("hello", 10) == "     hello");
echo("pad_left zero:", pad_left("42", 5, "0") == "00042");
echo("pad_left none:", pad_left("hello", 3) == "hello");
echo("pad_right space:", pad_right("hello", 10) == "hello     ");
echo("pad_right zero:", pad_right("42", 5, "0") == "42000");
echo("pad_right none:", pad_right("hello", 3) == "hello");

// Final marker
echo("string-functions-tests complete");
