// Test file for parsing functions: parse_delimited, parse_pairs

echo("=== Testing parse_delimited() ===");

// Test with default delimiter {
text1 = "before {content1} middle {content2} after";
echo("parse_delimited(text1) first =", parse_delimited(text1));
echo("parse_delimited(text1, \"{\", 1) =", parse_delimited(text1, "{", 1));
echo("parse_delimited(text1, \"{\", 2) =", parse_delimited(text1, "{", 2));
echo("parse_delimited(text1, \"{\", 0) all =", parse_delimited(text1, "{", 0));

echo("");

// Test with brackets
text2 = "start [item1] and [item2] end";
echo("parse_delimited(text2, \"[\", 1) =", parse_delimited(text2, "[", 1));
echo("parse_delimited(text2, \"[\", 2) =", parse_delimited(text2, "[", 2));
echo("parse_delimited(text2, \"[\", 0) all =", parse_delimited(text2, "[", 0));

echo("");

// Test with parentheses
text3 = "func(arg1, arg2) and (another)";
echo("parse_delimited(text3, \"(\", 1) =", parse_delimited(text3, "(", 1));
echo("parse_delimited(text3, \"(\", 2) =", parse_delimited(text3, "(", 2));
echo("parse_delimited(text3, \"(\", 0) all =", parse_delimited(text3, "(", 0));

echo("");

// Test with nested delimiters
text4 = "outer {inner {nested} content} more";
echo("parse_delimited(text4, \"{\", 1) nested =", parse_delimited(text4, "{", 1));

echo("");

// Test with quotes
text5 = "say 'hello world' and 'goodbye'";
echo("parse_delimited(text5, \"'\", 1) =", parse_delimited(text5, "'", 1));
echo("parse_delimited(text5, \"'\", 0) all =", parse_delimited(text5, "'", 0));

echo("");

// Edge cases
text6 = "no delimiters here";
echo("parse_delimited(text6, \"{\", 1) no match =", parse_delimited(text6, "{", 1));

text7 = "unclosed {bracket";
echo("parse_delimited(text7, \"{\", 1) unclosed =", parse_delimited(text7, "{", 1));

echo("");
echo("=== Testing parse_pairs() ===");

// Basic key=value parsing
pairs1 = "width=100, height=200, depth=50";
echo("parse_pairs(pairs1) =", parse_pairs(pairs1));

echo("");

// Custom delimiters
pairs2 = "name:John;age:30;city:NYC";
echo("parse_pairs(pairs2, \":\", \";\") =", parse_pairs(pairs2, ":", ";"));

echo("");

// With spaces
pairs3 = "  key1 = value1 ,  key2 = value2  ";
echo("parse_pairs(pairs3) with spaces =", parse_pairs(pairs3));

echo("");

// Single pair
pairs4 = "color=red";
echo("parse_pairs(pairs4) single =", parse_pairs(pairs4));

echo("");

// Edge cases
pairs5 = "a=1,b=,c=3"; // Empty value
echo("parse_pairs(pairs5) empty value =", parse_pairs(pairs5));

pairs6 = ""; // Empty string
echo("parse_pairs(pairs6) empty =", parse_pairs(pairs6));

echo("");

// URL-style parameters
pairs7 = "x=10&y=20&z=30";
echo("parse_pairs(pairs7, \"=\", \"&\") URL-style =", parse_pairs(pairs7, "=", "&"));

echo("");
echo("=== Combined usage example ===");

// Extract and parse configuration
config = "settings {width=800, height=600, fullscreen=true}";
config_str = parse_delimited(config, "{", 1);
echo("Extracted config:", config_str);
config_pairs = parse_pairs(config_str);
echo("Parsed config pairs:", config_pairs);

echo("");
echo("=== All parsing tests complete ===");
