// ============================================================================
// OpenSCAD File Functions Demo
// Demonstrates read_dir() with filters and glob_search() functionality
// ============================================================================

echo("╔════════════════════════════════════════════════════════════════════╗");
echo("║  OpenSCAD File Functions Demo - devSnap2025                       ║");
echo("╚════════════════════════════════════════════════════════════════════╝");

// ============================================================================
// PART 1: read_dir() - Enhanced Directory Reading with Filters
// ============================================================================

echo("\n--- Part 1: read_dir() with Filters ---\n");

// Basic usage - list everything in testdirectory
echo("1. List all files and directories in 'testdirectory':");
all_items = read_dir("testdirectory");
echo("   read_dir('testdirectory'):", all_items);

// Filter to show only files
echo("\n2. List only FILES in 'testdirectory':");
files_only = read_dir("testdirectory", "fileonly");
echo("   read_dir('testdirectory', 'fileonly'):", files_only);

// Filter to show only directories
echo("\n3. List only DIRECTORIES in 'testdirectory':");
dirs_only = read_dir("testdirectory", "dironly");
echo("   read_dir('testdirectory', 'dironly'):", dirs_only);

// Explicit request for both (same as default)
echo("\n4. List both files and directories explicitly:");
both = read_dir("testdirectory", "filesdirs");
echo("   read_dir('testdirectory', 'filesdirs'):", both);

// ============================================================================
// PART 2: glob_search() - Pattern Matching on Lists
// ============================================================================

echo("\n--- Part 2: glob_search() Pattern Matching ---\n");

// Get a list to work with
test_files = read_dir(".");

// Filter using wildcard patterns
echo("5. Find all .scad files in current directory:");
scad_files = glob_search(test_files, "*.scad");
echo("   glob_search(files, '*.scad'):", scad_files);

echo("\n6. Find all .txt files:");
txt_files = glob_search(test_files, "*.txt");
echo("   glob_search(files, '*.txt'):", txt_files);

echo("\n7. Find files starting with 'test':");
test_pattern = glob_search(test_files, "test*");
echo("   glob_search(files, 'test*'):", test_pattern);

echo("\n8. Find files containing 'functions' using wildcards:");
functions_pattern = glob_search(test_files, "*functions*");
echo("   glob_search(files, '*functions*'):", functions_pattern);

// Using ? for single character matching
echo("\n9. Pattern with ? (single character wildcard):");
echo("   Looking for 'demo-????-functions.scad' pattern:");
demo_pattern = glob_search(test_files, "demo-????-functions.scad");
echo("   glob_search(files, 'demo-????-functions.scad'):", demo_pattern);

// ============================================================================
// PART 3: Combining read_dir() and glob_search()
// ============================================================================

echo("\n--- Part 3: Combining Functions for Powerful Workflows ---\n");

// Example: Find all .scad test files
echo("10. Workflow: Find all .scad files that contain 'test' in name:");
all_files = read_dir(".", "fileonly");
test_scad = glob_search(all_files, "*test*.scad");
echo("    Step 1 - read_dir('.', 'fileonly'):", all_files);
echo("    Step 2 - glob_search(files, '*test*.scad'):", test_scad);

// Example: Count files vs directories
echo("\n11. Count files vs directories:");
total_items = len(read_dir("."));
file_count = len(read_dir(".", "fileonly"));
dir_count = len(read_dir(".", "dironly"));
echo("    Total items:", total_items);
echo("    Files:", file_count);
echo("    Directories:", dir_count);

// ============================================================================
// PART 4: Practical Use Cases
// ============================================================================

echo("\n--- Part 4: Practical Use Cases ---\n");

// Use case: Find all JSON configuration files
echo("12. Find configuration files (.json):");
json_files = glob_search(read_dir(".", "fileonly"), "*.json");
echo("    JSON files:", json_files);

// Use case: List subdirectories for processing
echo("\n13. Find subdirectories for batch processing:");
subdirs = read_dir(".", "dironly");
echo("    Subdirectories:", subdirs);

// Use case: Filter by multiple patterns (manual approach)
echo("\n14. Find files with specific extensions (.scad or .txt):");
all = read_dir(".", "fileonly");
scad = glob_search(all, "*.scad");
txt = glob_search(all, "*.txt");
combined = concat(scad, txt);
echo("    .scad files:", len(scad));
echo("    .txt files:", len(txt));
echo("    Combined:", len(combined));

// ============================================================================
// Summary
// ============================================================================

echo("\n╔════════════════════════════════════════════════════════════════════╗");
echo("║  Demo Complete!                                                    ║");
echo("║                                                                    ║");
echo("║  New File Functions:                                               ║");
echo("║  • read_dir(path, filter) - List directory with file/dir filters  ║");
echo("║  • glob_search(list, pattern) - Pattern matching with * and ?     ║");
echo("╚════════════════════════════════════════════════════════════════════╝");
