#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../zad1/library.h"

struct_sth *init(int size){
    return make_structure(size);
}

void count_file(struct_sth *a,char* path){
    int*tab;
    tab= count_words_and_lines(a,path);
    printf("%d\n",tab[1]);
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
    struct timespec{
        time_t tv_sec;
        long tv_nsec;
    };
    struct timespec start,end;
    struct_sth *a;
    char command[50];
    while(1==1){
        fgets(command,50,stdin);
        char* token=strtok(command, " \n");
        if (strcmp(token,"init")==0){
            clock_gettime(CLOCK_REALTIME,&start);
            token= strtok(NULL," ");
            if (token){
                int size= strtol(token,NULL,10);
                a=init(size);
            }
            else{
                printf("Empty param\n");
            }
            clock_gettime(CLOCK_REALTIME,&end);
            printf("Czas wykonania %ld: ",end.tv_nsec-start.tv_nsec);
        }
        else if(strcmp(token,"count")==0){
            clock_gettime(CLOCK_REALTIME,&start);
            token= strtok(NULL," \n");
            if(token){
                count_file(a,token);
            }
            else{
                printf("Empty param\n");
            }
            clock_gettime(CLOCK_REALTIME,&end);
            printf("Czas wykonania %ld: ",end.tv_nsec-start.tv_nsec);
        }
        else if(strcmp(token, "show")==0){
            clock_gettime(CLOCK_REALTIME,&start);
            token=strtok(NULL," \n");
            if(token){
                int index= strtol(token,NULL,10);
                show_index(a,index);
            }
            else{
                printf("Empty param\n");
            }
            clock_gettime(CLOCK_REALTIME,&end);
            printf("Czas wykonania %ld: ",end.tv_nsec-start.tv_nsec);
        }
        else if(strcmp(token, "delete")==0){
            clock_gettime(CLOCK_REALTIME,&start);
            token= strtok(NULL," \n");
            if (token){
                int index= strtol(token,NULL,10);
                delete_index(a,index);
            }
            else{
                printf("Empty param\n");
            }
            clock_gettime(CLOCK_REALTIME,&end);
            printf("Czas wykonania %ld: ",end.tv_nsec-start.tv_nsec);
        }
        else if(strcmp(token,"destroy")==0){
            clock_gettime(CLOCK_REALTIME,&start);
            destroy(a);
            clock_gettime(CLOCK_REALTIME,&end);
            printf("Czas wykonania %ld: ",end.tv_nsec-start.tv_nsec);
        }
        else if(strcmp(token,"exit_1")==0){
            break;
        }
        else{
            printf("Wrong command\n");
        }

    }
    return 0;
}
