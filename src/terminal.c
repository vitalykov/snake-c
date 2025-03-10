#include "terminal.h"

#include <unistd.h>
#include <sys/ioctl.h>

#include "strings.h"
#include "screen.h"

#define ICANON  0x00002
#define ECHO    0x00008

#define CSI "\x1b["

#define ALTERNATE_SCREEN_BUFFER_ON (CSI "?1049h")
#define ALTERNATE_SCREEN_BUFFER_OFF (CSI "?1049l")

#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"

#define CURSOR_BLOCK 2
#define CURSOR_UNDERLINE 4
#define CURSOR_BAR 6

#define CURSOR_BLINKING 1
#define CURSOR_STEADY 0

#define CC_COUNT 19
struct InputOutputControlParameters {
    unsigned int c_iflag;         /* input mode flags */
    unsigned int c_oflag;         /* output mode flags */
    unsigned int c_cflag;         /* control mode flags */
    unsigned int c_lflag;         /* local mode flags */
    unsigned char c_line;         /* line discipline */
    unsigned char c_cc[CC_COUNT]; /* control characters */
};

typedef struct InputOutputControlParameters InputOutputControlParameters;

void EnableNonBlockingMode(int fd) {
    InputOutputControlParameters ioctl_params;
    ioctl(fd, TCGETS, &ioctl_params);
    ioctl_params.c_lflag &= ~(ICANON | ECHO);
    ioctl(fd, TCSETS, &ioctl_params);
}

void InitializeScreen(int fd) {
    EnableNonBlockingMode(fd);
    AlternateScreenOn(fd);
    ResizeTerminal(fd, HEIGHT, WIDTH);
    HideCursor(fd);
    MovePrint(fd, HEIGHT / 2, WIDTH / 2 - 5, "Initializing...");

    sleep(1);

    ClearTerminal(fd);
}

int ProcessInput(char *input, struct pollfd* readfds, int timeout) {
    int ready = poll(readfds, 1, timeout);

    if (ready == -1) {
        MovePrint(STDERR_FD, 0, 0, "poll error");
        return 0;
    }

    if (readfds[0].revents & POLLIN) {
        int bytes_read = read(readfds[0].fd, input, 1);

        if (bytes_read <= 0) {
            MovePrint(STDERR_FD, 0, 0, "read error");
        }
    }

    return 1;
}

void AlternateScreenOn(int fd) {
    char sequence[] = ALTERNATE_SCREEN_BUFFER_ON;
    write(fd, sequence, Length(sequence));
}

void AlternateScreenOff(int fd) {
    char sequence[] = ALTERNATE_SCREEN_BUFFER_OFF;
    write(fd, sequence, Length(sequence));
}

void RestoreTerminal(int fd) {
    InputOutputControlParameters ioctl_params;
    ioctl(fd, TCGETS, &ioctl_params);
    ioctl_params.c_lflag |= (ICANON | ECHO);
    ioctl(fd, TCSETS, &ioctl_params);

    AlternateScreenOff(fd);
    ShowCursor(fd);
    SetCursorStyle(fd, CURSOR_BLOCK, CURSOR_BLINKING);
}

void ShowCursor(int fd) {
    char sequence[] = CSI "?25h";
    write(fd, sequence, Length(sequence));
}

void HideCursor(int fd) {
    char sequence[] = CSI "?25l";
    write(fd, sequence, Length(sequence));
}

void SetCursorStyle(int fd, int style, int is_blinking) {
    char sequence[30];
    sequence[0] = '\0';
    AppendString(sequence, CSI);
    int parameter = style - is_blinking;
    AppendInt(sequence, parameter);
    AppendString(sequence, " q");
    write(fd, sequence, Length(sequence));
}

void ClearTerminal(int fd) {
    char sequence[] = CSI "H" CSI "J";
    write(fd, sequence, Length(sequence));
}

void ResizeTerminal(int fd, int height, int width) {
    char sequence[30];
    sequence[0] = '\0';
    AppendString(sequence, CSI);
    AppendString(sequence, "8;");
    AppendInt(sequence, height);
    AppendChar(sequence, ';');
    AppendInt(sequence, width);
    AppendChar(sequence, 't');
    write(fd, sequence, Length(sequence));
}

void MoveCursor(int fd, int line, int column) {
    char sequence[30];
    sequence[0] = '\0';
    AppendString(sequence, CSI);
    AppendInt(sequence, line + 1);
    AppendChar(sequence, ';');
    AppendInt(sequence, column + 1);
    AppendChar(sequence, 'H');
    write(fd, sequence, Length(sequence));
}

void MovePrint(int fd, int line, int column, char* message) {
    MoveCursor(fd, line, column);
    write(fd, message, Length(message));
}
