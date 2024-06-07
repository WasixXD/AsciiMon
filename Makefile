CC = gcc
CFLAGS = -Wall -Wextra -Iinclude 

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))
EXEC = $(BIN_DIR)/main

HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
SRCS = $(wildcard $(SRC_DIR)/*.c)

.PHONY: all 

all: $(EXEC) run_script

$(EXEC): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lncurses

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run_script: $(EXEC)
	@./script.sh