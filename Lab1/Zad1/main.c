#include "library.h"


int main() {
    struct_sth *a=make_structure(20);
    int* tab;
    tab=count_words_and_lines(a,"/home/hubert/Dokumenty/doc1.txt");
    char* z= get_block_from_index(a,0);
    printf("%s",z);
    remove_block(a,0);
    free_memory(a);
    printf("%d",a==NULL);
    return 0;
}
