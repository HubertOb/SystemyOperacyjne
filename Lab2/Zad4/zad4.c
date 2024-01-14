#include <stdio.h>
#include <ftw.h>
#include <libgen.h>

long long entire_size=0;

static int foo(const char *path, const struct stat *stats, int flag){
    if (S_ISDIR(stats->st_mode)){
        return 0;
    }
    entire_size+=stats->st_size;
    char *a=basename((char*)path);
    printf("Rozmiar: %ld, Nazwa: %s\n", stats->st_size,a);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc!=2){
        printf("Niepoprawna ilośc danych wejściowych!\n");
        return 0;
    }
    ftw(argv[1],foo,300);
    printf("Całkowity rozmiar plików: %lld\n", entire_size);
    return 0;
}
