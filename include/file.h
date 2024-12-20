#ifndef FILE_H
#define FILE_H
#include <stdio.h>

void appendBitsToFile(char *bits, FILE *file);

int readBit(FILE *file);

void get_line(FILE *file, char *buffer);

#endif
