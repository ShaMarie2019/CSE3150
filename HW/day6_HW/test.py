import subprocess
import pytest


def run_program(input_data):
    """Run the homework executable with given input and return the result."""
    result = subprocess.run(
        ['./program.out'],
        input=input_data,
        capture_output=True,
        text=True,
        timeout=5
    )
    return result


def extract_counting_lines(stdout):
    """Extract just the number lines from option 4 output (between prompt and next menu)."""
    lines = stdout.split('\n')
    counting_started = False
    count_lines = []
    for line in lines:
        if "Enter a number to count to:" in line:
            counting_started = True
            continue
        if counting_started:
            if "1. Integer operations" in line:
                break
            stripped = line.strip()
            if stripped and stripped.isdigit():
                count_lines.append(stripped)
    return count_lines


class TestOption1:
    def test_normal_division(self):
        r = run_program("1\n10\n2\n6\n")
        assert r.returncode == 0
        assert "Enter first integer:" in r.stdout
        assert "Enter second integer:" in r.stdout
        assert "Result: 5" in r.stdout
        assert "After post-increment: 10" in r.stdout
        assert "After pre-increment: 12" in r.stdout

    def test_division_by_zero(self):
        r = run_program("1\n10\n0\n6\n")
        assert r.returncode == 0
        assert "Enter first integer:" in r.stdout
        assert "Enter second integer:" in r.stdout
        assert "Error: division by zero" in r.stdout
        assert "Result:" not in r.stdout

    def test_negative_numbers(self):
        r = run_program("1\n-20\n4\n6\n")
        assert r.returncode == 0
        assert "Result: -5" in r.stdout
        assert "After post-increment: -20" in r.stdout
        assert "After pre-increment: -18" in r.stdout


class TestOption2:
    def test_basic_string(self):
        r = run_program("2\n5\nHello World\n6\n")
        assert r.returncode == 0
        assert "Enter string length:" in r.stdout
        assert "Enter string:" in r.stdout
        assert "C-style string: Hello" in r.stdout

    def test_short_string(self):
        r = run_program("2\n3\nTest String\n6\n")
        assert r.returncode == 0
        assert "C-style string: Tes" in r.stdout

    def test_length_19_ok(self):
        r = run_program("2\n19\nThis is nineteen!!!\n6\n")
        assert r.returncode == 0
        assert "C-style string: This is nineteen!!!" in r.stdout

    def test_length_20_rejected(self):
        r = run_program("2\n20\n")
        assert r.returncode == 1
        assert "Error:" in r.stdout
        assert "20" in r.stdout or "less than" in r.stdout

    def test_length_25_rejected(self):
        r = run_program("2\n25\n")
        assert r.returncode == 1
        assert "Error:" in r.stdout


class TestOption3:
    def test_undergrad_pass(self):
        r = run_program("3\nU\n75\n6\n")
        assert r.returncode == 0
        assert "Enter student type" in r.stdout
        assert "Enter numeric grade:" in r.stdout
        assert "Status: Pass" in r.stdout

    def test_undergrad_fail(self):
        r = run_program("3\nU\n59\n6\n")
        assert r.returncode == 0
        assert "Status: Fail" in r.stdout

    def test_undergrad_boundary(self):
        r = run_program("3\nu\n60\n6\n")
        assert r.returncode == 0
        assert "Status: Pass" in r.stdout

    def test_grad_pass(self):
        r = run_program("3\nG\n85\n6\n")
        assert r.returncode == 0
        assert "Status: Pass" in r.stdout

    def test_grad_fail(self):
        r = run_program("3\nG\n69\n6\n")
        assert r.returncode == 0
        assert "Status: Fail" in r.stdout

    def test_grad_boundary(self):
        r = run_program("3\ng\n70\n6\n")
        assert r.returncode == 0
        assert "Status: Pass" in r.stdout

    def test_invalid_grade_low(self):
        r = run_program("3\nU\n-1\n")
        assert r.returncode == 1
        assert "Invalid grade" in r.stdout

    def test_invalid_grade_high(self):
        r = run_program("3\nU\n101\n")
        assert r.returncode == 1
        assert "Invalid grade" in r.stdout


class TestOption4:
    def test_count_to_5_skips_5(self):
        r = run_program("4\n5\n6\n")
        assert r.returncode == 0
        count_lines = extract_counting_lines(r.stdout)
        assert count_lines == ['1', '2', '3', '4']

    def test_count_to_10_skips_5(self):
        r = run_program("4\n10\n6\n")
        assert r.returncode == 0
        count_lines = extract_counting_lines(r.stdout)
        assert count_lines == ['1', '2', '3', '4', '6', '7', '8', '9', '10']

    def test_reject_over_10(self):
        r = run_program("4\n15\n8\n6\n")
        assert r.returncode == 0
        assert "I'm programmed to only count up to 10!" in r.stdout
        assert "Enter a number to count to:" in r.stdout
        assert "8" in r.stdout

    def test_multiple_rejections(self):
        r = run_program("4\n20\n11\n7\n6\n")
        assert r.returncode == 0
        assert "I'm programmed to only count up to 10!" in r.stdout
        assert "7" in r.stdout


class TestOption5:
    def test_input_1(self):
        r = run_program("5\n1\n6\n")
        assert r.returncode == 0
        assert "Enter a number between 1 and 5:" in r.stdout
        assert "Value: 1" in r.stdout
        assert "Value: 2" not in r.stdout

    def test_input_3(self):
        r = run_program("5\n3\n6\n")
        assert r.returncode == 0
        assert "Value: 1" in r.stdout
        assert "Value: 2" in r.stdout
        assert "Value: 3" in r.stdout
        assert "Value: 4" not in r.stdout

    def test_input_5(self):
        r = run_program("5\n5\n6\n")
        assert r.returncode == 0
        assert "Value: 1" in r.stdout
        assert "Value: 2" in r.stdout
        assert "Value: 3" in r.stdout
        assert "Value: 4" in r.stdout
        assert "Value: 5" in r.stdout

    def test_reject_invalid_low(self):
        r = run_program("5\n0\n6\n2\n6\n")
        assert r.returncode == 0
        assert r.stdout.count("Enter a number between 1 and 5:") >= 2
        assert "Value: 1" in r.stdout
        assert "Value: 2" in r.stdout

    def test_reject_invalid_high(self):
        r = run_program("5\n10\n4\n6\n")
        assert r.returncode == 0
        assert r.stdout.count("Enter a number between 1 and 5:") >= 2
        assert "Value: 4" in r.stdout


class TestOption6:
    def test_quit(self):
        r = run_program("6\n")
        assert r.returncode == 0
        assert "Goodbye!" in r.stdout


class TestMenu:
    def test_invalid_menu_option(self):
        r = run_program("99\n6\n")
        assert r.returncode == 0
        assert "1. Integer operations" in r.stdout
        assert "6. Quit" in r.stdout
        assert "Goodbye!" in r.stdout

    def test_menu_loops_after_option(self):
        r = run_program("1\n10\n2\n6\n")
        assert r.returncode == 0
        assert "Result: 5" in r.stdout
        # Menu text should appear at least twice (before option 1 and after it completes)
        assert r.stdout.count("1. Integer operations") >= 2
        assert "Goodbye!" in r.stdout
