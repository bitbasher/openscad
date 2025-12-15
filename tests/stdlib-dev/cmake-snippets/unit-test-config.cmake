# CMake Configuration for C++ Unit Tests
#
# This file contains CMake snippets for enabling C++ unit tests for the
# stdlib string and file functions. Currently disabled due to GCC 15 / CGAL
# linker compatibility issues in MSYS2/MinGW64.
#
# STATUS: DISABLED (pending toolchain fix)
# ISSUE: GCC 15.2.0 causes ~900 linker errors with CGAL template instantiations
#        (std::iter_swap, std::__make_heap, etc.) unrelated to our code.
#
# =============================================================================
# HOW IT WORKS (when enabled)
# =============================================================================
#
# The current CMakeLists.txt uses a glob pattern to find test files:
#
#   file(GLOB_RECURSE TEST_SOURCES
#     "src/*_test.cc"
#   )
#
# This automatically picks up any file matching `*_test.cc` in the src/ tree.
# The test file `builtin_functions_strings_test.cc` follows this pattern.
#
# =============================================================================
# TO RE-ENABLE UNIT TESTS
# =============================================================================
#
# 1. Move the test file back to src/core/:
#
#    mv tests/stdlib-dev/unit-tests/builtin_functions_strings_test.cc \
#       src/core/builtin_functions_strings_test.cc
#
# 2. Export the utility functions from builtin_functions_strings.cc:
#
#    The test file calls functions like string_upper(), string_lower(), etc.
#    These are currently static (internal linkage). To make them testable:
#
#    a) Remove the 'static' keyword from each function in .cc file
#    b) Add declarations to builtin_functions_strings.h:
#
#       // Utility functions (exported for testing)
#       std::string string_upper(const std::string& str);
#       std::string string_lower(const std::string& str);
#       std::string string_trim(const std::string& str);
#       std::string string_ltrim(const std::string& str);
#       std::string string_rtrim(const std::string& str);
#       std::vector<std::string> string_split(const std::string& str, const std::string& delim);
#       std::string string_join(const std::vector<std::string>& parts, const std::string& delim);
#       std::string string_replace(const std::string& str, const std::string& from, const std::string& to);
#       bool string_starts_with(const std::string& str, const std::string& prefix);
#       bool string_ends_with(const std::string& str, const std::string& suffix);
#       bool string_contains(const std::string& str, const std::string& sub);
#       std::string string_substr(const std::string& str, ssize_t start, ssize_t length);
#       ssize_t string_index_of(const std::string& str, const std::string& sub);
#       std::string string_pad_left(const std::string& str, size_t width, char pad_char);
#       std::string string_pad_right(const std::string& str, size_t width, char pad_char);
#
# 3. Rebuild:
#
#    cd build
#    cmake ..
#    cmake --build . --target OpenSCADUnitTests -j4
#
# 4. Run tests:
#
#    ./OpenSCADUnitTests --list-tests              # List all tests
#    ./OpenSCADUnitTests "[string]"                # Run string tests only
#    ./OpenSCADUnitTests "[string][upper]"         # Run specific test
#
# =============================================================================
# ALTERNATIVE: Keep functions static, test via OpenSCAD
# =============================================================================
#
# If you prefer to keep the C++ functions as static (better encapsulation),
# test them via OpenSCAD regression tests instead:
#
#   tests/data/scad/misc/string-functions-tests.scad
#
# Run with:
#
#   cd build
#   ctest -R string-functions
#
# =============================================================================
