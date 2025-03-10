#include "snake.h"

#include "screen.h"
#include "util.h"

void InitSnake(Snake* snake) {
    Vector pos = {.x = WIDTH / 2, .y = HEIGHT / 2};
    Vector vel = {.x = 2, .y = 0};
    char* symbol = FACE_WITH_TONGUE;

    SnakeSegment segment = {.pos = pos, .vel = vel, .symbol = symbol};

    snake->segments[0] = segment;
    snake->length = 1;
}

void AppendSnakeSegment(Snake* snake) {
    SnakeSegment last_segment = snake->segments[snake->length - 1];
    
    SnakeSegment segment;
    segment.pos.x = Mod(last_segment.pos.x - last_segment.vel.x, WIDTH);
    segment.pos.y = Mod(last_segment.pos.y - last_segment.vel.y, HEIGHT);
    segment.vel = last_segment.vel;
    
    if (snake->length == 1) {
        segment.symbol = YELLOW_CIRCLE;
    } else {
        segment.symbol = last_segment.symbol;
    }

    snake->segments[snake->length] = segment;
    snake->length += 1;
}

void UpdateSnakeSpeed(Snake* snake, char input) {
    SnakeSegment* head = snake->segments;
    
    // TODO. Make possible input with CAPS LOCK and maybe with russian layout

    if (input == 'w') {
        if (head->vel.x != 0) {
            head->vel.y = -Abs(head->vel.x) / 2;
            head->vel.x = 0;
        }
    } else if (input == 's') {
        if (head->vel.x != 0) {
            head->vel.y = Abs(head->vel.x) / 2;
            head->vel.x = 0;
        }
    } else if (input == 'a') {
        if (head->vel.y != 0) {
            head->vel.x = -Abs(head->vel.y) * 2;
            head->vel.y = 0;
        }
    } else if (input == 'd') {
        if (head->vel.y != 0) {
            head->vel.x = Abs(head->vel.y) * 2;
            head->vel.y = 0;
        }
    }
}

void MoveSnake(Snake* snake) {
    for (int i = snake->length - 1; i > 0; --i) {
        SnakeSegment* segment = snake->segments + i;
        segment->pos.x = Mod(segment->pos.x + segment->vel.x, WIDTH);
        segment->pos.y = Mod(segment->pos.y + segment->vel.y, HEIGHT);

        SnakeSegment* prev_segment = segment - 1;
        segment->vel.x = prev_segment->vel.x;
        segment->vel.y = prev_segment->vel.y;
    }

    SnakeSegment* head = snake->segments;
    head->pos.x = Mod(head->pos.x + head->vel.x, WIDTH);
    head->pos.y = Mod(head->pos.y + head->vel.y, HEIGHT);
}

int IsOccupiedBySnake(int x, int y, Snake* snake) {
    for (int i = 0; i < snake->length; ++i) {
        SnakeSegment segment = snake->segments[i];

        if (x == segment.pos.x && y == segment.pos.y) {
            return 1;
        }
    }

    return 0;
}

int SnakeEatYouself(Snake* snake) {
    SnakeSegment head = snake->segments[0];

    for (int i = 1; i < snake->length; ++i) {
        SnakeSegment segment = snake->segments[i];

        if (EqualVectors(segment.pos, head.pos)) {
            return 1;
        }
    }

    return 0;
}
