#ifndef FILE_H
#define FILE_H
#include <stdio.h>

/**
 * @brief append bits to file
 * @param *bits bits to append
 * @param *fiile file to append to
 */
void appendBitsToFile(char *bits, FILE *file);

/**
 * @brief read a bit from file
 * @param *file file to read from
 * @return next bit as 1 or 0
 */
int readBit(FILE *file);

/**
 * @brief reads the serialization of the binary tree
 * @param *file input file to read from
 * @param *buffer buffer to store the line
 */
void get_line(FILE *file, char *buffer);

#endif
