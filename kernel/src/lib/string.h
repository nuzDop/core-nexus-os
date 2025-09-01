#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// Compares two strings. Returns 0 if they are equal.
int strcmp(const char* str1, const char* str2);

// Copies a string from src to dest.
char* strcpy(char* dest, const char* src);

// Returns the length of a string.
size_t strlen(const char* str);

#endif
