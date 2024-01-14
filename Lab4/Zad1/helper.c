#include <stdio.h>
#include <string.h>
#include <signal.h>

const int SIGNAL=SIGUSR1;

void handler(int sig){
    printf("Otrzymano sygnał %d\n",sig);
}

int main(int argc, char* argv[]){
    if(strcmp(argv[0],"ignore")==0){
        printf("Exec wysyła sygnał:\n");
        raise(SIGNAL);

        struct sigaction sa;
        sigaction(SIGNAL, NULL, &sa);
        if (sa.sa_handler == SIG_IGN) {
            printf("Sygnał %d jest ignorowany\n", SIGNAL);
        }
    }

    else if(strcmp(argv[0],"mask")==0){
        printf("Exec wysyła sygnał:\n");
        raise(SIGNAL);

        sigset_t mask;
        sigaddset(&mask,SIGNAL);

        printf("Exec odblokowuje sygnał:\n");

        sigprocmask(SIG_UNBLOCK,&mask,NULL);
    }

    else if(strcmp(argv[0],"pending")==0){
        sigset_t pend,mask;
        sigaddset(&mask,SIGNAL);
        sigpending(&pend);
        if(sigismember(&pend,SIGNAL)==1){
            printf("Sygnał jest oczekujący w exec\n");
        }
        sigprocmask(SIG_UNBLOCK,&mask,NULL);
    }
    return 0;
}


