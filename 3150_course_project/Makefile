# BGP Simulator - Makefile
#
# Targets:
#   make                Build the bgp_sim binary in build/.
#   make test           Build and run the GoogleTest suite.
#   make bench-prefix   Run bgp_sim against bench/prefix and diff output.
#   make bench-subprefix
#   make bench-many     Same for bench/many (large dataset).
#   make clean          Remove build/ artifacts.
#   make distclean      Also remove third_party/googletest.
#
# Notes:
#   GoogleTest is auto-fetched into third_party/googletest on first use of
#   `make test`. If you have no internet but do have brew-installed gtest,
#   set GTEST_PREFIX=$(brew --prefix googletest) when invoking make.

CXX      ?= g++
CXXSTD   ?= -std=c++17
WARN     ?= -Wall -Wextra -Wpedantic
OPT      ?= -O2
DEBUG    ?=
CXXFLAGS ?= $(CXXSTD) $(WARN) $(OPT) $(DEBUG)
INCLUDES ?= -Iinclude

BUILD_DIR := build
BIN       := $(BUILD_DIR)/bgp_sim
TEST_BIN  := $(BUILD_DIR)/bgp_tests

LIB_SRCS  := src/bgp.cpp src/rov.cpp src/as_graph.cpp src/simulator.cpp src/csv_io.cpp
LIB_OBJS  := $(LIB_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
MAIN_OBJ  := $(BUILD_DIR)/main.o

TEST_SRCS := $(wildcard tests/*.cpp)
TEST_OBJS := $(TEST_SRCS:tests/%.cpp=$(BUILD_DIR)/test_%.o)

# GoogleTest discovery / fetch ------------------------------------------------
GTEST_DIR    ?= third_party/googletest
GTEST_REPO   ?= https://github.com/google/googletest.git
GTEST_TAG    ?= v1.14.0
GTEST_PREFIX ?=

ifneq ($(strip $(GTEST_PREFIX)),)
    GTEST_INC  := -I$(GTEST_PREFIX)/include
    GTEST_LIBS := -L$(GTEST_PREFIX)/lib -lgtest -lgtest_main -lpthread
    GTEST_SETUP_DEPS :=
else
    GTEST_INC  := -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googletest
    GTEST_LIBS := $(BUILD_DIR)/libgtest.a -lpthread
    GTEST_SETUP_DEPS := $(BUILD_DIR)/libgtest.a
endif

# Default --------------------------------------------------------------------
all: $(BIN)

# Binary ---------------------------------------------------------------------
$(BIN): $(LIB_OBJS) $(MAIN_OBJ) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Tests ----------------------------------------------------------------------
test: $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN): $(LIB_OBJS) $(TEST_OBJS) $(GTEST_SETUP_DEPS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(LIB_OBJS) $(TEST_OBJS) $(GTEST_LIBS)

$(BUILD_DIR)/test_%.o: tests/%.cpp $(GTEST_SETUP_DEPS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GTEST_INC) -c $< -o $@

# Build a static libgtest.a from the vendored sources.
$(BUILD_DIR)/libgtest.a: $(GTEST_DIR)/googletest/src/gtest-all.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR)/googletest -I$(GTEST_DIR)/googletest/include \
	    -c $(GTEST_DIR)/googletest/src/gtest-all.cc -o $(BUILD_DIR)/gtest-all.o
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR)/googletest -I$(GTEST_DIR)/googletest/include \
	    -c $(GTEST_DIR)/googletest/src/gtest_main.cc -o $(BUILD_DIR)/gtest_main.o
	ar rcs $@ $(BUILD_DIR)/gtest-all.o $(BUILD_DIR)/gtest_main.o

$(GTEST_DIR)/googletest/src/gtest-all.cc:
	@if [ ! -d $(GTEST_DIR) ]; then \
	    echo "Cloning googletest $(GTEST_TAG) into $(GTEST_DIR)..."; \
	    git clone --depth 1 --branch $(GTEST_TAG) $(GTEST_REPO) $(GTEST_DIR); \
	fi

# System tests ---------------------------------------------------------------
bench-prefix: $(BIN)
	@mkdir -p $(BUILD_DIR)
	$(BIN) --caida bench/prefix/CAIDAASGraphCollector_2025.10.15.txt \
	       --anns  bench/prefix/anns.csv \
	       --rov   bench/prefix/rov_asns.csv \
	       --output $(BUILD_DIR)/output_prefix.csv
	bench/compare_output.sh bench/prefix/ribs.csv $(BUILD_DIR)/output_prefix.csv

bench-subprefix: $(BIN)
	@mkdir -p $(BUILD_DIR)
	$(BIN) --caida bench/subprefix/CAIDAASGraphCollector_2025.10.15.txt \
	       --anns  bench/subprefix/anns.csv \
	       --rov   bench/subprefix/rov_asns.csv \
	       --output $(BUILD_DIR)/output_subprefix.csv
	bench/compare_output.sh bench/subprefix/ribs.csv $(BUILD_DIR)/output_subprefix.csv

bench-many: $(BIN)
	@mkdir -p $(BUILD_DIR)
	$(BIN) --caida bench/many/CAIDAASGraphCollector_2025.10.15.txt \
	       --anns  bench/many/anns.csv \
	       --rov   bench/many/rov_asns.csv \
	       --output $(BUILD_DIR)/output_many.csv
	bench/compare_output.sh bench/many/ribs.csv $(BUILD_DIR)/output_many.csv

# Clean ----------------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -rf third_party

.PHONY: all test bench-prefix bench-subprefix bench-many clean distclean
