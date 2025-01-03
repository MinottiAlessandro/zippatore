#ifndef TREE_H
#define TREE_H
#include <stdio.h>

typedef struct Node {
  struct Node *left;
  struct Node *right;
  char key[5];
  int value;
} Node;

/**
 * @brief sort the array of nodes
 * @param n array of nodes
 * @param start starting index
 */
void sort(Node *n, int start);

/**
 * @brief traverse the binary tree to search for a specific key and store the path to it
 * @param current current node being traversed
 * @param key key to search for
 * @param path array to store path information
 * @param level current level in the tree
 * @return 1 if found, 0 if not found
 */
int tree_spider(Node *current, char key, char *path, int level);

/**
 * @brief build the binary tree from the array of nodes
 * @param n array of nodes
 * @return the root of the binary tree
 */
Node build_binary_tree(Node *n);

/**
 * @brief compress the file
 * @param f file to compress
 * @param t binary tree
 * @param filename name of the file
 * @return 0 if successful, -1 if not
 */
int compress_old(FILE *f, Node *t, char *filename);

/**
 * @brief compress the file
 * @param f file to compress
 * @param t binary tree
 * @param filename name of the file
 * @return 0 if successful, -1 if not
 */
int compress(FILE *f, Node *t, char *filename);

/**
 * @brief serialize the binary tree
 * @param t binary tree
 * @param f file to write to
 */
void serialize_wrapper(Node *t, FILE *f);

/**
 * @brief serialize the binary tree
 * @param root binary tree
 * @param file file to write to
 */
void serialize(Node *root, FILE *file);

/**
 * @brief decompress the file
 * @param f file to decompress
 * @param t binary tree
 * @param padding padding information
 * @param filename name of the file
 * @return 0 if successful, -1 if not
 */
int decompress_old(FILE *f, Node *t, int padding, char *filename);

/**
 * @brief decompress the file
 * @param f file to decompress
 * @param t binary tree
 * @param padding padding information
 * @param filename name of the file
 * @return 0 if successful, -1 if not
 */
int decompress(FILE *f, Node *t, int padding, char *filename);

/**
 * @brief deserialize the binary tree
 * @param buffer buffer to read from
 * @return the binary tree
 */
Node *deserialize(char **buffer);

/**
 * @brief free the nodes of the binary tree
 * @param n binary tree
 */
void free_nodes(Node n);

/**
 * @brief free a node
 * @param n node to free
 */
void free_node(Node *n);

#endif
