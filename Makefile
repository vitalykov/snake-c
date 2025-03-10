CC := gcc
CFLAGS := -Wall -Werror -Wextra
VPATH := src
SRC := $(wildcard src/*.c)
OBJ := $(subst src/,build/,$(SRC:.c=.o))
BUILD_PATH := build
EXECUTABLE := $(BUILD_PATH)/snake

.PHONY: all
all: snake

$(BUILD_PATH)/game.o: game.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_PATH)/%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: snake
snake: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_PATH)/*.o $(EXECUTABLE)

.PHONY: rebuild
rebuild: clean snake

.PHONY: run
run: snake
	$(EXECUTABLE)