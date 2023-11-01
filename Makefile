#!/usr/bin/env -S make -f

# Compiler
CC := gcc

# Compiler flags
CFLAGS := -pedantic -std=c99
CLIBS := -lcurl -ljson-c

# Source files location
SRC_DIR := src
# Object files location
OBJ_DIR := obj

# Source files
SRC := $(wildcard $(SRC_DIR)/*.c)
# Object files (derived from source files)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# Binary name
TARGET := mgpt

.PHONY: all clean debug release

# Default Target
.DEFAULT_GOAL := release
all: $(TARGET)

debug: CFLAGS += -O0 -g -D DEBUG_ASSERTS=true
debug: all

release: CFLAGS += -O3
release: STRIP := -s
release: all

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# Link object files and create binary
$(TARGET): $(OBJ)
	@mkdir -p $(dir $(TARGET))
	$(CC) $(OBJ) -o $(TARGET) $(CLIBS) $(CFLAGS) $(STRIP)
# Clean object files and binary
clean:
	$(RM) -r $(OBJ_DIR) $(TARGET)
#```
#
#To use this Makefile, create a directory structure as follows:
#
#```
#project_folder/
#  |--- src/
#  |     |- file1.c
#  |     |- file2.c
#  |     |- file3.c
#  |     ...
#  |
#  |--- obj/
#  |
#  |--- Makefile
#```
#
#Place your C source files inside the `src/` directory. Then, execute `make` command in the `project_folder/` directory to compile and link all the source files into a binary named `my_binary`. The compiled binary will be created in `project_folder/`.
#
#To clean the generated object files and binary, execute `make clean`.
