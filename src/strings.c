#include "strings.h"

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
        num = -num;
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