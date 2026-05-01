import pytest
import subprocess
import os
import csv
import tempfile
import shutil
from pathlib import Path


def normalize_string(s):
    """Normalize string by converting to lowercase and removing all spaces."""
    return s.lower().replace(" ", "").replace("\t", "").replace("\n", "")


def contains_normalized(haystack, needle):
    """Check if needle is a substring of haystack after normalization."""
    return normalize_string(needle) in normalize_string(haystack)


@pytest.fixture(scope="session")
def compiled_program():
    """Compile the C++ program once for all tests."""
    # Get the directory where the test file is located
    test_dir = Path(__file__).parent

    # Find all .cpp files in src directory
    src_dir = test_dir / "src"
    cpp_files = list(src_dir.glob("*.cpp"))

    if not cpp_files:
        pytest.fail("No C++ source files found in src/ directory")

    # Compile all C++ files directly with g++
    compile_cmd = [
        "g++",
        "-std=c++17",
        "-Wall",
        "-Wextra",
        f"-I{test_dir / 'include'}",
        "-o", str(test_dir / "war_game")
    ] + [str(f) for f in cpp_files]

    result = subprocess.run(compile_cmd, capture_output=True, text=True, cwd=test_dir)

    if result.returncode != 0:
        pytest.fail(f"Compilation failed:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}")

    executable = test_dir / "war_game"
    if not executable.exists():
        pytest.fail(f"Executable 'war_game' was not created")

    yield executable

    # Cleanup after all tests
    if executable.exists():
        executable.unlink()


@pytest.fixture
def temp_output_file():
    """Create a temporary output file for each test."""
    fd, path = tempfile.mkstemp(suffix=".csv")
    os.close(fd)
    yield path
    # Cleanup
    if os.path.exists(path):
        os.remove(path)


class TestCompilation:
    """Test that the code compiles successfully."""

    def test_compilation_succeeds(self, compiled_program):
        """Verify that the program compiles without errors."""
        assert compiled_program.exists()
        assert os.access(compiled_program, os.X_OK)


class TestCommandLineArguments:
    """Test command line argument handling."""

    def test_no_arguments(self, compiled_program):
        """Test running with no arguments."""
        result = subprocess.run([compiled_program], capture_output=True, text=True)
        assert result.returncode == 1
        assert contains_normalized(result.stderr, "Usage") or contains_normalized(result.stderr, "usage")
        assert contains_normalized(result.stderr, "war_game")

    def test_one_argument(self, compiled_program):
        """Test running with only one argument."""
        result = subprocess.run([compiled_program, "input.csv"], capture_output=True, text=True)
        assert result.returncode == 1
        assert contains_normalized(result.stderr, "Usage") or contains_normalized(result.stderr, "usage")

    def test_too_many_arguments(self, compiled_program):
        """Test running with too many arguments."""
        result = subprocess.run(
            [compiled_program, "input.csv", "output.csv", "extra.csv"],
            capture_output=True, text=True
        )
        assert result.returncode == 1
        assert contains_normalized(result.stderr, "Usage") or contains_normalized(result.stderr, "usage")


class TestFileHandling:
    """Test file I/O handling."""

    def test_missing_input_file(self, compiled_program, temp_output_file):
        """Test with non-existent input file."""
        result = subprocess.run(
            [compiled_program, "nonexistent.csv", temp_output_file],
            capture_output=True, text=True
        )
        assert result.returncode == 1
        assert contains_normalized(result.stderr, "Error") or contains_normalized(result.stderr, "error")

    def test_empty_input_file(self, compiled_program, temp_output_file):
        """Test with empty input file."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "empty.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )
        assert result.returncode == 1
        assert contains_normalized(result.stderr, "Error") or contains_normalized(result.stderr, "error")

    def test_malformed_csv(self, compiled_program, temp_output_file):
        """Test with malformed CSV file."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "malformed.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )
        assert result.returncode == 1
        assert contains_normalized(result.stderr, "Error") or contains_normalized(result.stderr, "error")


class TestSimpleGame:
    """Test a simple game with basic cards."""

    def test_simple_game_runs(self, compiled_program, temp_output_file):
        """Test that a simple game runs successfully."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "simple_deck.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Check for key output messages
        assert contains_normalized(result.stdout, "Starting War") or contains_normalized(result.stdout, "StartingWar")
        assert contains_normalized(result.stdout, "Game Over") or contains_normalized(result.stdout, "GameOver")
        assert contains_normalized(result.stdout, "Round 1") or contains_normalized(result.stdout, "Round1")

        # Check that at least one player plays cards
        assert contains_normalized(result.stdout, "Player A plays") or contains_normalized(result.stdout, "PlayerAplays")
        assert contains_normalized(result.stdout, "Player B plays") or contains_normalized(result.stdout, "PlayerBplays")

    def test_simple_game_has_winner(self, compiled_program, temp_output_file):
        """Test that the game declares a winner."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "simple_deck.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Check that there's a winner or tie
        output_normalized = normalize_string(result.stdout)
        assert ("playera" in output_normalized and "wins" in output_normalized) or \
               ("playerb" in output_normalized and "wins" in output_normalized) or \
               ("tie" in output_normalized)

    def test_output_csv_created(self, compiled_program, temp_output_file):
        """Test that the output CSV file is created."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "simple_deck.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0
        assert os.path.exists(temp_output_file)
        assert os.path.getsize(temp_output_file) > 0

    def test_output_csv_format(self, compiled_program, temp_output_file):
        """Test that the output CSV has correct format."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "simple_deck.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Read and verify CSV structure
        with open(temp_output_file, 'r') as f:
            reader = csv.reader(f)
            header = next(reader)

            # Check header exists and has required columns
            assert len(header) == 5
            header_normalized = [normalize_string(h) for h in header]
            assert "round" in header_normalized
            assert any("playera" in h and "count" in h for h in header_normalized)
            assert any("playerb" in h and "count" in h for h in header_normalized)

            # Check that there are data rows
            rows = list(reader)
            assert len(rows) > 0

            # Check each row has 5 columns
            for row in rows:
                assert len(row) == 5


class TestFaceCards:
    """Test games with face cards."""

    def test_face_cards_game(self, compiled_program, temp_output_file):
        """Test game with face cards (Jack, Queen, King)."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "face_cards.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0
        assert contains_normalized(result.stdout, "Starting War")
        assert contains_normalized(result.stdout, "Game Over")

    def test_face_cards_display(self, compiled_program, temp_output_file):
        """Test that face cards are displayed properly."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "face_cards.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Check that face cards appear in output (case insensitive, normalized)
        output_normalized = normalize_string(result.stdout)
        # Should see at least one face card (Jack, Queen, or King)
        has_face_card = "jack" in output_normalized or \
                       "queen" in output_normalized or \
                       "king" in output_normalized or \
                       "j" in output_normalized or \
                       "q" in output_normalized or \
                       "k" in output_normalized
        assert has_face_card


class TestJokers:
    """Test games with joker cards."""

    def test_jokers_game(self, compiled_program, temp_output_file):
        """Test game with joker cards."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "with_jokers.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0
        assert contains_normalized(result.stdout, "Starting War")
        assert contains_normalized(result.stdout, "Game Over")

    def test_jokers_display(self, compiled_program, temp_output_file):
        """Test that jokers are displayed properly."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "with_jokers.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Check that jokers appear in output
        output_normalized = normalize_string(result.stdout)
        assert "joker" in output_normalized


class TestGameLogic:
    """Test game logic and rules."""

    def test_game_progresses_through_rounds(self, compiled_program, temp_output_file):
        """Test that the game progresses through multiple rounds."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "simple_deck.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Check for multiple rounds
        output = result.stdout
        round_count = 0
        for i in range(1, 100):  # Check up to round 100
            if contains_normalized(output, f"Round {i}") or contains_normalized(output, f"Round{i}"):
                round_count += 1
            else:
                break

        assert round_count >= 1

    def test_csv_rounds_match_output(self, compiled_program, temp_output_file):
        """Test that CSV rounds match console output."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "simple_deck.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Count rounds in CSV
        with open(temp_output_file, 'r') as f:
            reader = csv.reader(f)
            next(reader)  # Skip header
            csv_rounds = len(list(reader))

        # Count rounds in output
        output = result.stdout
        console_rounds = 0
        for i in range(1, 100):
            if contains_normalized(output, f"Round {i}") or contains_normalized(output, f"Round{i}"):
                console_rounds += 1
            else:
                break

        # They should match (or be close)
        assert csv_rounds == console_rounds or abs(csv_rounds - console_rounds) <= 1

    def test_deck_splitting(self, compiled_program, temp_output_file):
        """Test that deck is split evenly between players."""
        test_dir = Path(compiled_program).parent
        input_file = test_dir / "test_data" / "simple_deck.csv"

        result = subprocess.run(
            [compiled_program, input_file, temp_output_file],
            capture_output=True, text=True
        )

        assert result.returncode == 0

        # Read first round from CSV to check that total cards are preserved
        with open(temp_output_file, 'r') as f:
            reader = csv.reader(f)
            next(reader)  # Skip header
            first_round = next(reader)

            # Columns: Round, PlayerA_Count, PlayerB_Count, PlayerA_Cards, PlayerB_Cards
            player_a_count = int(first_round[1])
            player_b_count = int(first_round[2])

            # With 6 cards total, after round 1, total should still be 6
            total_cards = player_a_count + player_b_count
            assert total_cards == 6


class TestCardValues:
    """Test that card values are correct."""

    def test_ace_is_low(self, compiled_program, temp_output_file):
        """Test that Ace (value 1) is lower than other cards."""
        # Create a test deck where ace should lose
        test_dir = Path(compiled_program).parent
        temp_input = test_dir / "test_data" / "test_ace.csv"

        with open(temp_input, 'w') as f:
            f.write("Hearts,1\n")  # Ace
            f.write("Spades,2\n")  # Two

        try:
            result = subprocess.run(
                [compiled_program, temp_input, temp_output_file],
                capture_output=True, text=True
            )

            assert result.returncode == 0

            # Player B (who has the 2) should win
            output_normalized = normalize_string(result.stdout)
            assert ("playerb" in output_normalized and "wins" in output_normalized)
        finally:
            if temp_input.exists():
                temp_input.unlink()

    def test_joker_is_highest(self, compiled_program, temp_output_file):
        """Test that Joker (value 14) beats King."""
        test_dir = Path(compiled_program).parent
        temp_input = test_dir / "test_data" / "test_joker.csv"

        with open(temp_input, 'w') as f:
            f.write("Hearts,13\n")  # King
            f.write("Joker,Red\n")  # Joker

        try:
            result = subprocess.run(
                [compiled_program, temp_input, temp_output_file],
                capture_output=True, text=True
            )

            assert result.returncode == 0

            # Player B (who has the Joker) should win
            output_normalized = normalize_string(result.stdout)
            assert ("playerb" in output_normalized and "wins" in output_normalized)
        finally:
            if temp_input.exists():
                temp_input.unlink()


class TestEdgeCases:
    """Test edge cases and boundary conditions."""

    def test_two_card_deck(self, compiled_program, temp_output_file):
        """Test with minimum viable deck (2 cards)."""
        test_dir = Path(compiled_program).parent
        temp_input = test_dir / "test_data" / "test_two_cards.csv"

        with open(temp_input, 'w') as f:
            f.write("Hearts,5\n")
            f.write("Spades,3\n")

        try:
            result = subprocess.run(
                [compiled_program, temp_input, temp_output_file],
                capture_output=True, text=True
            )

            assert result.returncode == 0
            assert contains_normalized(result.stdout, "Game Over")
        finally:
            if temp_input.exists():
                temp_input.unlink()

    def test_large_deck(self, compiled_program, temp_output_file):
        """Test with a larger deck."""
        test_dir = Path(compiled_program).parent
        temp_input = test_dir / "test_data" / "test_large_deck.csv"

        with open(temp_input, 'w') as f:
            for suit in ["Hearts", "Diamonds", "Clubs", "Spades"]:
                for rank in range(1, 14):
                    f.write(f"{suit},{rank}\n")

        try:
            result = subprocess.run(
                [compiled_program, temp_input, temp_output_file],
                capture_output=True, text=True,
                timeout=30  # Prevent infinite loops
            )

            assert result.returncode == 0
            assert contains_normalized(result.stdout, "Starting War")
            assert contains_normalized(result.stdout, "Game Over")
        finally:
            if temp_input.exists():
                temp_input.unlink()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
