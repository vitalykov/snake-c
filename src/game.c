#include <unistd.h>

#include "food.h"
#include "screen.h"
#include "snake.h"
#include "strings.h"
#include "terminal.h"
#include "util.h"
#include "vector2d.h"

#define FPS 20

int main(void) {
    Snake snake;
    InitSnake(&snake);
    Food food = GenerateFood(&snake);

    InitializeScreen(STDOUT_FD);

    struct pollfd readfds[1];
    readfds[0].fd = STDIN_FD;
    readfds[0].events = POLLIN;

    int timeout = 0;
    char input = 'X';

    UpdateScreen(STDOUT_FD, &food, &snake);

    while (input != ESC) {
        if (!ProcessInput(&input, readfds, timeout)) {
            break;
        }

        UpdateSnakeSpeed(&snake, input);
        MoveSnake(&snake);

        if (SnakeEatYouself(&snake)) {
            snake.segments[0].symbol = SKULL;
            UpdateScreen(STDOUT_FD, &food, &snake);
            MovePrint(STDIN_FD, HEIGHT / 2, WIDTH / 2 - 3, COLOR_RED "Dead" COLOR_RESET);

            sleep(2);
            break;
        }

        if (EqualVectors(snake.segments[0].pos, food.pos)) {
            food = GenerateFood(&snake);
            AppendSnakeSegment(&snake);
        }

        UpdateScreen(STDOUT_FD, &food, &snake);

        usleep(1000000 / FPS);
    }

    RestoreTerminal(STDOUT_FD);

    char statistics[30] = "food eaten: ";
    AppendInt(statistics, snake.length - 1);
    AppendChar(statistics, '\n');
    write(STDOUT_FD, statistics, Length(statistics));
}
