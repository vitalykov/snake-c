#ifndef SNAKE_H
#define SNAKE_H

#include "vector2d.h"

#define MAX_SNAKE_LENGTH 2000

#define YELLOW_CIRCLE "🟡"

#define FACE_WITH_TONGUE "😛"
#define SKULL "💀"

struct SnakeSegment {
    Vector vel;
    Vector pos;
    char* symbol;
};

typedef struct SnakeSegment SnakeSegment;

struct Snake {
    int length;
    SnakeSegment segments[MAX_SNAKE_LENGTH];
};

typedef struct Snake Snake;

void InitSnake(Snake* snake);
void AppendSnakeSegment(Snake* snake);
void UpdateSnakeSpeed(Snake* snake, char input);
void MoveSnake(Snake* snake);
int IsOccupiedBySnake(int x, int y, Snake* snake);
int SnakeEatYouself(Snake* snake);

#endif  // SNAKE_H