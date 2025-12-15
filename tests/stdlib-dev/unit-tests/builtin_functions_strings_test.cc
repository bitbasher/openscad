/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2024 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <catch2/catch_all.hpp>
#include <string>

// Include the string utility functions we're testing
#include "core/builtin_functions_strings.h"

TEST_CASE("upper() converts strings to uppercase", "[string][upper]")
{
  SECTION("Basic ASCII lowercase to uppercase")
  {
    CHECK(string_upper("hello") == "HELLO");
    CHECK(string_upper("world") == "WORLD");
  }

  SECTION("Mixed case ASCII")
  {
    CHECK(string_upper("Hello World") == "HELLO WORLD");
    CHECK(string_upper("hElLo WoRlD") == "HELLO WORLD");
  }

  SECTION("Already uppercase")
  {
    CHECK(string_upper("HELLO") == "HELLO");
  }

  SECTION("Empty string")
  {
    CHECK(string_upper("") == "");
  }

  SECTION("Non-alphabetic characters unchanged")
  {
    CHECK(string_upper("hello123!@#") == "HELLO123!@#");
    CHECK(string_upper("test_case") == "TEST_CASE");
    CHECK(string_upper("hello-world") == "HELLO-WORLD");
  }

  SECTION("UTF-8 characters")
  {
    // German eszett: ß -> SS (special case in Unicode)
    CHECK(string_upper("straße") == "STRASSE");
    // Greek lowercase to uppercase
    CHECK(string_upper("αβγ") == "ΑΒΓ");
    // Cyrillic lowercase to uppercase
    CHECK(string_upper("привет") == "ПРИВЕТ");
  }
}

TEST_CASE("lower() converts strings to lowercase", "[string][lower]")
{
  SECTION("Basic ASCII uppercase to lowercase")
  {
    CHECK(string_lower("HELLO") == "hello");
    CHECK(string_lower("WORLD") == "world");
  }

  SECTION("Mixed case ASCII")
  {
    CHECK(string_lower("Hello World") == "hello world");
    CHECK(string_lower("hElLo WoRlD") == "hello world");
  }

  SECTION("Already lowercase")
  {
    CHECK(string_lower("hello") == "hello");
  }

  SECTION("Empty string")
  {
    CHECK(string_lower("") == "");
  }

  SECTION("Non-alphabetic characters unchanged")
  {
    CHECK(string_lower("HELLO123!@#") == "hello123!@#");
    CHECK(string_lower("TEST_CASE") == "test_case");
    CHECK(string_lower("HELLO-WORLD") == "hello-world");
  }

  SECTION("UTF-8 characters")
  {
    // Greek uppercase to lowercase
    CHECK(string_lower("ΑΒΓ") == "αβγ");
    // Cyrillic uppercase to lowercase
    CHECK(string_lower("ПРИВЕТ") == "привет");
  }
}

TEST_CASE("trim() removes whitespace from both ends", "[string][trim]")
{
  SECTION("Leading whitespace")
  {
    CHECK(string_trim("  hello") == "hello");
    CHECK(string_trim("\thello") == "hello");
    CHECK(string_trim("\nhello") == "hello");
    CHECK(string_trim("   \t\n  hello") == "hello");
  }

  SECTION("Trailing whitespace")
  {
    CHECK(string_trim("hello  ") == "hello");
    CHECK(string_trim("hello\t") == "hello");
    CHECK(string_trim("hello\n") == "hello");
    CHECK(string_trim("hello   \t\n  ") == "hello");
  }

  SECTION("Both ends")
  {
    CHECK(string_trim("  hello  ") == "hello");
    CHECK(string_trim("\t\nhello\t\n") == "hello");
    CHECK(string_trim("   hello world   ") == "hello world");
  }

  SECTION("No whitespace")
  {
    CHECK(string_trim("hello") == "hello");
    CHECK(string_trim("hello world") == "hello world");
  }

  SECTION("Empty string")
  {
    CHECK(string_trim("") == "");
  }

  SECTION("Only whitespace")
  {
    CHECK(string_trim("   ") == "");
    CHECK(string_trim("\t\n\r ") == "");
  }

  SECTION("Preserves internal whitespace")
  {
    CHECK(string_trim("  hello   world  ") == "hello   world");
  }
}

TEST_CASE("ltrim() removes whitespace from left end only", "[string][ltrim]")
{
  SECTION("Leading whitespace")
  {
    CHECK(string_ltrim("  hello") == "hello");
    CHECK(string_ltrim("\thello") == "hello");
    CHECK(string_ltrim("\nhello") == "hello");
  }

  SECTION("Trailing whitespace preserved")
  {
    CHECK(string_ltrim("hello  ") == "hello  ");
    CHECK(string_ltrim("  hello  ") == "hello  ");
  }

  SECTION("No whitespace")
  {
    CHECK(string_ltrim("hello") == "hello");
  }

  SECTION("Empty string")
  {
    CHECK(string_ltrim("") == "");
  }

  SECTION("Only whitespace")
  {
    CHECK(string_ltrim("   ") == "");
  }
}

TEST_CASE("rtrim() removes whitespace from right end only", "[string][rtrim]")
{
  SECTION("Trailing whitespace")
  {
    CHECK(string_rtrim("hello  ") == "hello");
    CHECK(string_rtrim("hello\t") == "hello");
    CHECK(string_rtrim("hello\n") == "hello");
  }

  SECTION("Leading whitespace preserved")
  {
    CHECK(string_rtrim("  hello") == "  hello");
    CHECK(string_rtrim("  hello  ") == "  hello");
  }

  SECTION("No whitespace")
  {
    CHECK(string_rtrim("hello") == "hello");
  }

  SECTION("Empty string")
  {
    CHECK(string_rtrim("") == "");
  }

  SECTION("Only whitespace")
  {
    CHECK(string_rtrim("   ") == "");
  }
}

TEST_CASE("split() splits strings by delimiter", "[string][split]")
{
  SECTION("Split by comma")
  {
    auto result = string_split("a,b,c", ",");
    REQUIRE(result.size() == 3);
    CHECK(result[0] == "a");
    CHECK(result[1] == "b");
    CHECK(result[2] == "c");
  }

  SECTION("Split by space")
  {
    auto result = string_split("hello world foo", " ");
    REQUIRE(result.size() == 3);
    CHECK(result[0] == "hello");
    CHECK(result[1] == "world");
    CHECK(result[2] == "foo");
  }

  SECTION("Empty delimiter splits into characters")
  {
    auto result = string_split("abc", "");
    REQUIRE(result.size() == 3);
    CHECK(result[0] == "a");
    CHECK(result[1] == "b");
    CHECK(result[2] == "c");
  }

  SECTION("UTF-8 character split")
  {
    auto result = string_split("αβγ", "");
    REQUIRE(result.size() == 3);
    CHECK(result[0] == "α");
    CHECK(result[1] == "β");
    CHECK(result[2] == "γ");
  }

  SECTION("Empty string returns empty vector")
  {
    auto result = string_split("", ",");
    CHECK(result.empty());
  }

  SECTION("No delimiter found")
  {
    auto result = string_split("hello", ",");
    REQUIRE(result.size() == 1);
    CHECK(result[0] == "hello");
  }
}

TEST_CASE("join() joins strings with delimiter", "[string][join]")
{
  SECTION("Join with comma")
  {
    std::vector<std::string> parts = {"a", "b", "c"};
    CHECK(string_join(parts, ",") == "a,b,c");
  }

  SECTION("Join with space")
  {
    std::vector<std::string> parts = {"hello", "world"};
    CHECK(string_join(parts, " ") == "hello world");
  }

  SECTION("Join with empty delimiter")
  {
    std::vector<std::string> parts = {"a", "b", "c"};
    CHECK(string_join(parts, "") == "abc");
  }

  SECTION("Join empty vector")
  {
    std::vector<std::string> parts = {};
    CHECK(string_join(parts, ",") == "");
  }

  SECTION("Join single element")
  {
    std::vector<std::string> parts = {"hello"};
    CHECK(string_join(parts, ",") == "hello");
  }
}

TEST_CASE("replace() replaces all occurrences", "[string][replace]")
{
  SECTION("Single occurrence")
  {
    CHECK(string_replace("hello world", "world", "there") == "hello there");
  }

  SECTION("Multiple occurrences")
  {
    CHECK(string_replace("hello hello hello", "hello", "hi") == "hi hi hi");
  }

  SECTION("No occurrence")
  {
    CHECK(string_replace("hello world", "foo", "bar") == "hello world");
  }

  SECTION("Empty old string")
  {
    CHECK(string_replace("hello", "", "x") == "hello");
  }

  SECTION("Empty new string (deletion)")
  {
    CHECK(string_replace("hello world", " world", "") == "hello");
  }

  SECTION("UTF-8 replacement")
  {
    CHECK(string_replace("αβγ", "β", "X") == "αXγ");
  }
}

TEST_CASE("starts_with() checks string prefix", "[string][starts_with]")
{
  SECTION("Matching prefix")
  {
    CHECK(string_starts_with("hello world", "hello") == true);
    CHECK(string_starts_with("hello", "hello") == true);
  }

  SECTION("Non-matching prefix")
  {
    CHECK(string_starts_with("hello world", "world") == false);
    CHECK(string_starts_with("hello", "hello world") == false);
  }

  SECTION("Empty prefix")
  {
    CHECK(string_starts_with("hello", "") == true);
  }

  SECTION("Empty string")
  {
    CHECK(string_starts_with("", "hello") == false);
    CHECK(string_starts_with("", "") == true);
  }
}

TEST_CASE("ends_with() checks string suffix", "[string][ends_with]")
{
  SECTION("Matching suffix")
  {
    CHECK(string_ends_with("hello world", "world") == true);
    CHECK(string_ends_with("hello", "hello") == true);
  }

  SECTION("Non-matching suffix")
  {
    CHECK(string_ends_with("hello world", "hello") == false);
    CHECK(string_ends_with("hello", "hello world") == false);
  }

  SECTION("Empty suffix")
  {
    CHECK(string_ends_with("hello", "") == true);
  }

  SECTION("Empty string")
  {
    CHECK(string_ends_with("", "hello") == false);
    CHECK(string_ends_with("", "") == true);
  }
}

TEST_CASE("contains() checks for substring", "[string][contains]")
{
  SECTION("Substring present")
  {
    CHECK(string_contains("hello world", "world") == true);
    CHECK(string_contains("hello world", "hello") == true);
    CHECK(string_contains("hello world", "lo wo") == true);
    CHECK(string_contains("hello", "hello") == true);
  }

  SECTION("Substring not present")
  {
    CHECK(string_contains("hello world", "foo") == false);
    CHECK(string_contains("hello", "hello world") == false);
  }

  SECTION("Empty substring")
  {
    CHECK(string_contains("hello", "") == true);
  }

  SECTION("Empty string")
  {
    CHECK(string_contains("", "hello") == false);
    CHECK(string_contains("", "") == true);
  }
}

TEST_CASE("substr() extracts substrings", "[string][substr]")
{
  SECTION("Basic extraction")
  {
    CHECK(string_substr("hello world", 0, 5) == "hello");
    CHECK(string_substr("hello world", 6, 5) == "world");
    CHECK(string_substr("hello world", 6) == "world");
  }

  SECTION("Negative start (from end)")
  {
    CHECK(string_substr("hello world", -5) == "world");
    CHECK(string_substr("hello world", -5, 3) == "wor");
  }

  SECTION("Length exceeds remaining")
  {
    CHECK(string_substr("hello", 3, 100) == "lo");
  }

  SECTION("Start beyond string length")
  {
    CHECK(string_substr("hello", 100) == "");
  }

  SECTION("UTF-8 substring")
  {
    CHECK(string_substr("αβγδ", 1, 2) == "βγ");
    CHECK(string_substr("αβγδ", -2) == "γδ");
  }

  SECTION("Empty string")
  {
    CHECK(string_substr("", 0, 5) == "");
  }
}

TEST_CASE("index_of() finds substring position", "[string][index_of]")
{
  SECTION("Substring found")
  {
    CHECK(string_index_of("hello world", "world") == 6);
    CHECK(string_index_of("hello world", "hello") == 0);
    CHECK(string_index_of("hello world", "o") == 4);
  }

  SECTION("Substring not found")
  {
    CHECK(string_index_of("hello world", "foo") == -1);
  }

  SECTION("Empty substring")
  {
    CHECK(string_index_of("hello", "") == 0);
  }

  SECTION("UTF-8 index")
  {
    CHECK(string_index_of("αβγδ", "γ") == 2);
  }
}

TEST_CASE("pad_left() pads string on left", "[string][pad_left]")
{
  SECTION("Padding needed")
  {
    CHECK(string_pad_left("hello", 10) == "     hello");
    CHECK(string_pad_left("hello", 10, '0') == "00000hello");
  }

  SECTION("No padding needed")
  {
    CHECK(string_pad_left("hello", 5) == "hello");
    CHECK(string_pad_left("hello", 3) == "hello");
  }

  SECTION("Empty string")
  {
    CHECK(string_pad_left("", 5) == "     ");
  }

  SECTION("UTF-8 string")
  {
    CHECK(string_pad_left("αβ", 5) == "   αβ");
  }
}

TEST_CASE("pad_right() pads string on right", "[string][pad_right]")
{
  SECTION("Padding needed")
  {
    CHECK(string_pad_right("hello", 10) == "hello     ");
    CHECK(string_pad_right("hello", 10, '0') == "hello00000");
  }

  SECTION("No padding needed")
  {
    CHECK(string_pad_right("hello", 5) == "hello");
    CHECK(string_pad_right("hello", 3) == "hello");
  }

  SECTION("Empty string")
  {
    CHECK(string_pad_right("", 5) == "     ");
  }

  SECTION("UTF-8 string")
  {
    CHECK(string_pad_right("αβ", 5) == "αβ   ");
  }
}
