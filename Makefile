CC ?= cc
CPPFLAGS := -Iinclude
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
LDFLAGS :=
BUILD_DIR := build
CORE := src/cpu.c
TEST_BIN := $(BUILD_DIR)/test_cpu

.PHONY: all test sanitize clean

all: $(BUILD_DIR)/charisvm

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_BIN): $(CORE) tests/test_cpu.c include/cpu.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CORE) tests/test_cpu.c $(LDFLAGS) -o $@

$(BUILD_DIR)/charisvm: $(CORE) src/main.c include/cpu.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CORE) src/main.c $(LDFLAGS) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

sanitize: CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: LDFLAGS := -fsanitize=address,undefined
sanitize: clean test

clean:
	rm -rf $(BUILD_DIR)