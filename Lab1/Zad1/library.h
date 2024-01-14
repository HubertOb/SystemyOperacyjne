#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ZAD1_LIBRARY_H
#define ZAD1_LIBRARY_H

#endif //ZAD1_LIBRARY_H

typedef struct{
    char** pointer;
    int max_size;
    int curr_size;
}struct_sth;

struct_sth* make_structure(int size);

int* count_words_and_lines(struct_sth *structure, const char* filepath);

char* get_block_from_index(struct_sth *structure,int index);

void remove_block(struct_sth *structure, int index);

void free_memory(struct_sth* structure);


