#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tree.h"
#include "file.h"
#define BUFFER_SIZE 1024
#define CHARSET 128

double execution_time(clock_t start, clock_t end, char *msg) {
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%s - %f seconds\n", msg, time_taken);

    return time_taken;
}

int main(int argc, char* argv[]) {
    
    if(argc < 3) return -1;

    int option_c = 0;
    int option_d = 0;
    int option_v = 0;
    char *filename = NULL;
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-c") == 0) option_c = 1;
        else if(strcmp(argv[i], "-d") == 0) option_d = 1;
        else if(strcmp(argv[i], "-v") == 0) option_v = 1;
        else if(filename == NULL) filename = argv[i];
        else {
            printf("Error: unexpected parameter.\n");
            return -1;
        }
    }

    if(option_c && option_d) {
        printf("Error: only one of the -c or -d can be active.\n");
        return -1;
    }

    if(filename == NULL) {
        printf("Error: filename not provided.\n");
        return -1;
    }

    double total_execution_time = 0;
    clock_t start = clock();
    FILE *f = open_file(filename, "rb");
    clock_t end = clock();
    if(option_v) total_execution_time += execution_time(start, end, "Opening file");

    Node n[CHARSET];
    for(int i = 0; i < CHARSET; i++) n[i].value = 0;

    if(option_c) { // Compress
        int ch;
        start = clock();
        while ((ch = fgetc(f)) != EOF) {
            n[ch].key = (char) ch;
            n[ch].value++;
            n[ch].left = NULL;
            n[ch].right = NULL;
        }
        end = clock();
        if(option_v) total_execution_time += execution_time(start, end, "Inserting char into tree");
        
        rewind(f);

        start = clock();
        sort(n, 0);
        end = clock();
        if(option_v) total_execution_time += execution_time(start, end, "Sorting tree");

        start = clock();
        Node bt = build_binary_tree(n);
        end = clock();
        if(option_v) total_execution_time += execution_time(start, end, "Building binary tree");

        start = clock();
        compress(f, &bt, filename);
        end = clock();
        if(option_v) total_execution_time += execution_time(start, end, "Compressing file");

        free_nodes(bt);
    } else if(option_d) { // Decompress
        char *buffer = malloc(BUFFER_SIZE);
        Node *bt;
        
        start = clock();
        get_line(f, buffer);
        end = clock();
        if(option_v) total_execution_time += execution_time(start, end, "Getting BinaryTree header");

        fseek(f, -1, SEEK_END);
        int bit_padding = fgetc(f) - '0';

        start = clock();
        bt = deserialize(&buffer);
        end = clock();
        if(option_v) total_execution_time += execution_time(start, end, "Deserialize BinaryTree");

        start = clock();
        decompress(f, bt, bit_padding, filename);
        end = clock();
        if(option_v) total_execution_time += execution_time(start, end, "Decompression");
    } else {
        printf("Error: missing compression/decompression parameter.\n");
    }

    if(option_v) printf("\nTotal execution time - %f seconds\n", total_execution_time);

    fclose(f);

    return 0;
}