#ifndef TERMINAL_H
#define TERMINAL_H

void EnableNonBlockingMode(int fd);
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