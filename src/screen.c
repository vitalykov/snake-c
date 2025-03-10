#include "screen.h"

#include "terminal.h"

#define EMPTY_CELL "  "

void UpdateScreen(int fd, Food* food, Snake* snake) {
    static char* frame[HEIGHT][WIDTH];

    for (int row = 0; row < HEIGHT; ++row) {
        for (int col = 0; col < WIDTH; col += 2) {
            frame[row][col] = EMPTY_CELL;
        }
    }

    for (int i = snake->length - 1; i >= 0; --i) {
        SnakeSegment segment = snake->segments[i];
        frame[segment.pos.y][segment.pos.x] = segment.symbol;
    }

    frame[food->pos.y][food->pos.x] = food->symbol;

    DrawFrame(fd, frame);
}

void DrawFrame(int fd, char* frame[HEIGHT][WIDTH]) {
    for (int row = 0; row < HEIGHT; ++row) {
        for (int col = 0; col < WIDTH; col += 2) {
            MovePrint(fd, row, col, frame[row][col]);
        }
    }
}