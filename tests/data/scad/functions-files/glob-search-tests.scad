// OpenSCAD glob_search() Tests
// Tests for glob_search(vector, pattern) functionality

_debug_on = true;

echo("=== glob_search() Tests ===");

// Sample data for testing
test_files = ["readme.txt", "test.scad", "model.stl", "archive.tar.gz", "notes.md", "config.json"];
test_mixed = ["file1.txt", "file2.dat", "test", "data.csv", "image.png"];

if (_debug_on) {
    echo("--- Test data ---");
    echo("test_files:", test_files);
    echo("test_mixed:", test_mixed);
    
    echo("--- Basic pattern matching ---");
    echo("glob_search test_files, '*.txt':", glob_search(test_files, "*.txt"));
    echo("glob_search test_files, '*.scad':", glob_search(test_files, "*.scad"));
    echo("glob_search test_files, '*.stl':", glob_search(test_files, "*.stl"));
    
    echo("--- Wildcard * (match any characters) ---");
    echo("glob_search test_files, 'test*':", glob_search(test_files, "test*"));
    echo("glob_search test_files, '*tar*':", glob_search(test_files, "*tar*"));
    echo("glob_search test_files, 'model*':", glob_search(test_files, "model*"));
    
    echo("--- Wildcard ? (match single character) ---");
    echo("glob_search test_mixed, 'file?.txt':", glob_search(test_mixed, "file?.txt"));
    echo("glob_search test_mixed, 'file?.dat':", glob_search(test_mixed, "file?.dat"));
    
    echo("--- Complex patterns ---");
    echo("glob_search test_files, '*.tar.gz':", glob_search(test_files, "*.tar.gz"));
    echo("glob_search test_files, '*.*':", glob_search(test_files, "*.*"));
    echo("glob_search test_mixed, '*.???':", glob_search(test_mixed, "*.???"));
    
    echo("--- Match all ---");
    echo("glob_search test_files, '*':", glob_search(test_files, "*"));
    
    echo("--- No matches ---");
    echo("glob_search test_files, '*.xyz':", glob_search(test_files, "*.xyz"));
    echo("glob_search test_files, 'nomatch*':", glob_search(test_files, "nomatch*"));
}

// Assertions
assert(is_list(glob_search(test_files, "*.txt")), "glob_search should return a list");
assert(len(glob_search(test_files, "*.txt")) == 1, "Should find 1 .txt file");
assert(len(glob_search(test_files, "*.scad")) == 1, "Should find 1 .scad file");
assert(len(glob_search(test_files, "*.*")) == 6, "Should find all files with extensions");
assert(len(glob_search(test_files, "*.xyz")) == 0, "Should find no .xyz files");

// Test with directory listing
if (_debug_on) {
    echo("--- Real directory test ---");
    all_entries = read_dir(".");
    echo("All entries:", all_entries);
    
    echo("Filter .scad files:", glob_search(all_entries, "*.scad"));
    echo("Filter .txt files:", glob_search(all_entries, "*.txt"));
    echo("Filter 'test*' pattern:", glob_search(all_entries, "test*"));
}

// Combined usage: read_dir + glob_search
scad_files = glob_search(read_dir("."), "*.scad");
txt_files = glob_search(read_dir("."), "*.txt");

echo("--- Combined read_dir + glob_search ---");
echo(".scad files in current dir:", scad_files);
echo(".txt files in current dir:", txt_files);

assert(is_list(scad_files), "Combined operation should return a list");
assert(is_list(txt_files), "Combined operation should return a list");
