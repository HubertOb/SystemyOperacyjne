#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char* argv[]) {
    int status,a;
    if (argc!=2){
        printf("Niepoprawna liczba danych wejściowych!\n");
        return 0;
    }

    a=strtol(argv[1],NULL,10);
    pid_t z=0;
    for (int i=0;i<a;i++){
        z=fork();
        if (z==0){
            int pid=getpid();
            int ppid=getppid();
            printf("PPID = %d, PID = %d\n",ppid,pid);
            exit(0);
        }

    }
    for(int i = 0; i < a; i++) {
        wait(&status);
    }
    printf("Koniec, argv[1] = %d\n",a);
    return 0;
}
