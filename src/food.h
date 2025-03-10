#ifndef FOOD_H
#define FOOD_H

#include "snake.h"
#include "vector2d.h"

#define FOOD_APPLE "🍎"

struct Food {
    Vector pos;
    char *symbol;
};

typedef struct Food Food;

Food GenerateFood(Snake *snake);

#endif // FOOD_H