#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>


void foo(char* path, char* string_){
    int status;
    DIR *dir= opendir(path);
    if (dir==NULL){
        printf("Błędna ścieżka %s\n",path);
    }
    struct dirent *a;
    struct stat stats;
    pid_t t;

    while ((a=readdir(dir))!=NULL){
        char path_2[PATH_MAX];
        snprintf(path_2,PATH_MAX,"%s/%s",path,a->d_name);
        if(stat(path_2,&stats)==-1){
            perror("stat");
            return;}
        if (strcmp(a->d_name,".")==0 || strcmp(a->d_name,"..")==0){continue;}

        if (S_ISDIR(stats.st_mode)){
            t=fork();
            if (t==0){
                foo(path_2,string_);
                fflush(stdout);
                exit(EXIT_SUCCESS);
            }
            else{
                wait(&status);
            }
        }
        else{
            FILE *file= fopen(path_2,"r");
            unsigned long string_len= strlen(string_);
            char *start_file=calloc(string_len+1,sizeof(char));
            fread(start_file, sizeof(char),string_len,file);
            if (strcmp(string_,start_file)==0){
                printf("%s %ld\n",path_2,a->d_ino);
            }
            free(start_file);
            fclose(file);
        }
    }
    closedir(dir);
}


int main(int argc, char* argv[]) {
    int status;
    if (argc!=3){
        printf("Nieprawidłowa ilość danych wejściowych!\n");
        return 0;
    }
    if (strlen(argv[2])>255){
        printf("Za długi łańcuch znaków!\n");
        return 0;
    }
    fflush(stdout);
    char* path=argv[1];
    char* str=argv[2];
    setbuf(stdout, NULL);

    foo(path,str);
    return 0;
}
