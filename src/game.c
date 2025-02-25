#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>

#define STDIN_FD 0
#define STDOUT_FD 1
#define STDERR_FD 2

#define ICANON	0x00002
#define ECHO	0x00008

#define FPS 20

#define WIDTH 80
#define HEIGHT 20

#define ESC 27
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

#define MAX_SNAKE_LENGTH 2000
#define EMPTY_CELL "  "
#define SPACE ' '

#define FOOD_APPLE "🍎"

#define YELLOW_CIRCLE "🟡"

#define FACE_WITH_TONGUE "😛"
#define SKULL "💀"

struct Vector {
    int x;
    int y;
};

typedef struct Vector Vector;

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

struct Food {
	Vector pos;
	char* symbol;
};

typedef struct Food Food;

#define CC_COUNT 19
struct InputOutputControlParameters {
    unsigned int c_iflag;		/* input mode flags */
    unsigned int c_oflag;		/* output mode flags */
    unsigned int c_cflag;		/* control mode flags */
    unsigned int c_lflag;		/* local mode flags */
    unsigned char c_line;			/* line discipline */
    unsigned char c_cc[CC_COUNT];		/* control characters */
};

typedef struct InputOutputControlParameters InputOutputControlParameters;

void InitializeScreen(int fd);
int ProcessInput(char *input, struct pollfd* readfds, int timeout);
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
Vector GetTerminalPos(int x, int y);
void CleanInputBuffer(int fd);
unsigned int RandomUnsignedInt(void);
int Max(int arr[], int n);
int Mod(int a, int b);
int Abs(int a);
int EqualVectors(Vector v1, Vector v2);
int Length(const char* str);
void AppendChar(char* destination, char c);
void AppendString(char* destination, const char* str);
void AppendInt(char* destination, int num);
Food GenerateFood(Snake* snake);
void InitSnake(Snake* snake);
void AppendSnakeSegment(Snake* snake);
void UpdateSnakeSpeed(Snake* snake, char input);
void MoveSnake(Snake* snake);
int IsOccupiedBySnake(int x, int y, Snake* snake);
int SnakeEatYouself(Snake* snake);
void UpdateScreen(int fd, Food* food, Snake* snake);
void DrawFrame(int fd, char* frame[HEIGHT][WIDTH]);

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

void EnableNonBlockingMode(int fd) {
	InputOutputControlParameters ioctl_params;
	ioctl(fd, TCGETS, &ioctl_params);
	ioctl_params.c_lflag &= ~(ICANON | ECHO);
	ioctl(fd, TCSETS, &ioctl_params);
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

void CleanInputBuffer(int fd) {
	char c;
	while (read(fd, &c, 1) > 0);
}

unsigned int RandomUnsignedInt(void) {
	unsigned int result;
	getentropy(&result, sizeof(result));

	return result;
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
	Vector TermPos = GetTerminalPos(column, line);
	char sequence[30];
	sequence[0] = '\0';
	AppendString(sequence, CSI);
	AppendInt(sequence, TermPos.y);
	AppendChar(sequence, ';');
	AppendInt(sequence, TermPos.x);
	AppendChar(sequence, 'H');
	write(fd, sequence, Length(sequence));
}

void MovePrint(int fd, int line, int column, char* message) {
	MoveCursor(fd, line, column);
	write(fd, message, Length(message));
}

Vector GetTerminalPos(int x, int y) {
	Vector pos;
	pos.y = Mod(y, HEIGHT) + 1;
	pos.x = Mod(x, WIDTH) + 1;

    return pos;
}

int Mod(int a, int b) { return ((a % b) + b) % b; }

int Abs(int a) { return a > 0 ? a : -a; }

int EqualVectors(Vector v1, Vector v2) { return v1.x == v2.x && v1.y == v2.y; }

int Length(const char* str) {
	int length = 0;
	
	while (str[length] != '\0') {
		++length;
	}

	return length;
}

void AppendChar(char* destination, char c) {
	int length = Length(destination);
	destination[length] = c;
	destination[length + 1] = '\0';
}

void AppendString(char* destination, const char* str) {
	int dest_len = Length(destination);
	int str_len = Length(str);

	for (int i = 0; i < str_len; ++i) {
		destination[dest_len + i] = str[i];
	}

	destination[dest_len + str_len] = '\0';
}

void AppendInt(char* destination, int num) {	
	if (num == 0) {
		AppendChar(destination, '0');
		return;
	}
	
	char num_str[12];
	int i = 0;

	if (num < 0) {
		num_str[i] = '-';
		++i;
		num -= num;
	}

	int l = i;

	while (num != 0) {
		int digit = num % 10;
		num_str[i] = '0' + digit;
		++i;

		num /= 10;
	}

	num_str[i] = '\0';
	int r = i - 1;

	while (l < r) {
		char temp = num_str[l];
		num_str[l] = num_str[r];
		num_str[r] = temp;

		++l, --r;
	}

	AppendString(destination, num_str);
}

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
			MoveCursor(fd, row, col);
			write(fd, frame[row][col], Length(frame[row][col]));
		}
	}
}
