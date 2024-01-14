#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    DIR *directory= opendir(".");
    struct dirent *a;
    struct stat stats;
    long long entire_size=0;

    if (directory==NULL){
        printf("Błędna ściezka.");
        return 0;
    }
    while((a=readdir(directory))!=NULL){
        if(stat(a->d_name,&stats)==-1){return 0;}
        if(S_ISDIR(stats.st_mode)){continue;}
        printf("Rozmiar: %ld, Nazwa: %s\n",stats.st_size,a->d_name);
        entire_size+=stats.st_size;
    }
    printf("Sumaryczny rozmiar: %lld\n",entire_size);
    return 0;
}
