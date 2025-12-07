// Test file for new string functions: substr_count, substr_positions, replace_last, to_safe_filename

echo("=== Testing substr_count() ===");
echo("substr_count('hello', 'l') =", substr_count("hello", "l"));           // Expected: 2
echo("substr_count('hello world', 'o') =", substr_count("hello world", "o")); // Expected: 2
echo("substr_count('aaa', 'aa') =", substr_count("aaa", "aa"));             // Expected: 1 (non-overlapping)
echo("substr_count('test', 'x') =", substr_count("test", "x"));             // Expected: 0
echo("substr_count('', 'a') =", substr_count("", "a"));                     // Expected: 0
echo("substr_count('hello', '') =", substr_count("hello", ""));             // Expected: 0 (empty needle)

echo("");
echo("=== Testing substr_positions() - Single substring ===");
echo("substr_positions('hello', 'l') =", substr_positions("hello", "l"));   // Expected: [2, 3]
echo("substr_positions('hello world', 'o') =", substr_positions("hello world", "o")); // Expected: [4, 7]
echo("substr_positions('test', 'x') =", substr_positions("test", "x"));     // Expected: []
echo("substr_positions('abcabc', 'abc') =", substr_positions("abcabc", "abc")); // Expected: [0, 3]
echo("substr_positions('', 'a') =", substr_positions("", "a"));             // Expected: []

echo("");
echo("=== Testing substr_positions() - Multiple substrings ===");
result1 = substr_positions("a,b;c", [",", ";"]);
echo("substr_positions('a,b;c', [',', ';']) =", result1);
// Expected: [[",", 1], [";", 3]]

result2 = substr_positions("hello world", ["l", "o"]);
echo("substr_positions('hello world', ['l', 'o']) =", result2);
// Expected: [["l", 2, 3, 9], ["o", 4, 7]]

result3 = substr_positions("test", ["a", "b", "e"]);
echo("substr_positions('test', ['a', 'b', 'e']) =", result3);
// Expected: [["a"], ["b"], ["e", 1]]

echo("");
echo("=== Testing replace_last() ===");
echo("replace_last('hello hello', 'hello', 'hi') =", replace_last("hello hello", "hello", "hi"));
// Expected: "hello hi"

echo("replace_last('a,b,c', ',', ';') =", replace_last("a,b,c", ",", ";"));
// Expected: "a,b;c"

echo("replace_last('test', 'x', 'y') =", replace_last("test", "x", "y"));
// Expected: "test" (no match)

echo("replace_last('one', 'one', 'two') =", replace_last("one", "one", "two"));
// Expected: "two"

echo("replace_last('aaa', 'aa', 'bb') =", replace_last("aaa", "aa", "bb"));
// Expected: "abb" (replaces last occurrence)

echo("");
echo("=== Testing to_safe_filename() ===");
echo("to_safe_filename('normal_file.txt') =", to_safe_filename("normal_file.txt"));
// Expected: "normal_file.txt" (unchanged)

echo("to_safe_filename('file<name>.txt') =", to_safe_filename("file<name>.txt"));
// Expected: "file_name_.txt"

echo("to_safe_filename('my file: test') =", to_safe_filename("my file: test"));
// Expected: "my file_ test"

echo("to_safe_filename('file/path\\name') =", to_safe_filename("file/path\\name"));
// Expected: "file_path_name"

echo("to_safe_filename('file?.txt') =", to_safe_filename("file?.txt"));
// Expected: "file_.txt"

echo("to_safe_filename('file|name*test') =", to_safe_filename("file|name*test"));
// Expected: "file_name_test"

echo("to_safe_filename('  .file.  ') =", to_safe_filename("  .file.  "));
// Expected: "file" (leading/trailing spaces and dots trimmed)

echo("to_safe_filename('   ') =", to_safe_filename("   "));
// Expected: "file" (empty after sanitization)

echo("to_safe_filename('file\"name') =", to_safe_filename("file\"name"));
// Expected: "file_name"

echo("");
echo("=== Combined usage examples ===");

// Count and locate
text = "the quick brown fox jumps over the lazy dog";
count_the = substr_count(text, "the");
positions_the = substr_positions(text, "the");
echo("Text has", count_the, "occurrences of 'the' at positions:", positions_the);

// Replace and sanitize for filename
original = "Report: Q4-2024 <FINAL>";
modified = replace_last(original, "<FINAL>", "[v2]");
safe = to_safe_filename(modified);
echo("Original:", original);
echo("Modified:", modified);
echo("Safe filename:", safe);

// Multi-delimiter search
csv_data = "name,age;city,country";
delimiters = substr_positions(csv_data, [",", ";"]);
echo("CSV delimiters found:", delimiters);

echo("");
echo("=== All string function tests complete ===");
