#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define false 0;
#define true 1;

#define bool int

bool lib_foo(int argc, char* argv[]){
    char char_to_find= *argv[1];
    char char_to_change= *argv[2];
    char* in_filepath=argv[3];
    char* out_filepath=argv[4];

    FILE *file_in= fopen(in_filepath,"r");
    if(!file_in){
        printf("Niepoprawna ścieżka do pliku wejściowego!\n");
        return false;
    }

    FILE *file_out= fopen(out_filepath,"w");

    char a;
    while ((a= fgetc(file_in))!=EOF){
        if (a==char_to_find){
            fwrite(&char_to_change, sizeof(char),1,file_out);
        }
        else{
            fwrite(&a, sizeof(char),1,file_out);
        }
    }

    fclose(file_in);
    fclose(file_out);
    return true;
}


bool sys_foo(int argc, char* argv[]){
    char char_to_find= *argv[1];
    char char_to_change= *argv[2];
    char* in_filepath=argv[3];
    char* out_filepath=argv[4];

    char a;
    int file_in,file_out;

    file_in=open(in_filepath, O_RDONLY);
    file_out=open(out_filepath,O_WRONLY|O_CREAT);

    if(file_in==-1){
        printf("Niepoprawna ścieżka do pliku wejściowego!\n");
        return false;
    }

    while (read(file_in,&a,1)==1){
        if (a==char_to_find){
            write(file_out,&char_to_change,1);
        }
        else{
            write(file_out,&a,1);
        };
    }

    close(file_in);
    close(file_out);
    return true;
}


int main(int argc, char* argv[]) {
    if (argc!=5){
        printf("nieprawidłowa ilość danych wejściowych, powinno ich być 4\n");
        return 0;
    }

    FILE *results= fopen("pomiar_zad_1.txt", "w");
    clock_t start, end;

    start=clock();
    if (!lib_foo(argc,argv)){
        printf("Warning\n");
        return 0;
    }
    end=clock();
    fprintf(results,"Wyniki dla lib: %f\n",(double)(end-start)/CLOCKS_PER_SEC);

    start=clock();
    if (!sys_foo(argc,argv)){
        printf("Warning\n");
        return 0;
    }
    end=clock();
    fprintf(results,"Wyniki dla sys: %f\n",(double)(end-start)/CLOCKS_PER_SEC);

    fclose(results);
    return 0;
}
