#include <stdio.h>
#include <time.h>

#define false 0;
#define true 1;

#define bool int


bool wariant_1(int argc, char* argv[]){
    char* in_filepath=argv[1];
    char* out_filepath=argv[2];

    FILE *file_in= fopen(in_filepath,"r");
    if (!file_in){
        printf("Niepoprawna ścieżka do pliku wejściowego!\n");
        return false;
    }

    FILE *file_out= fopen(out_filepath,"w");

    char a;
    fseek(file_in,-1,SEEK_END);
    a= fgetc(file_in);
    fwrite(&a, sizeof(char),1,file_out);
    while (fseek(file_in,-2,SEEK_CUR)!=-1){
        a= fgetc(file_in);
        fwrite(&a, sizeof(char),1,file_out);
    }

    fclose(file_in);
    fclose(file_out);
    return true;
}


bool wariant_2(int argc, char* argv[]){
    char* in_filepath=argv[1];
    char* out_filepath=argv[2];

    FILE *file_in= fopen(in_filepath,"r");
    if (!file_in){
        printf("Niepoprawna ścieżka do pliku wejściowego!\n");
        return false;
    }

    FILE *file_out= fopen(out_filepath,"w");

    fseek(file_in,0,SEEK_END);

    long long file_length= ftell(file_in)/ sizeof(char);
    long long buffs=file_length/1024;
    long long r_buffs=file_length%1024;

    char a[r_buffs-1];
    fseek(file_in,-r_buffs,SEEK_CUR);
    fread(a, sizeof(char),r_buffs-1,file_in);
    char tmp;
    for (int i=0;i<r_buffs/2;i++){
        tmp=a[i];
        a[i]=a[r_buffs-2-i];
        a[r_buffs-2-i]=tmp;
    }
    fwrite(a,sizeof(char),r_buffs-1,file_out);
    fseek(file_in,-r_buffs-1023,SEEK_CUR);

    char b[1024];
    for (int j=0;j<buffs;j++){
        fread(b, sizeof(char),1024,file_in);
        for (int i=0;i<512;i++){
            tmp=b[i];
            b[i]=b[1023-i];
            b[1023-i]=tmp;
        }
        fwrite(b, sizeof(char),1024,file_out);
        fseek(file_in,-2048,SEEK_CUR);
    }

    char c=10;
    fwrite(&c,sizeof(char),1,file_out);
    fclose(file_in);
    fclose(file_out);
    return true;
}


int main(int argc, char* argv[]) {
    if (argc!=3){
        printf("Nieprawidłowa ilość danych wejściowych, powinno ich być 2");
        return 0;
    }

    FILE *results= fopen("pomiar_zad_2.txt","w");
    clock_t start, end;

    start=clock();
    if(!wariant_1(argc, argv)){
        printf("Warning\n");
        return 0;
    }
    end=clock();
    fprintf(results,"Wyniki dla wariantu 1: %f\n", (double)(end-start)/CLOCKS_PER_SEC);

    start=clock();
    if(!wariant_2(argc, argv)){
        printf("Warning\n");
        return 0;
    }
    end=clock();
    fprintf(results, "Wyniki dla wariantu 2: %f\n",(double)(end-start)/CLOCKS_PER_SEC);

    fclose(results);
    return 0;
}
