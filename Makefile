CC = gcc
CC_FLAGS = -Wall -Werror -Wextra
VPATH = src
sources = game.c
BUILD_PATH = build
EXECUTABLE = $(BUILD_PATH)/snake

.PHONY := all
all: snake

.PHONY += snake
snake: $(EXECUTABLE)

$(EXECUTABLE): $(sources)
	$(CC) $(CC_FLAGS) $^ -o $@

.PHONY += clean
clean:
	rm -rf *.o $(EXECUTABLE)

.PHONY += rebuild
rebuild: clean snake

.PHONY += run
run: snake
	$(EXECUTABLE)