#include "food.h"

#include "util.h"

Food GenerateFood(Snake* snake) {
    int x = (RandomUnsignedInt() << 1) % WIDTH;
    int y = RandomUnsignedInt() % HEIGHT;

    while (IsOccupiedBySnake(x, y, snake)) {
        x = (RandomUnsignedInt() << 1) % WIDTH;
        y = RandomUnsignedInt() % HEIGHT;
    }

    char* symbol = FOOD_APPLE;

    Vector pos = {.x = x, .y = y};
    Food food = {.pos = pos, .symbol = symbol};

    return food;
}
