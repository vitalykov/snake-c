#ifndef TERMINAL_H
#define TERMINAL_H

#include <poll.h>

#define STDIN_FD 0
#define STDOUT_FD 1
#define STDERR_FD 2

#define ESC 27

#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"

void EnableNonBlockingMode(int fd);
void InitializeScreen(int fd);
int ProcessInput(char *input, struct pollfd* readfds, int timeout);
void AlternateScreenOn(int fd);
void AlternateScreenOff(int fd);
void RestoreTerminal(int fd);
void ShowCursor(int fd);
void HideCursor(int fd);
void ClearTerminal(int fd);
void ResizeTerminal(int fd, int height, int width);
void MoveCursor(int fd, int line, int column);
void SetCursorStyle(int fd, int style, int is_blinking);
void MovePrint(int fd, int line, int column, char* message);

#endif // TERMINAL_H