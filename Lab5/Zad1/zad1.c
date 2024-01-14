#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc!=2 && argc!=3){
        printf("Niepoprawna ilość danych wejściowych!\nWpisz \"fortune\" lub \"cowsay <tekst do wypisania>\"\n");
        return 0;
    }
    if (strcmp(argv[1], "fortune")==0 && argc==2){
        FILE* fort=popen("fortune","r");
        char buff[1024];
        if (fort==NULL){
            printf("Bład!\n");
            return 0;
        }
        while (fgets(buff, sizeof(buff),fort)!=NULL){
            printf("%s",buff);
        }
        pclose(fort);
    }
    else if(strcmp(argv[1], "cowsay")==0 && argc==3){
        FILE* cow= popen("cowsay","w");
        if(cow==NULL){
            printf("Błąd\n");
            return 0;
        }
        fprintf(cow,"%s",argv[2]);
        pclose(cow);
    }
    else{
        printf("Niepoprawne dane!\nWpisz \"fortune\" lub \"cowsay <tekst do wypisania>\"\n");
    }
    return 0;
}
