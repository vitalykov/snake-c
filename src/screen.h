#ifndef SNAKE_C_SCREEN_H
#define SNAKE_C_SCREEN_H

#include "food.h"
#include "snake.h"

#define WIDTH 80
#define HEIGHT 20

void UpdateScreen(int fd, Food* food, Snake* snake);
void DrawFrame(int fd, char* frame[HEIGHT][WIDTH]);

#endif  // SNAKE_C_SCREEN_H
