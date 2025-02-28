#ifndef STRINGS_H
#define STRINGS_H

int Length(const char* str);
void AppendChar(char* destination, char c);
void AppendString(char* destination, const char* str);
void AppendInt(char* destination, int num);

#endif // STRINGS_H