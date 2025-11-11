CC = gcc
CFLAGS_DEBUG = -Wall -Wextra -g
CFLAGS_RELEASE = -O2
BUILD_DIR = build

SRC_INI = ./ini.c
OBJ_INI = $(BUILD_DIR)/ini.o

SRC_TESTS = tests/rktest.c tests/ini_tests.c
TEST_BIN = $(BUILD_DIR)/tests

SRC_MAIN = example/main.c
EXAMPLE_BIN = $(BUILD_DIR)/example

.PHONY: all ini tests example clean debug release

all: ini tests example

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

ini: $(OBJ_INI)

$(OBJ_INI): $(SRC_INI) ini.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(OBJ_INI) $(SRC_TESTS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CFLAGS_TESTS) $(SRC_TESTS) $(OBJ_INI) -lm -o $(TEST_BIN)

example: $(OBJ_INI) $(SRC_MAIN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_MAIN) $(OBJ_INI) -o $(EXAMPLE_BIN)

debug: CFLAGS=$(CFLAGS_DEBUG)
debug: all

release: CFLAGS=$(CFLAGS_RELEASE)
release: all

clean:
	rm -rf $(BUILD_DIR)
