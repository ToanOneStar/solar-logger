# Makefile for Solar Logger Project

# ============================================================================
# Variables
# ============================================================================
CC := gcc
CFLAGS := -Wall -Wextra -O2 -std=c99
DEBUG_FLAGS := -g -O0 -DDEBUG
LDFLAGS := -lm -lcjson

# Directories
SRC_DIR := src
INC_DIR := include
CONFIG_DIR := config
BUILD_DIR := build
BIN_DIR := bin

# Source files
SOURCES := main.c \
           $(SRC_DIR)/json_parser.c \
           $(SRC_DIR)/device_manager.c \
           $(SRC_DIR)/error_handler.c \
           $(SRC_DIR)/utils.c \
		   $(SRC_DIR)/cJSON.c

# Test source files
TEST_SOURCES := $(SRC_DIR)/test_json_parser.c \
                $(SRC_DIR)/json_parser.c \
                $(SRC_DIR)/device_manager.c \
                $(SRC_DIR)/utils.c \
                $(SRC_DIR)/cJSON.c

# Object files
OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Test objects
TEST_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

# Output executable
TARGET := $(BIN_DIR)/solar-logger
TEST_TARGET := $(BIN_DIR)/test-json-parser

# ============================================================================
# Phony Targets
# ============================================================================
.PHONY: all build debug clean run help install-deps test

# ============================================================================
# Default Target
# ============================================================================
all: build

# ============================================================================
# Build Targets
# ============================================================================
build: $(TARGET)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $@..."
	@$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# ============================================================================
# Run Target
# ============================================================================
run: build
	@echo "Running application..."
	@$(TARGET)

# ============================================================================
# Test Target
# ============================================================================
test: $(TEST_TARGET)
	@echo "Running test..."
	@$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking test executable..."
	@$(CC) $(TEST_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Test build complete: $@"

# ============================================================================
# Utility Targets
# ============================================================================
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete"

install-deps:
	@echo "Installing dependencies..."
	@apt-get update
	@apt-get install -y build-essential
	@echo "Dependencies installed"

help:
	@echo "Available targets:"
	@echo "  make build         - Build the project (default)"
	@echo "  make debug         - Build with debugging symbols"
	@echo "  make run           - Build and run the application"
	@echo "  make test          - Build and run JSON parser test"
	@echo "  make clean         - Remove build artifacts"
	@echo "  make install-deps  - Install build dependencies"
	@echo "  make help          - Show this help message"
