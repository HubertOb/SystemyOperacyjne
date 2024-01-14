#include "library.h"

struct_sth* make_structure(int size){
    struct_sth *a= malloc(sizeof(struct_sth));
    a->pointer= calloc(size,sizeof(char*));
    a->max_size=size;
    a->curr_size=0;
    return a;
}


int* count_words_and_lines(struct_sth *structure, const char* filepath){
    char sys_command[100]="wc ";
    strcat(sys_command,filepath);
    strcat(sys_command," > /tmp/tmpFile.txt");
    system(sys_command);
    FILE * temporary_file= fopen("/tmp/tmpFile.txt","r");
    int lines,words,characters;
    char d[50];
    fscanf(temporary_file, "%d %d %d %s", &lines, &words, &characters, d);
    fclose(temporary_file);
    structure->pointer[structure->curr_size]= calloc(characters, sizeof(char));
    FILE *file1= fopen(filepath,"r");
    char* buffer= malloc(characters * sizeof(char));
    fread(buffer, sizeof(char), characters, file1);
    structure->pointer[structure->curr_size]=buffer;
    structure->curr_size++;
    fclose(file1);
    remove("/tmp/tmpFile.txt");
    int*tab=malloc(2* sizeof(int));
    tab[0]=lines;
    tab[1]=words;
    return tab;
}

char* get_block_from_index(struct_sth *structure,int index){
    return structure->pointer[index];
}


void remove_block(struct_sth *structure, int index){
    free(structure->pointer[index]);
    structure->curr_size--;
}


void free_memory(struct_sth* structure){
    free(structure);
}
