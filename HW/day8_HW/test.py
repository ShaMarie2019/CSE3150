"""
Tests for CSE3150 Homework 2: analyzer program.

To compile before running tests:
    g++ -o analyzer src/main.cpp src/text_mode.cpp src/add_mode.cpp src/stats_mode.cpp -Iinclude

To run tests:
    pytest test.py -v
"""

import subprocess
import os
import re
import pytest

PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
ANALYZER = os.path.join(PROJECT_DIR, "analyzer")


def run_analyzer(*args):
    """Run the analyzer binary with given arguments."""
    result = subprocess.run(
        [ANALYZER] + list(args),
        capture_output=True,
        text=True,
        timeout=5,
    )
    return result


def read_source(relpath):
    """Read a source file relative to the project directory."""
    path = os.path.join(PROJECT_DIR, relpath)
    with open(path) as f:
        return f.read()


def normalize(s):
    """Remove all non-alphanumeric characters and lowercase."""
    return re.sub(r"[^a-z0-9]", "", s.lower())


def output_contains(stdout, expected):
    """Check if normalized expected is found within normalized stdout."""
    return normalize(expected) in normalize(stdout)


# ============================================================
# Main Function Structure Tests
# ============================================================
class TestMainStructure:
    """main.cpp must follow specific structural requirements."""

    def test_has_switch_statement(self):
        content = read_source("src/main.cpp")
        assert re.search(r"\bswitch\b", content), (
            "main.cpp must contain a switch statement"
        )

    def test_no_mode_logic(self):
        """main.cpp must not contain mode-specific logic."""
        content = read_source("src/main.cpp")
        forbidden = [
            "isalpha", "isdigit", "isspace", "ispunct", "isalnum",
            "stoi", "stod", "toupper", "tolower", "substr",
        ]
        for func in forbidden:
            assert func not in content, (
                f"main.cpp should not contain '{func}' — mode logic belongs in mode files"
            )

    def test_calls_namespaces(self):
        """main.cpp should call into the three mode namespaces."""
        content = read_source("src/main.cpp")
        assert "text_mode::" in content, "main.cpp must call text_mode::run"
        assert "add_mode::" in content, "main.cpp must call add_mode::run"
        assert "stats_mode::" in content, "main.cpp must call stats_mode::run"

    def test_includes_all_headers(self):
        content = read_source("src/main.cpp")
        assert '"text_mode.h"' in content, "main.cpp must include text_mode.h"
        assert '"add_mode.h"' in content, "main.cpp must include add_mode.h"
        assert '"stats_mode.h"' in content, "main.cpp must include stats_mode.h"


# ============================================================
# Restrictions Tests
# ============================================================
class TestRestrictions:
    """No global variables, no external libraries, no regex."""

    @pytest.mark.parametrize(
        "src",
        [
            "src/main.cpp",
            "src/text_mode.cpp",
            "src/add_mode.cpp",
            "src/stats_mode.cpp",
        ],
    )
    def test_no_regex(self, src):
        content = read_source(src)
        assert "<regex>" not in content, f"{src} must not use <regex>"

    @pytest.mark.parametrize(
        "src",
        [
            "src/main.cpp",
            "src/text_mode.cpp",
            "src/add_mode.cpp",
            "src/stats_mode.cpp",
        ],
    )
    def test_no_external_libraries(self, src):
        """Only standard C++ library headers allowed."""
        content = read_source(src)
        external = ["<boost/", "<fmt/", "<nlohmann/", "<curl/"]
        for lib in external:
            assert lib not in content, f"{src} must not use external library {lib}"


# ============================================================
# Code Requirements Tests
# ============================================================
class TestTextModeCodeRequirements:
    """text_mode.cpp must use specific constructs."""

    def test_range_based_for(self):
        content = read_source("src/text_mode.cpp")
        assert re.search(r"for\s*\(.*:.*\)", content), (
            "text_mode.cpp must use a range-based for loop"
        )

    def test_index_based_for_with_size_t(self):
        content = read_source("src/text_mode.cpp")
        assert re.search(r"for\s*\(\s*size_t", content), (
            "text_mode.cpp must use an index-based for loop with size_t"
        )

    def test_uses_size_t(self):
        content = read_source("src/text_mode.cpp")
        assert "size_t" in content, "text_mode.cpp must use size_t"

    def test_uses_isalpha(self):
        content = read_source("src/text_mode.cpp")
        assert "isalpha" in content

    def test_uses_isdigit(self):
        content = read_source("src/text_mode.cpp")
        assert "isdigit" in content

    def test_uses_isspace(self):
        content = read_source("src/text_mode.cpp")
        assert "isspace" in content

    def test_uses_ispunct(self):
        content = read_source("src/text_mode.cpp")
        assert "ispunct" in content

    def test_uses_find(self):
        content = read_source("src/text_mode.cpp")
        assert ".find(" in content, "text_mode.cpp must use .find()"

    def test_uses_substr(self):
        content = read_source("src/text_mode.cpp")
        assert ".substr(" in content, "text_mode.cpp must use .substr()"

    def test_uses_size(self):
        content = read_source("src/text_mode.cpp")
        assert ".size()" in content, "text_mode.cpp must use .size()"


class TestAddModeCodeRequirements:
    """add_mode.cpp must use specific constructs."""

    def test_uses_stoi(self):
        content = read_source("src/add_mode.cpp")
        assert "stoi" in content, "add_mode.cpp must use std::stoi"

    def test_uses_stod(self):
        content = read_source("src/add_mode.cpp")
        assert "stod" in content, "add_mode.cpp must use std::stod"

    def test_uses_to_string(self):
        content = read_source("src/add_mode.cpp")
        assert "to_string" in content, "add_mode.cpp must use std::to_string"

    def test_uses_find_dot(self):
        content = read_source("src/add_mode.cpp")
        assert ".find(" in content, "add_mode.cpp must use .find('.')"

    def test_three_add_overloads_defined(self):
        """add_mode.cpp must define all three add overloads."""
        content = read_source("src/add_mode.cpp")
        assert re.search(r"int\s+add\s*\(\s*int", content)
        assert re.search(r"double\s+add\s*\(\s*double", content)
        assert re.search(r"string\s+add\s*\(\s*std::string|string\s+add\s*\(\s*string", content)


class TestStatsModeCodeRequirements:
    """stats_mode.cpp must use specific constructs."""

    def test_range_based_for(self):
        content = read_source("src/stats_mode.cpp")
        assert re.search(r"for\s*\(.*:.*\)", content), (
            "stats_mode.cpp must use a range-based for loop"
        )

    def test_index_based_with_size_t(self):
        content = read_source("src/stats_mode.cpp")
        assert "size_t" in content, "stats_mode.cpp must use size_t"
        assert re.search(r"for\s*\(\s*size_t", content) or re.search(
            r"\w+\[", content
        ), "stats_mode.cpp must use index-based access"

    def test_uses_isalnum(self):
        content = read_source("src/stats_mode.cpp")
        assert "isalnum" in content, "stats_mode.cpp must use std::isalnum"

    def test_uses_tolower(self):
        content = read_source("src/stats_mode.cpp")
        assert "tolower" in content, "stats_mode.cpp must use std::tolower"


# ============================================================
# Runtime Tests — Usage and Error Handling
# ============================================================
class TestUsageErrors:
    """Test program behavior with invalid or missing arguments."""

    def test_no_arguments_prints_usage(self):
        result = run_analyzer()
        assert output_contains(result.stdout, "Usage ./analyzer mode arguments")

    def test_no_arguments_returns_1(self):
        result = run_analyzer()
        assert result.returncode == 1

    def test_invalid_mode_prints_error(self):
        result = run_analyzer("invalid")
        assert output_contains(result.stdout, "Invalid mode")

    def test_invalid_mode_returns_1(self):
        result = run_analyzer("invalid")
        assert result.returncode == 1

    @pytest.mark.parametrize(
        "mode", ["foo", "bar", "TEXT", "ADD", "STATS", "123", ""]
    )
    def test_various_invalid_modes(self, mode):
        result = run_analyzer(mode)
        assert result.returncode == 1
        assert output_contains(result.stdout, "Invalid mode")

    def test_text_mode_missing_arg(self):
        result = run_analyzer("text")
        assert result.returncode == 0 or output_contains(result.stdout, "Usage")

    def test_add_mode_missing_args(self):
        result = run_analyzer("add")
        assert result.returncode == 0 or output_contains(result.stdout, "Usage")

    def test_add_mode_missing_second_arg(self):
        result = run_analyzer("add", "5")
        assert result.returncode == 0 or output_contains(result.stdout, "Usage")

    def test_stats_mode_missing_arg(self):
        result = run_analyzer("stats")
        assert result.returncode == 0 or output_contains(result.stdout, "Usage")


# ============================================================
# Runtime Tests — Text Mode
# ============================================================
class TestTextModeRuntime:
    """Test text mode output for various inputs."""

    def test_original_printed(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Original Hello World 123")

    def test_length(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Length 16")

    def test_letter_count(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Letters 10")

    def test_digit_count(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Digits 3")

    def test_space_count(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Spaces 2")

    def test_punctuation_count(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Punctuation 1")

    def test_uppercase_copy(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Uppercase HELLO WORLD 123")

    def test_lowercase_inplace(self):
        result = run_analyzer("text", "Hello World 123!")
        assert output_contains(result.stdout, "Lowercase hello world 123")

    def test_contains_test_no(self):
        result = run_analyzer("text", "Hello World")
        assert output_contains(result.stdout, "containstestno")

    def test_contains_test_yes_case_insensitive(self):
        """After lowercasing, 'Testing' becomes 'testing' which contains 'test'."""
        result = run_analyzer("text", "Testing 123")
        assert output_contains(result.stdout, "containstestyes")

    def test_first_word_with_space(self):
        result = run_analyzer("text", "Hello World")
        assert output_contains(result.stdout, "First word hello")

    def test_first_word_not_printed_without_space(self):
        result = run_analyzer("text", "hello")
        assert not output_contains(result.stdout, "firstword")

    def test_all_letters(self):
        result = run_analyzer("text", "abcdef")
        out = result.stdout
        assert output_contains(out, "Letters 6")
        assert output_contains(out, "Digits 0")
        assert output_contains(out, "Spaces 0")
        assert output_contains(out, "Punctuation 0")

    def test_all_digits_input(self):
        result = run_analyzer("text", "12345")
        out = result.stdout
        assert output_contains(out, "Letters 0")
        assert output_contains(out, "Digits 5")

    def test_punctuation_only(self):
        result = run_analyzer("text", "!@#")
        out = result.stdout
        assert output_contains(out, "Punctuation 3")
        assert output_contains(out, "Letters 0")

    def test_empty_string(self):
        result = run_analyzer("text", "")
        assert output_contains(result.stdout, "Length 0")

    def test_output_order(self):
        """Verify outputs appear in the correct order."""
        result = run_analyzer("text", "Hi there")
        out = normalize(result.stdout)
        expected = [
            "original", "length", "letters", "digits", "spaces",
            "punctuation", "uppercase", "lowercase", "containstest",
            "firstword",
        ]
        positions = []
        for label in expected:
            pos = out.find(label)
            assert pos != -1, f"Missing label in output: {label}"
            positions.append(pos)
        assert positions == sorted(positions), "Output labels are not in the expected order"


# ============================================================
# Runtime Tests — Add Mode
# ============================================================
class TestAddModeRuntime:
    """Test add mode with various input types."""

    def test_integer_addition(self):
        result = run_analyzer("add", "3", "5")
        assert output_contains(result.stdout, "Result 8")

    def test_integer_addition_large(self):
        result = run_analyzer("add", "100", "200")
        assert output_contains(result.stdout, "Result 300")

    def test_integer_addition_zero(self):
        result = run_analyzer("add", "0", "0")
        assert output_contains(result.stdout, "Result 0")

    def test_double_addition(self):
        result = run_analyzer("add", "3.5", "2.1")
        assert output_contains(result.stdout, "Result 56")

    def test_double_addition_one_decimal(self):
        """One integer, one decimal -> double path."""
        result = run_analyzer("add", "5", "3.5")
        assert output_contains(result.stdout, "Result 85")

    def test_string_concatenation(self):
        result = run_analyzer("add", "hello", "world")
        assert output_contains(result.stdout, "Result helloworld")

    def test_string_concatenation_mixed(self):
        result = run_analyzer("add", "abc", "123xyz")
        assert output_contains(result.stdout, "Result abc123xyz")

    def test_negative_numbers_as_strings(self):
        """Negative sign means not 'only digits', no decimal -> string concat."""
        result = run_analyzer("add", "-5", "-3")
        assert output_contains(result.stdout, "Result 53")

    def test_both_decimals(self):
        result = run_analyzer("add", "1.5", "2.5")
        assert output_contains(result.stdout, "Result 4")

    def test_result_prefix(self):
        """Output must contain 'Result'."""
        result = run_analyzer("add", "1", "2")
        assert output_contains(result.stdout, "Result 3")


# ============================================================
# Runtime Tests — Stats Mode
# ============================================================
class TestStatsModeRuntime:
    """Test stats mode output for various inputs."""

    def test_length(self):
        result = run_analyzer("stats", "racecar")
        assert output_contains(result.stdout, "Length 7")

    def test_unique_count(self):
        result = run_analyzer("stats", "racecar")
        assert output_contains(result.stdout, "Unique 4")

    def test_alphanumeric_yes(self):
        result = run_analyzer("stats", "racecar")
        assert output_contains(result.stdout, "Alphanumeric Yes")

    def test_alphanumeric_no_with_space(self):
        result = run_analyzer("stats", "Race Car")
        assert output_contains(result.stdout, "Alphanumeric No")

    def test_alphanumeric_no_with_punctuation(self):
        result = run_analyzer("stats", "hello!")
        assert output_contains(result.stdout, "Alphanumeric No")

    def test_reversed(self):
        result = run_analyzer("stats", "hello")
        assert output_contains(result.stdout, "Reversed olleh")

    def test_reversed_with_spaces(self):
        result = run_analyzer("stats", "Race Car")
        assert output_contains(result.stdout, "Reversed raC ecaR")

    def test_without_spaces(self):
        result = run_analyzer("stats", "Race Car")
        assert output_contains(result.stdout, "Without spaces RaceCar")

    def test_without_spaces_no_change(self):
        result = run_analyzer("stats", "hello")
        assert output_contains(result.stdout, "withoutspaceshello")

    def test_palindrome_yes(self):
        result = run_analyzer("stats", "racecar")
        assert output_contains(result.stdout, "Palindrome Yes")

    def test_palindrome_no(self):
        result = run_analyzer("stats", "hello")
        assert output_contains(result.stdout, "Palindrome No")

    def test_palindrome_case_insensitive(self):
        """'Racecar' should be detected as palindrome."""
        result = run_analyzer("stats", "Racecar")
        assert output_contains(result.stdout, "Palindrome Yes")

    def test_palindrome_with_spaces(self):
        """'Race Car' with spaces removed and lowered is 'racecar' -> palindrome."""
        result = run_analyzer("stats", "Race Car")
        assert output_contains(result.stdout, "Palindrome Yes")

    def test_single_character(self):
        result = run_analyzer("stats", "a")
        out = result.stdout
        assert output_contains(out, "Length 1")
        assert output_contains(out, "Unique 1")
        assert output_contains(out, "Palindrome Yes")

    def test_numeric_palindrome(self):
        result = run_analyzer("stats", "12321")
        out = result.stdout
        assert output_contains(out, "Length 5")
        assert output_contains(out, "Alphanumeric Yes")
        assert output_contains(out, "Palindrome Yes")

    def test_unique_count_with_spaces(self):
        result = run_analyzer("stats", "Race Car")
        assert output_contains(result.stdout, "Unique 7")

    def test_unique_count_hello(self):
        result = run_analyzer("stats", "hello")
        assert output_contains(result.stdout, "Unique 4")

    def test_output_order(self):
        """Verify stats outputs appear in the correct order."""
        result = run_analyzer("stats", "test")
        out = normalize(result.stdout)
        expected = [
            "length", "unique", "alphanumeric", "reversed",
            "withoutspaces", "palindrome",
        ]
        positions = []
        for label in expected:
            pos = out.find(label)
            assert pos != -1, f"Missing label in output: {label}"
            positions.append(pos)
        assert positions == sorted(positions), "Output labels are not in the expected order"


# ============================================================
# Integration / Edge Case Tests
# ============================================================
class TestEdgeCases:
    """Test edge cases across modes."""

    def test_text_mode_returns_0(self):
        result = run_analyzer("text", "hello")
        assert result.returncode == 0

    def test_add_mode_returns_0(self):
        result = run_analyzer("add", "1", "2")
        assert result.returncode == 0

    def test_stats_mode_returns_0(self):
        result = run_analyzer("stats", "hello")
        assert result.returncode == 0

    def test_text_special_characters(self):
        result = run_analyzer("text", "a1 !")
        out = result.stdout
        assert output_contains(out, "Letters 1")
        assert output_contains(out, "Digits 1")
        assert output_contains(out, "Spaces 1")
        assert output_contains(out, "Punctuation 1")

    def test_add_single_digit(self):
        result = run_analyzer("add", "0", "1")
        assert output_contains(result.stdout, "Result 1")

    def test_stats_empty_string(self):
        result = run_analyzer("stats", "")
        out = result.stdout
        assert output_contains(out, "Length 0")
        assert output_contains(out, "Unique 0")
        assert output_contains(out, "Palindrome Yes")

    def test_no_stderr_output_text(self):
        """Program should not write to stderr under normal operation."""
        result = run_analyzer("text", "hello")
        assert result.stderr == ""

    def test_no_stderr_output_add(self):
        result = run_analyzer("add", "1", "2")
        assert result.stderr == ""

    def test_no_stderr_output_stats(self):
        result = run_analyzer("stats", "hello")
        assert result.stderr == ""
