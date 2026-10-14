CC ?= cc
CPPFLAGS := -Iinclude
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
LDFLAGS :=
SANITIZE_CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer
SANITIZE_LDFLAGS := -fsanitize=address,undefined
BUILD_DIR := build
CORE := src/cpu.c
TEST_BIN := $(BUILD_DIR)/test_cpu
SANITIZE_BIN := $(BUILD_DIR)/test_cpu_sanitize
ASM_BIN := $(BUILD_DIR)/charisasm
DEBUG_BIN := $(BUILD_DIR)/charisvm-debug

.PHONY: all test test-assembler sanitize clean

all: $(BUILD_DIR)/charisvm $(ASM_BIN) $(DEBUG_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ASM_BIN): src/assembler.c include/cpu.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) src/assembler.c $(LDFLAGS) -o $@

$(DEBUG_BIN): $(CORE) src/debugger.c include/cpu.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CORE) src/debugger.c $(LDFLAGS) -o $@

$(TEST_BIN): $(CORE) tests/test_cpu.c include/cpu.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CORE) tests/test_cpu.c $(LDFLAGS) -o $@

$(BUILD_DIR)/charisvm: $(CORE) src/main.c include/cpu.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CORE) src/main.c $(LDFLAGS) -o $@

$(SANITIZE_BIN): $(CORE) tests/test_cpu.c include/cpu.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(SANITIZE_CFLAGS) $(CORE) tests/test_cpu.c $(SANITIZE_LDFLAGS) -o $@

test: $(TEST_BIN) $(ASM_BIN) $(BUILD_DIR)/charisvm
	./$(TEST_BIN)
	sh tests/test_assembler.sh

test-assembler: $(ASM_BIN) $(BUILD_DIR)/charisvm
	sh tests/test_assembler.sh

sanitize: $(SANITIZE_BIN)
	./$(SANITIZE_BIN)

clean:
	rm -rf $(BUILD_DIR)
