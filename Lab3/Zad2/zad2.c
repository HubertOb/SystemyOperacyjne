#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc!=2){
        printf("Niepoprawna ilość danych wejściowych!\n");
        return 0;
    }

    setbuf(stdout, NULL);

    printf("%s",argv[0]);

    execl("/bin/ls","ls",argv[1],NULL);
    return 0;
}
