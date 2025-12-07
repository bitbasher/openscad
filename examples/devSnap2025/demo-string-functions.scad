// ============================================================================
// OpenSCAD String Functions Demo
// Demonstrates substr_count(), substr_positions(), replace_last(), 
// and to_safe_filename()
// ============================================================================

echo("╔════════════════════════════════════════════════════════════════════╗");
echo("║  OpenSCAD String Functions Demo - devSnap2025                     ║");
echo("╚════════════════════════════════════════════════════════════════════╝");

// ============================================================================
// PART 1: substr_count() - Count Substring Occurrences
// ============================================================================

echo("\n--- Part 1: substr_count() - Count Occurrences ---\n");

text1 = "The quick brown fox jumps over the lazy dog";
echo("1. Count 'the' in:", text1);
count1 = substr_count(text1, "the");
echo("   substr_count(text, 'the'):", count1, "(case-sensitive)");

text2 = "banana";
echo("\n2. Count 'na' in:", text2);
count2 = substr_count(text2, "na");
echo("   substr_count('banana', 'na'):", count2, "(non-overlapping)");

text3 = "aaa";
echo("\n3. Count 'aa' in:", text3);
count3 = substr_count(text3, "aa");
echo("   substr_count('aaa', 'aa'):", count3, "(non-overlapping: 1, not 2)");

path = "/usr/local/bin/openscad";
echo("\n4. Count '/' separators in path:", path);
count4 = substr_count(path, "/");
echo("   substr_count(path, '/'):", count4);

// ============================================================================
// PART 2: substr_positions() - Find All Positions
// ============================================================================

echo("\n--- Part 2: substr_positions() - Find Positions ---\n");

// Single search string
text5 = "OpenSCAD is awesome, OpenSCAD is powerful";
echo("5. Find all positions of 'OpenSCAD':");
echo("   Text:", text5);
positions1 = substr_positions(text5, "OpenSCAD");
echo("   substr_positions(text, 'OpenSCAD'):", positions1);

// Multiple search strings (returns nested format)
text6 = "x=10, y=20, z=30";
echo("\n6. Find positions of multiple delimiters:");
echo("   Text:", text6);
delimiters = ["=", ","];
positions2 = substr_positions(text6, delimiters);
echo("   substr_positions(text, ['=', ',']):", positions2);
echo("   Format: [[delimiter, pos1, pos2, ...], ...]");

// Practical example: Find all spaces
text7 = "Hello World from OpenSCAD";
echo("\n7. Find all space positions:");
echo("   Text:", text7);
spaces = substr_positions(text7, " ");
echo("   substr_positions(text, ' '):", spaces);

// No matches found
text8 = "hello";
echo("\n8. Search with no matches:");
echo("   Text:", text8);
no_match = substr_positions(text8, "xyz");
echo("   substr_positions('hello', 'xyz'):", no_match, "(empty list)");

// ============================================================================
// PART 3: replace_last() - Replace Last Occurrence
// ============================================================================

echo("\n--- Part 3: replace_last() - Replace Last Occurrence ---\n");

text9 = "file.backup.backup";
echo("9. Replace last '.backup' extension:");
echo("   Original:", text9);
replaced1 = replace_last(text9, ".backup", "");
echo("   replace_last(text, '.backup', ''):", replaced1);

text10 = "one, two, three, four";
echo("\n10. Replace last comma with 'and':");
echo("    Original:", text10);
replaced2 = replace_last(text10, ", ", " and ");
echo("    replace_last(text, ', ', ' and '):", replaced2);

text11 = "C:\\temp\\file.txt";
echo("\n11. Replace last backslash with forward slash:");
echo("    Original:", text11);
replaced3 = replace_last(text11, "\\", "/");
echo("    replace_last(text, '\\\\', '/'):", replaced3);

// No match - returns original
text12 = "hello world";
echo("\n12. Replace when search string not found:");
echo("    Original:", text12);
replaced4 = replace_last(text12, "xyz", "abc");
echo("    replace_last(text, 'xyz', 'abc'):", replaced4, "(unchanged)");

// ============================================================================
// PART 4: to_safe_filename() - Sanitize Filenames
// ============================================================================

echo("\n--- Part 4: to_safe_filename() - Sanitize Filenames ---\n");

// Remove illegal characters
filename1 = "my<cool>file:name.txt";
echo("13. Sanitize filename with illegal characters:");
echo("    Original:", filename1);
safe1 = to_safe_filename(filename1);
echo("    to_safe_filename():", safe1);
echo("    (removes < > : from filename)");

// Handle multiple issues at once
filename2 = "  report (2024/12/07).pdf  ";
echo("\n14. Clean up messy filename:");
echo("    Original: '", filename2, "'");
safe2 = to_safe_filename(filename2);
echo("    to_safe_filename():", safe2);
echo("    (removes /, trims spaces)");

// Windows reserved characters
filename3 = "data|pipe*wild?chars\\.csv";
echo("\n15. Remove Windows reserved characters:");
echo("    Original:", filename3);
safe3 = to_safe_filename(filename3);
echo("    to_safe_filename():", safe3);
echo("    (removes | * ? \\)");

// Extreme case - all illegal
filename4 = "<>:\"/\\|?*";
echo("\n16. All illegal characters:");
echo("    Original: '<>:\"/\\\\|?*'");
safe4 = to_safe_filename(filename4);
echo("    to_safe_filename():", safe4, "(returns 'file' as fallback)");

// Already safe filename
filename5 = "valid_filename-v2.scad";
echo("\n17. Already safe filename:");
echo("    Original:", filename5);
safe5 = to_safe_filename(filename5);
echo("    to_safe_filename():", safe5, "(unchanged)");

// ============================================================================
// PART 5: Practical Workflows
// ============================================================================

echo("\n--- Part 5: Practical Use Cases ---\n");

// Use case: Parse CSV-like data
csv_line = "name,age,city,country";
echo("18. Count fields in CSV:");
echo("    Data:", csv_line);
field_count = substr_count(csv_line, ",") + 1;
echo("    Number of fields:", field_count);

// Use case: Build safe export filename
user_input = "My Model (v2.0) <final>.stl";
echo("\n19. Generate safe export filename:");
echo("    User input:", user_input);
export_name = to_safe_filename(user_input);
echo("    Safe filename:", export_name);

// Use case: Find and replace last path component
filepath = "/home/user/projects/old_name/file.scad";
echo("\n20. Replace last path component:");
echo("    Original:", filepath);
new_path = replace_last(filepath, "/old_name/", "/new_name/");
echo("    Updated:", new_path);

// Use case: Count word occurrences
sentence = "Buffalo buffalo Buffalo buffalo buffalo buffalo Buffalo buffalo";
echo("\n21. Count specific word (case-sensitive):");
echo("    Sentence:", sentence);
buffalo_count = substr_count(sentence, "Buffalo");
echo("    Occurrences of 'Buffalo':", buffalo_count);

// Use case: Find delimiter positions for manual parsing
data = "key1=value1; key2=value2; key3=value3";
echo("\n22. Locate delimiters for parsing:");
echo("    Data:", data);
equals_pos = substr_positions(data, "=");
semicolon_pos = substr_positions(data, ";");
echo("    '=' positions:", equals_pos);
echo("    ';' positions:", semicolon_pos);

// ============================================================================
// Summary
// ============================================================================

echo("\n╔════════════════════════════════════════════════════════════════════╗");
echo("║  Demo Complete!                                                    ║");
echo("║                                                                    ║");
echo("║  New String Functions:                                             ║");
echo("║  • substr_count(haystack, needle) - Count occurrences             ║");
echo("║  • substr_positions(haystack, needle(s)) - Find positions         ║");
echo("║  • replace_last(haystack, search, replace) - Replace last match   ║");
echo("║  • to_safe_filename(input) - Sanitize filenames                   ║");
echo("╚════════════════════════════════════════════════════════════════════╝");
