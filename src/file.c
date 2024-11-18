#include <stdio.h>
#include <stdlib.h>
#include "file.h"

FILE* open_file(const char *filename, char *mode) {
    FILE *f = fopen(filename, mode);

    if(f ==  NULL) {
        perror("Error: opening file");
        abort();
    }

    return f;
}

void appendBitsToFile(char *bits, FILE *file) {
    unsigned char byte = 0;
    int count = 0;

    while (*bits) {
        byte = (byte << 1) | (*bits - '0');
        count++;

        if (count == 8) {
            fputc(byte, file);
            byte = 0;
            count = 0;
        }

        bits++;
    }

    if (count > 0) {
        byte <<= (8 - count);
        fputc(byte, file);
    }
}

int readBit(FILE *file) {
    static int bitPosition = 8;
    static unsigned char currentByte;

    if (bitPosition == 8) {
        int readResult = fgetc(file);
        if (readResult == EOF) {
            return -1;
        }
        currentByte = (unsigned char)readResult;
        bitPosition = 0;
    }

    int bit = (currentByte >> (7 - bitPosition)) & 1;
    bitPosition++;

    return bit;
}

#define BUFFER_SIZE 1024
void get_line(FILE *file, char* buffer) {
    if (file == NULL) {
        perror("Error: opening file");
        return;
    }

    int index = 0;
    int prev_char = 0;
    int curr_char = 0;

    while ((curr_char = fgetc(file)) != EOF && index < BUFFER_SIZE - 1) {
        buffer[index++] = curr_char;

        if (prev_char == '\n' && curr_char == '\n') {
            break;
        }

        prev_char = curr_char;
    }

    buffer[index] = '\1';
}
