#include "tree.h"
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARSET 128
#define OPEN_FILE_ERR_MSG "Opening file\n"
#define BUFFER_SIZE 8192
#define OUTPUT_BUFFER_SIZE 16384
#define PATH_BUFFER_SIZE 256

void free_nodes(Node n) {
  free_node(n.left);
  free_node(n.right);
}

void free_node(Node *n) {
  if (n->left != NULL)
    free_node(n->left);
  if (n->right != NULL)
    free_node(n->right);
  free(n);
}

int tree_spider(Node *current, char key, char *path, int level) {
  if (current->key[0] != key && current->left == NULL && current->right == NULL)
    return 0;
  if (current->key[0] == key && current->left == NULL &&
      current->right == NULL) {
    path[level] = '\0';
    return 1;
  }

  path[level] = '0';
  if (current->left != NULL &&
      tree_spider(current->left, key, path, level + 1) == 1)
    return 1;

  path[level] = '1';
  if (current->right != NULL &&
      tree_spider(current->right, key, path, level + 1) == 1)
    return 1;

  return 0;
}

void sort(Node *n, int start) {
  int swap;
  for (unsigned int i = start; i < CHARSET; i++) {
    swap = 0;
    for (unsigned int j = i + 1; j < CHARSET; j++) {
      if (n[i].value < n[j].value)
        continue;

      swap = 1;
      Node tmp = n[i];
      n[i] = n[j];
      n[j] = tmp;
    }
    if (!swap)
      break;
  }
}

Node build_binary_tree(Node *n) {
  int start = 0;
  for (int i = 0; i < CHARSET; i++) {
    if (n[i].value <= 0)
      continue;
    start = i;
    break;
  }

  while (start < 127) {
    Node *left = (Node *)malloc(sizeof(Node));
    strcpy(left->key, n[start].key);
    left->value = n[start].value;
    left->left = n[start].left;
    left->right = n[start].right;
    Node *right = (Node *)malloc(sizeof(Node));
    strcpy(right->key, n[start + 1].key);
    right->value = n[start + 1].value;
    right->left = n[start + 1].left;
    right->right = n[start + 1].right;

    Node new;

    strcpy(new.key, "\xde\xad\xbe\xef\0");
    new.value = left->value + right->value;
    new.left = left;
    new.right = right;

    n[start].value = 0;
    n[start + 1] = new;

    sort(n, start + 1);

    start++;
  }

  return n[127];
}

int compress_old(FILE *f, Node *t, char *filename) {
  FILE *w;
  if ((w = fopen(strcat(filename, ".zippatore"), "wb")) == NULL) {
    perror(OPEN_FILE_ERR_MSG);
    return -1;
  }
  int ch;
  char bits[9] = {'\0'};

  int null_flag = 0;
  char cache_key[CHARSET];
  for (int c = 0; c < CHARSET; c++)
    cache_key[c] = '\0';
  char *cache_val[CHARSET];

  serialize_wrapper(t, w);
  while ((ch = fgetc(f)) != EOF) {
    char actualChar = (char)ch;

    char *path = malloc(10);
    if (cache_key[ch] != actualChar || (actualChar == '\0' && null_flag == 0)) {
      tree_spider(t, actualChar, path, 0);

      if (actualChar == '\0')
        null_flag = 1;
      cache_key[ch] = actualChar;
      cache_val[ch] = malloc(sizeof(char) * strlen(path));
      cache_val[ch] = path;

    } else {
      path = cache_val[ch];
    }

    int i = 0;
    int bits_len = strlen(bits);
    while (path[i] != '\0') {
      bits[bits_len] = path[i];
      bits_len++;
      i++;

      if (bits_len >= 8) {
        appendBitsToFile(bits, w);
        for (int j = 0; j <= 9; j++)
          bits[j] = '\0';
        bits_len = 0;
      }
    }
  }
  if (strlen(bits) > 0) {
    appendBitsToFile(bits, w);
    char padding[] = {(8 - strlen(bits)) + '0', '\0'};
    fprintf(w, "%s", padding);
  } else {
    char padding[] = {'0', '\0'};
    fprintf(w, "%s", padding);
  }

  fclose(w);
  return 0;
}

int compress(FILE *f, Node *t, char *filename) {
  // Pre-allocate buffers on stack
  char output_filename[PATH_BUFFER_SIZE];
  char read_buffer[BUFFER_SIZE];
  unsigned char output_buffer[OUTPUT_BUFFER_SIZE];
  size_t output_pos = 0;

  // Prepare output file
  snprintf(output_filename, PATH_BUFFER_SIZE, "%s.zippatore", filename);
  FILE *w = fopen(output_filename, "wb");
  if (!w) {
    perror(OPEN_FILE_ERR_MSG);
    return -1;
  }

  // Initialize cache with static allocation
  static char cache_key[CHARSET] = {0};
  static char *cache_val[CHARSET] = {0};
  static char path_buffer[PATH_BUFFER_SIZE] = {0};

  // Write tree structure first
  serialize_wrapper(t, w);

  // Bit accumulator
  unsigned char bits_buffer = 0;
  int bits_count = 0;
  int null_flag = 0;

  // Process file in chunks
  size_t bytes_read;
  while ((bytes_read = fread(read_buffer, 1, BUFFER_SIZE, f)) > 0) {
    for (size_t i = 0; i < bytes_read; i++) {
      unsigned char ch = (unsigned char)read_buffer[i];
      const char *path;

      // Cache lookup and update
      if (cache_key[ch] != ch || (ch == '\0' && !null_flag)) {
        tree_spider(t, ch, path_buffer, 0);
        if (ch == '\0') {
          null_flag = 1;
        }

        // Update cache
        cache_key[ch] = ch;
        size_t path_len = strlen(path_buffer) + 1;
        char *new_path = realloc(cache_val[ch], path_len);
        if (new_path) {
          cache_val[ch] = new_path;
          memcpy(cache_val[ch], path_buffer, path_len);
        }
        path = path_buffer;
      } else {
        path = cache_val[ch];
      }

      // Process path bits
      for (const char *p = path; *p; p++) {
        bits_buffer = (bits_buffer << 1) | (*p == '1');
        bits_count++;

        if (bits_count == 8) {
          output_buffer[output_pos++] = bits_buffer;
          if (output_pos >= OUTPUT_BUFFER_SIZE) {
            fwrite(output_buffer, 1, output_pos, w);
            output_pos = 0;
          }
          bits_buffer = 0;
          bits_count = 0;
        }
      }
    }
  }

  // Handle remaining bits
  if (bits_count > 0) {
    bits_buffer <<= (8 - bits_count);
    output_buffer[output_pos++] = bits_buffer;
  }

  // Flush remaining output buffer
  if (output_pos > 0) {
    fwrite(output_buffer, 1, output_pos, w);
  }

  // Write padding information
  fputc(bits_count ? ('0' + (8 - bits_count)) : '0', w);

  // Cleanup
  for (int i = 0; i < CHARSET; i++) {
    free(cache_val[i]);
  }

  fclose(w);
  return 0;
}

void serialize_wrapper(Node *t, FILE *f) {
  serialize(t, f);
  fprintf(f, "\n\n");
}

void serialize(Node *root, FILE *file) {
  if (root == NULL) {
    fprintf(file, "%c", '\0');
    return;
  }

  if (!strcmp(root->key, "\xde\xad\xbe\xef")) {
    fprintf(file, "%c", '\1');
  } else {
    fprintf(file, "%c", '\2');
    fprintf(file, "%c", root->key[0]);
  }

  serialize(root->left, file);
  serialize(root->right, file);
}

int decompress(FILE *f, Node *t, int padding, char *filename) {
  rewind(f);

  int prev_char = 0;
  int curr_char = 0;
  int next_char = 0;
  int next_next_char = 0;
  while ((curr_char = fgetc(f)) != EOF) {
    if (prev_char == '\n' && curr_char == '\n') {
      break;
    }

    prev_char = curr_char;
  }

  curr_char = fgetc(f);
  next_char = fgetc(f);
  char currentByte = (char)curr_char;
  Node *current = t;
  char *extension = strstr(filename, ".zippatore");
  if (extension != NULL)
    *extension = '\0';
  FILE *w;
  if ((w = fopen(filename, "w")) == NULL) {
    perror(OPEN_FILE_ERR_MSG);
    return -1;
  }
  while ((next_next_char = fgetc(f)) != EOF) {
    for (int i = 0; i < 8; i++) {
      int bit = (currentByte >> (7 - i)) & 1;
      if (strcmp(current->key, "\xde\xad\xbe\xef")) {
        fprintf(w, "%c", current->key[0]);
        current = !bit ? t->left : t->right;
        continue;
      }

      if (!bit)
        current = current->left;
      else
        current = current->right;
    }
    currentByte = (char)next_char;
    next_char = next_next_char;
  }

  for (int i = 0; i < 8 - (padding - 1); i++) {
    int bit = (currentByte >> (7 - i)) & 1;
    if (strcmp(current->key, "\xde\xad\xbe\xef")) {
      fprintf(w, "%c", current->key[0]);
      current = !bit ? t->left : t->right;
      continue;
    }

    if (!bit)
      current = current->left;
    else
      current = current->right;
  }

  fclose(w);
  return 0;
}

Node *deserialize(char **buffer) {
  if (**buffer == '\0') {
    (*buffer)++;
    return NULL;
  }

  Node *node = (Node *)malloc(sizeof(Node));
  if (**buffer == '\1') {
    strcpy(node->key, "\xde\xad\xbe\xef\0");
  } else {
    (*buffer)++;
    node->key[0] = **buffer;
  }

  (*buffer)++;
  node->left = deserialize(buffer);
  node->right = deserialize(buffer);

  return node;
}
