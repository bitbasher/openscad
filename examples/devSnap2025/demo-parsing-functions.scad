// ============================================================================
// OpenSCAD Parsing Functions Demo
// Demonstrates parse_delimited() and parse_pairs()
// ============================================================================

echo("╔════════════════════════════════════════════════════════════════════╗");
echo("║  OpenSCAD Parsing Functions Demo - devSnap2025                    ║");
echo("╚════════════════════════════════════════════════════════════════════╝");

// ============================================================================
// PART 1: parse_delimited() - Extract Content Between Delimiters
// ============================================================================

echo("\n--- Part 1: parse_delimited() - Extract Delimited Content ---\n");

// Basic brace extraction
text1 = "transform{scale: 2, rotate: 45}";
echo("1. Extract content within curly braces:");
echo("   Text:", text1);
content1 = parse_delimited(text1, "{");
echo("   parse_delimited(text, '{'):", content1);

// Multiple brace pairs - extract first by default
text2 = "module box{size=10} module sphere{r=5}";
echo("\n2. Multiple brace pairs (extracts first by default):");
echo("   Text:", text2);
first = parse_delimited(text2, "{");
echo("   parse_delimited(text, '{'):", first);

// Extract from specific position
text3 = "first{data1} second{data2} third{data3}";
echo("\n3. Extract from specific position:");
echo("   Text:", text3);
pos1 = parse_delimited(text3, "{", 1);
pos2 = parse_delimited(text3, "{", 2);
pos3 = parse_delimited(text3, "{", 3);
echo("   Position 1:", pos1);
echo("   Position 2:", pos2);
echo("   Position 3:", pos3);

// Bracket delimiters
text4 = "array[0, 1, 2, 3]";
echo("\n4. Extract content within square brackets:");
echo("   Text:", text4);
content4 = parse_delimited(text4, "[");
echo("   parse_delimited(text, '['):", content4);

// Parentheses
text5 = "function(arg1, arg2, arg3)";
echo("\n5. Extract function arguments:");
echo("   Text:", text5);
content5 = parse_delimited(text5, "(");
echo("   parse_delimited(text, '('):", content5);

// Double quotes
text6 = "message=\"Hello, World!\"";
echo("\n6. Extract quoted string:");
echo("   Text:", text6);
content6 = parse_delimited(text6, "\"");
echo("   parse_delimited(text, '\"'):", content6);

// Single quotes
text7 = "value='sample text'";
echo("\n7. Extract single-quoted string:");
echo("   Text:", text7);
content7 = parse_delimited(text7, "'");
echo("   parse_delimited(text, \"'\"):", content7);

// Nested delimiters (extracts to first closing)
text8 = "outer{inner{nested}}";
echo("\n8. Nested delimiters (extracts outermost):");
echo("   Text:", text8);
content8 = parse_delimited(text8, "{");
echo("   parse_delimited(text, '{'):", content8);

// No match found
text9 = "no delimiters here";
echo("\n9. No matching delimiter:");
echo("   Text:", text9);
content9 = parse_delimited(text9, "{");
echo("   parse_delimited(text, '{'):", content9, "(returns undef)");

// ============================================================================
// PART 2: parse_pairs() - Parse Key-Value Pairs
// ============================================================================

echo("\n--- Part 2: parse_pairs() - Parse Key-Value Pairs ---\n");

// Basic key=value parsing
config1 = "width=100, height=50, depth=25";
echo("10. Parse comma-separated key=value pairs:");
echo("    Text:", config1);
parsed1 = parse_pairs(config1);
echo("    parse_pairs(text):", parsed1);
echo("    Format: [[key, value], [key, value], ...]");

// Custom pair delimiter (colon instead of equals)
config2 = "name:OpenSCAD, version:2024, type:3D";
echo("\n11. Parse with custom pair delimiter (:):");
echo("    Text:", config2);
parsed2 = parse_pairs(config2, ":");
echo("    parse_pairs(text, ':'):", parsed2);

// Custom item delimiter (semicolon instead of comma)
config3 = "x=10; y=20; z=30";
echo("\n12. Parse with custom item delimiter (;):");
echo("    Text:", config3);
parsed3 = parse_pairs(config3, "=", ";");
echo("    parse_pairs(text, '=', ';'):", parsed3);

// Both custom delimiters
config4 = "color:red; size:large; active:true";
echo("\n13. Both delimiters customized (: and ;):");
echo("    Text:", config4);
parsed4 = parse_pairs(config4, ":", ";");
echo("    parse_pairs(text, ':', ';'):", parsed4);

// Whitespace handling
config5 = "  key1 = value1  ,  key2 = value2  ";
echo("\n14. Parse with extra whitespace:");
echo("    Text:", config5);
parsed5 = parse_pairs(config5);
echo("    parse_pairs(text):", parsed5);
echo("    (whitespace automatically trimmed)");

// Empty values
config6 = "name=OpenSCAD, version=, status=active";
echo("\n15. Handle empty values:");
echo("    Text:", config6);
parsed6 = parse_pairs(config6);
echo("    parse_pairs(text):", parsed6);

// Single pair
config7 = "answer=42";
echo("\n16. Single key-value pair:");
echo("    Text:", config7);
parsed7 = parse_pairs(config7);
echo("    parse_pairs(text):", parsed7);

// No pairs (malformed)
config8 = "just some text without pairs";
echo("\n17. Malformed input (no delimiters):");
echo("    Text:", config8);
parsed8 = parse_pairs(config8);
echo("    parse_pairs(text):", parsed8);

// ============================================================================
// PART 3: Practical Use Cases
// ============================================================================

echo("\n--- Part 3: Practical Use Cases ---\n");

// Use case: Parse CSS-like properties
css = "color{background: #ffffff; padding: 10px; margin: 5px}";
echo("18. Extract and parse CSS-like properties:");
echo("    Input:", css);
properties = parse_delimited(css, "{");
echo("    Step 1 - extract:", properties);
parsed_props = parse_pairs(properties, ":", ";");
echo("    Step 2 - parse:", parsed_props);

// Use case: Parse function call
func_call = "drawShape(type=circle, radius=15, color=blue)";
echo("\n19. Parse function arguments:");
echo("    Function call:", func_call);
args = parse_delimited(func_call, "(");
echo("    Step 1 - extract args:", args);
parsed_args = parse_pairs(args);
echo("    Step 2 - parse pairs:", parsed_args);

// Use case: Configuration file format
config_line = "server:localhost; port:8080; protocol:http; timeout:30";
echo("\n20. Parse configuration string:");
echo("    Config:", config_line);
settings = parse_pairs(config_line, ":", ";");
echo("    Parsed settings:", settings);
echo("    Access example - server:", settings[0][1]);
echo("                     port:", settings[1][1]);

// Use case: Extract JSON-like data (simplified)
json_like = "{\"name\": \"model\", \"version\": \"1.0\"}";
echo("\n21. Extract JSON-like content:");
echo("    JSON-like:", json_like);
json_content = parse_delimited(json_like, "{");
echo("    Extracted:", json_content);

// Use case: Parse URL query string
query = "search=openscad&category=3d&sort=recent";
echo("\n22. Parse URL query parameters:");
echo("    Query string:", query);
params = parse_pairs(query, "=", "&");
echo("    Parsed params:", params);

// Use case: Extract array elements
array_text = "values[10, 20, 30, 40, 50]";
echo("\n23. Extract array elements:");
echo("    Array:", array_text);
elements = parse_delimited(array_text, "[");
echo("    Elements string:", elements);
echo("    (Could further split by comma if needed)");

// Use case: Multi-level parsing
nested = "config{display{width=1920, height=1080}, sound{volume=75}}";
echo("\n24. Multi-level nested parsing:");
echo("    Input:", nested);
outer = parse_delimited(nested, "{");
echo("    Level 1 (outer):", outer);
display = parse_delimited(outer, "{", 1);
sound = parse_delimited(outer, "{", 2);
echo("    Level 2 (display):", display);
echo("    Level 2 (sound):", sound);
display_settings = parse_pairs(display);
sound_settings = parse_pairs(sound);
echo("    Display parsed:", display_settings);
echo("    Sound parsed:", sound_settings);

// Use case: Extract and validate
validation_text = "bounds{min=0, max=100}";
echo("\n25. Extract and access specific values:");
echo("    Input:", validation_text);
bounds_str = parse_delimited(validation_text, "{");
bounds = parse_pairs(bounds_str);
echo("    Parsed:", bounds);
min_val = bounds[0][1];
max_val = bounds[1][1];
echo("    Min value:", min_val);
echo("    Max value:", max_val);
echo("    (Could convert to numbers for validation)");

// ============================================================================
// Summary
// ============================================================================

echo("\n╔════════════════════════════════════════════════════════════════════╗");
echo("║  Demo Complete!                                                    ║");
echo("║                                                                    ║");
echo("║  New Parsing Functions:                                            ║");
echo("║  • parse_delimited(str, delims, pos) - Extract delimited content  ║");
echo("║    Supports: { }, [ ], ( ), \" \", ' '                            ║");
echo("║  • parse_pairs(str, pair_delim, item_delim) - Parse key=value     ║");
echo("║    Returns: [[key1, val1], [key2, val2], ...]                     ║");
echo("╚════════════════════════════════════════════════════════════════════╝");
