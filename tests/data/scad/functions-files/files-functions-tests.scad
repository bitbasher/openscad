// OpenSCAD File Functions Test Suite
// Location: tests/data/scad/functions-files/

// Set to true to echo results to console, false for assert-only mode
_debug_on = true;

echo("=== File Function Tests ===");

if (_debug_on) {
    echo("--- Path Resolution Tests ---");
    echo("absolute_path .:", absolute_path("."));
    echo("absolute_path ..:", absolute_path(".."));
    echo("canonical_path .:", canonical_path("."));
    
    echo("--- File Existence Tests ---");
    echo("file_exists this file:", file_exists("files-functions-tests.scad"));
    echo("file_exists nonexistent:", file_exists("nonexistent-file.scad"));
    echo("file_exists testdirectory:", file_exists("testdirectory"));
    echo("file_exists sub-folder:", file_exists("sub-folder"));
    
    echo("--- File Type Tests ---");
    echo("is_file this file:", is_file("files-functions-tests.scad"));
    echo("is_file testdirectory:", is_file("testdirectory"));
    echo("is_dir testdirectory:", is_dir("testdirectory"));
    echo("is_dir sub-folder:", is_dir("sub-folder"));
    echo("is_dir this file:", is_dir("files-functions-tests.scad"));
    
    echo("--- File Size Tests ---");
    echo("file_size readtesttext.txt:", file_size("readtesttext.txt"));
    echo("file_size readtestlines.txt:", file_size("readtestlines.txt"));
    
    echo("--- Path Manipulation Tests ---");
    echo("basename /path/to/file.scad:", basename("/path/to/file.scad"));
    echo("basename file.scad:", basename("file.scad"));
    echo("dirname /path/to/file.scad:", dirname("/path/to/file.scad"));
    echo("dirname file.scad:", dirname("file.scad"));
    echo("file_extension model.stl:", file_extension("model.stl"));
    echo("file_extension archive.tar.gz:", file_extension("archive.tar.gz"));
    echo("file_extension noextension:", file_extension("noextension"));
    
    echo("--- Directory Reading Tests ---");
    echo("read_dir .:", read_dir("."));
    echo("read_dir testdirectory:", read_dir("testdirectory"));
    echo("read_dir sub-folder:", read_dir("sub-folder"));
    
    echo("--- Relative Path Tests ---");
    echo("file_exists sub-folder/relative-path-test.txt:", file_exists("sub-folder/relative-path-test.txt"));
    echo("is_file sub-folder/relative-path-test.txt:", is_file("sub-folder/relative-path-test.txt"));
    echo("absolute_path sub-folder:", absolute_path("sub-folder"));
    
    echo("--- Read Text Tests ---");
    echo("read_text readtesttext.txt:", read_text("readtesttext.txt"));
    
    echo("--- Read Lines Tests ---");
    echo("read_lines readtestlines.txt:", read_lines("readtestlines.txt"));
    
    echo("--- Project Root Access Tests ---");
    // Navigate up to project root: functions-files -> scad -> data -> tests -> project root
    echo("file_exists ../../../../CMakeLists.txt:", file_exists("../../../../CMakeLists.txt"));
    echo("is_file ../../../../CMakeLists.txt:", is_file("../../../../CMakeLists.txt"));
    echo("is_dir ../../../../src:", is_dir("../../../../src"));
    echo("file_size ../../../../CMakeLists.txt:", file_size("../../../../CMakeLists.txt"));
    
} else {
    // Assert mode for automated testing
    
    echo("Running assertions...");
    
    // File existence
    assert(file_exists("files-functions-tests.scad"), "file_exists self");
    assert(!file_exists("nonexistent-file.scad"), "file_exists nonexistent");
    assert(file_exists("testdirectory"), "file_exists testdirectory");
    assert(file_exists("sub-folder"), "file_exists sub-folder");
    
    // File type checks
    assert(is_file("files-functions-tests.scad"), "is_file self");
    assert(!is_file("testdirectory"), "is_file on directory should be false");
    assert(is_dir("testdirectory"), "is_dir testdirectory");
    assert(is_dir("sub-folder"), "is_dir sub-folder");
    assert(!is_dir("files-functions-tests.scad"), "is_dir on file should be false");
    
    // File size
    assert(file_size("readtesttext.txt") > 0, "file_size readtesttext.txt");
    assert(file_size("readtestlines.txt") > 0, "file_size readtestlines.txt");
    
    // Path manipulation
    assert(basename("/path/to/file.scad") == "file.scad", "basename with path");
    assert(basename("file.scad") == "file.scad", "basename without path");
    assert(dirname("/path/to/file.scad") == "/path/to", "dirname with path");
    assert(file_extension("model.stl") == "stl", "file_extension stl");
    assert(file_extension("archive.tar.gz") == "gz", "file_extension tar.gz");
    assert(file_extension("noextension") == "", "file_extension none");
    
    // Directory reading
    assert(len(read_dir(".")) > 0, "read_dir current");
    assert(len(read_dir("testdirectory")) >= 0, "read_dir testdirectory");
    
    // Relative paths
    assert(file_exists("sub-folder"), "sub-folder exists");
    assert(is_dir("sub-folder"), "sub-folder is dir");
    assert(file_exists("sub-folder/relative-path-test.txt"), "sub-folder file exists");
    assert(is_file("sub-folder/relative-path-test.txt"), "sub-folder file is file");
    
    // Read functions
    assert(len(read_text("readtesttext.txt")) > 0, "read_text returns content");
    assert(len(read_lines("readtestlines.txt")) > 0, "read_lines returns lines");
    
    // Project root access
    assert(file_exists("../../../../CMakeLists.txt"), "project CMakeLists.txt exists");
    assert(is_file("../../../../CMakeLists.txt"), "project CMakeLists.txt is file");
    assert(is_dir("../../../../src"), "project src dir exists");
    
    echo("All assertions passed!");
}

echo("=== File Function Tests Complete ===");
/*
 Written 2025 by
  Jeff Hayes <vulcan_at_mac_com>

 To the extent possible under law, the author(s) have
 dedicated all copyright and related and neighboring
 rights to this software to the public domain worldwide.
 This software is distributed without any warranty.
 You should have received a copy of the
 CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
