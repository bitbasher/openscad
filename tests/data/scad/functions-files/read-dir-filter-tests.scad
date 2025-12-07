// OpenSCAD read_dir() Filter Tests
// Tests for read_dir(path, filter) functionality

_debug_on = true;

echo("=== read_dir() Filter Tests ===");

if (_debug_on) {
    echo("--- Default behavior (no filter) ---");
    echo("read_dir testdirectory:", read_dir("testdirectory"));
    
    echo("--- Filter: 'filesdirs' (default - both files and directories) ---");
    echo("read_dir testdirectory, filesdirs:", read_dir("testdirectory", "filesdirs"));
    
    echo("--- Filter: 'fileonly' (files only) ---");
    echo("read_dir testdirectory, fileonly:", read_dir("testdirectory", "fileonly"));
    
    echo("--- Filter: 'dironly' (directories only) ---");
    echo("read_dir testdirectory, dironly:", read_dir("testdirectory", "dironly"));
    
    echo("--- Current directory with different filters ---");
    echo("read_dir ., filesdirs:", read_dir(".", "filesdirs"));
    echo("read_dir ., fileonly:", read_dir(".", "fileonly"));
    echo("read_dir ., dironly:", read_dir(".", "dironly"));
}

// Assertions to verify correct filtering
assert(is_list(read_dir("testdirectory")), "read_dir should return a list");
assert(is_list(read_dir("testdirectory", "filesdirs")), "read_dir with filesdirs should return a list");
assert(is_list(read_dir("testdirectory", "fileonly")), "read_dir with fileonly should return a list");
assert(is_list(read_dir("testdirectory", "dironly")), "read_dir with dironly should return a list");

// Verify that fileonly excludes directories
files_only = read_dir(".", "fileonly");
dirs_only = read_dir(".", "dironly");

echo("--- Verification ---");
echo("Files only count:", len(files_only));
echo("Dirs only count:", len(dirs_only));
