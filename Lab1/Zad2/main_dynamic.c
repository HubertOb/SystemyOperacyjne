#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "../zad1/library.h"


typedef struct{
    char** pointer;
    int max_size;
    int curr_size;
}struct_sth;

struct_sth *init(struct_sth (*make_structure)(int),int size){
    return make_structure(size);
}

void count_file(struct_sth *a,char* path){
    int*tab;
    tab= count_words_and_lines(a,path);
}

void show_index(struct_sth *a,int index){
    printf("%s", get_block_from_index(a,index));
}

void delete_index(struct_sth *a,int index){
    remove_block(a,index);
}

void destroy(struct_sth* a){
    free_memory(a);
}

int main() {
    void* handle=dlopen("../zad1/liblibrary.so",RTLD_LAZY);
    if(!handle){
        printf("nie działa dynamiczna biblioteka");
        return 1;
    }
    struct_sth *a;
    char command[50];
    while(1==1){
        fgets(command,50,stdin);
        char* token=strtok(command, " \n");
        if (strcmp(token,"init")==0){
            token= strtok(NULL," ");
            if (token){
                int size= strtol(token,NULL,10);
                struct struct_sth (*make_structure)()=(struct struct_sth (*)())dlsym(handle,"make_structure");
            }
            else{
                printf("Empty param\n");
            }
        }
        else if(strcmp(token,"count")==0){
            token= strtok(NULL," \n");
            if(token){
                int (*count_words_and_lines)(struct_sth,const char*) = dlsym(handle, "count_words_and_lines");
                (*count_words_and_lines)(a,token)
                count_file(a,token);
            }
            else{
                printf("Empty param\n");
            }
        }
        else if(strcmp(token, "show")==0){
            token=strtok(NULL," \n");
            if(token){
                int index= strtol(token,NULL,10);
                show_index(a,index);
            }
            else{
                printf("Empty param\n");
            }
        }
        else if(strcmp(token, "delete")==0){
            token= strtok(NULL," \n");
            if (token){
                int index= strtol(token,NULL,10);
                delete_index(a,index);
            }
            else{
                printf("Empty param\n");
            }
        }
        else if(strcmp(token,"destroy")==0){
            destroy(a);
        }
        else{
            printf("Wrong command\n");
        }

    }
    return 0;
}
