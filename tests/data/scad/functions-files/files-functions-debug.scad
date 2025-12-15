echo("=== File Function Debug ===");

// Test absolute_path to see what paths resolve to
echo("absolute_path of .:", absolute_path("."));
echo("absolute_path of ..:", absolute_path(".."));
echo("absolute_path of ../..:", absolute_path("../.."));
echo("absolute_path of ../../../../CMakeLists.txt:", absolute_path("../../../../CMakeLists.txt"));

// Test with the current file's directory
echo("file_exists on current dir .:", file_exists("."));
echo("is_dir on current dir .:", is_dir("."));

// Try reading the current directory
echo("read_dir .:", read_dir("."));

echo("=== End Debug ===");
