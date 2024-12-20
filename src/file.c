#include "file.h"
#include <stdio.h>
#include <stdlib.h>

#define OPEN_FILE_ERR_MSG "Opening file\n"
#define BUFFER_SIZE 1024

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

void get_line(FILE *file, char *buffer) {
  int index = 0;
  int prev_char = 0;
  int curr_char = 0;

  while ((curr_char = fgetc(file)) != EOF && index < BUFFER_SIZE - 1) {
    buffer[index++] = curr_char;

    if (prev_char == '\n' && curr_char == '\n')
      break;

    prev_char = curr_char;
  }
}
