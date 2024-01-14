#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum){
    printf("Otrzymano\n");
}



int main(int argc, char* argv[]) {
    if (argc<3){
        printf("Niepoprawna ilość danych wejściowych!\n");
        return 0;
    }
    else if(strcmp(argv[argc-1],"5")!=0){
        printf("Ostatni argument powinien być 5!\n");
        return 0;
    }

    pid_t pid=strtol(argv[1],NULL,10);
    printf("pid: %d\n",pid);
    sigset_t set;
    sigaddset(&set, SIGUSR1);

    signal(SIGUSR1,handler);

    sigval_t sig_val;

    union sigval val;
    val.sival_int=strtol(argv[2],NULL,10);
    sigqueue(pid,SIGUSR1,val);
    printf("wysłano\n");
    for (int i=3;i<argc;i++){
        printf("Wysłano");
        val.sival_int=strtol(argv[i],NULL,10);
        pause();
        sigqueue(pid,SIGUSR1,val);
    }
    return 0;
}
