# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra

# Target executable
TARGET = solution

# Source file
SRC = solution.cpp

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run tests using pytest
test: $(TARGET)
	pytest test_game.py

# Clean compiled files
clean:
	rm -f $(TARGET) *.o game_output.csv