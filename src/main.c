#include "file.h"
#include "tree.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define CHARSET 128
#define OPTSTR "vc:f:"
#define TOT_EXEC_TME_MSG "\nTotal execution time - %f seconds\n"
#define UNEXPECTED_OPT_ERR_MSG "Unexpected parameter.\n"
#define MUTUAL_OPT_ERR_MSG "Only one of the -c or -d can be active.\n"
#define MANDATORY_OPT_ERR_MSG "One of -c or -d must be active.\n"
#define FILENAME_OPT_ERR_MSG "Filename not provided.\n"
#define UNKNOWN_OPT_MSG "Unknown option found will be ignored.\n"
#define NON_ASCII_ERR_MSG "File contains non-ASCII characters\n"
#define ONLY_ONE_CHAR_ERROR_MSG "File contains only 1 type of char\n"

typedef struct {
  u_int8_t verbose;
  u_int8_t compress;
  u_int8_t decompress;
  u_int8_t unknown;
  char *filename;
} options_t;

clock_t start;
clock_t end;
double total_execution_time = 0;

int validate_options(options_t *options);
double execution_time(clock_t start, clock_t end, char *msg);
int compress_opt(options_t *options, FILE *f);
int decompress_opt(options_t *options, FILE *f);

int main(int argc, char *argv[]) {
  int opt;
  options_t options = {0, 0, 0, 0, NULL};

  while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
    switch (opt) {
    case 'v':
      options.verbose += 1;
      break;
    case 'c':
      options.filename = optarg;
      options.compress += 1;
      break;
    case 'd':
      options.filename = optarg;
      options.decompress += 1;
      break;
    default:
      options.unknown += 1;
    }
  }

  if (validate_options(&options) == -1)
    exit(EXIT_FAILURE);

  start = clock();
  FILE *f = open_file(options.filename, "rb");
  end = clock();
  if (options.verbose)
    total_execution_time += execution_time(start, end, "Opening file");

  int res = 0;
  if (options.compress)
    res = compress_opt(&options, f);
  if (options.decompress)
    res = decompress_opt(&options, f);

  if (options.verbose)
    printf(TOT_EXEC_TME_MSG, total_execution_time);

  if (res == -1)
    exit(EXIT_FAILURE);

  fclose(f);

  return 0;
}

int validate_options(options_t *options) {
  if (options->compress && options->decompress) {
    perror(MUTUAL_OPT_ERR_MSG);
    return -1;
  }

  if (!options->compress && !options->decompress) {
    perror(MANDATORY_OPT_ERR_MSG);
    return -1;
  }

  if (options->filename == NULL) {
    perror(FILENAME_OPT_ERR_MSG);
    return -1;
  }

  if (options->unknown) {
    printf(UNKNOWN_OPT_MSG);
  }

  return 0;
}

double execution_time(clock_t start, clock_t end, char *msg) {
  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("%s - %f seconds\n", msg, time_taken);

  return time_taken;
}

int compress_opt(options_t *options, FILE *f) {
  Node n[CHARSET];
  for (int i = 0; i < CHARSET; i++)
    n[i].value = 0;

  int ch;
  start = clock();
  while ((ch = fgetc(f)) != EOF) {
    if (ch > 127) {
      perror(NON_ASCII_ERR_MSG);
      return -1;
    }
    n[ch].key[0] = (char)ch;
    n[ch].key[1] = '\0';
    n[ch].value++;
    n[ch].left = NULL;
    n[ch].right = NULL;
  }
  end = clock();
  if (options->verbose)
    total_execution_time +=
        execution_time(start, end, "Inserting char into tree");

  rewind(f);

  start = clock();
  sort(n, 0);
  end = clock();
  if (options->verbose)
    total_execution_time += execution_time(start, end, "Sorting tree");

  if (n[CHARSET - 2].value == 0) {
    perror(ONLY_ONE_CHAR_ERROR_MSG);
    return -1;
  }

  start = clock();
  Node bt = build_binary_tree(n);
  end = clock();
  if (options->verbose)
    total_execution_time += execution_time(start, end, "Building binary tree");

  start = clock();
  compress(f, &bt, options->filename);
  end = clock();
  if (options->verbose)
    total_execution_time += execution_time(start, end, "Compressing file");

  free_nodes(bt);

  return 0;
}

int decompress_opt(options_t *options, FILE *f) {
  char *buffer = malloc(BUFFER_SIZE);
  Node *bt;

  start = clock();
  get_line(f, buffer);
  end = clock();
  if (options->verbose)
    total_execution_time +=
        execution_time(start, end, "Getting BinaryTree header");

  fseek(f, -1, SEEK_END);
  int bit_padding = fgetc(f) - '0';

  start = clock();
  bt = deserialize(&buffer);
  end = clock();
  if (options->verbose)
    total_execution_time +=
        execution_time(start, end, "Deserialize BinaryTree");

  start = clock();
  decompress(f, bt, bit_padding, options->filename);
  end = clock();
  if (options->verbose)
    total_execution_time += execution_time(start, end, "Decompression");

  return 0;
}
