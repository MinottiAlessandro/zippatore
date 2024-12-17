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
 * @brief sort the nodes in an ascending order
 * @param *n Node pointer
 */
void sort(Node *n, int start);

int tree_spider(Node *current, char key, char *path, int level);

Node build_binary_tree(Node *n);

void compress(FILE *f, Node *t, char *filename);

void serialize_wrapper(Node *t, FILE *f);

void serialize(Node *root, FILE *file);

void decompress(FILE *f, Node *t, int padding, char *filename);

Node* deserialize(char **buffer);

void free_nodes(Node n);

void free_node(Node *n);

#endif