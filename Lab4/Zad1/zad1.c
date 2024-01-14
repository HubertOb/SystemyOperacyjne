#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

const int SIGNAL=SIGUSR1;

enum Execution{
    FORK,EXEC
};

void handler(int sig){
    printf("Otrzymano sygnał %d\n",sig);
}

void ignore_function(enum Execution a, char* arg){
    printf("%s\n",arg);
    int status;
    signal(SIGNAL,handler);
    signal(SIGNAL,SIG_IGN);
    printf("Rodzic wysyła sygnał:\n");
    raise(SIGNAL);
    if (a==FORK){
        pid_t pid=fork();
        if (pid==0){
            printf("Potomek wysyła sygnał:\n");
            raise(SIGNAL);
            exit(0);
        }
        else{
            wait(&status);
        }
    }
    else if (a==EXEC){
        execl("./helper", arg, NULL);
    }
}

void handler_function(enum Execution a){
    int status;
    signal(SIGNAL,handler);
    printf("Rodzic wysyła sygnał:\n");
    raise(SIGNAL);
    pid_t pid=fork();
    if (pid==0){
        printf("Potomek wysyła sygnał:\n");
        raise(SIGNAL);
        exit(0);
    }
    else{
        wait(&status);
    }
}

void mask_function(enum Execution a, char* arg){
    int status;
    signal(SIGNAL,handler);

    sigset_t mask,old_mask;
    sigaddset(&mask,SIGNAL);
    sigprocmask(SIG_BLOCK,&mask,&old_mask);

    printf("Rodzic wysyła sygnał:\n");
    raise(SIGNAL);

    if (a==FORK){
        pid_t pid=fork();
        if (pid==0){
            printf("Potomek wysyła sygnał: \n");
            raise(SIGNAL);
            printf("Potomek odblokowuje sygnał:\n");
            sigprocmask(SIG_UNBLOCK,&mask,NULL);
            exit(0);
        }
        else{
            wait(&status);
            printf("Rodzic odblokowuje sygnał:\n");
            sigprocmask(SIG_UNBLOCK,&mask,NULL);
        }
    }
    else if (a==EXEC){
        execl("./helper",arg,NULL);
    }
}

void pending_function(enum Execution a, char* arg){
    int status;
    signal(SIGNAL, handler);

    sigset_t mask,old_mask,pend;
    sigaddset(&mask,SIGNAL);
    sigprocmask(SIG_BLOCK,&mask,&old_mask);
    sigemptyset(&pend);

    raise(SIGNAL);

    sigpending(&pend);

    if(sigismember(&pend,SIGNAL)==1){
        printf("Sygnał %d jest oczekujący w rodzicu\n", SIGNAL);
    }

    if (a==FORK){
        pid_t pid=fork();
        if (pid==0){
            sigpending(&pend);
            if(sigismember(&pend,SIGNAL)==1){
                printf("Sygnał %d jest oczekujący w potomku\n", SIGNAL);
            }
            else{
                printf("Sygnał %d jest niewidoczny w potomku\n",SIGNAL);
            }
            raise(SIGNAL);
            sigpending(&pend);
            if(sigismember(&pend,SIGNAL)==1){
                printf("Teraz sygnał %d jest oczekujący w potomku\n",SIGNAL);
            }
            printf("Odblokowuje sygnał w potomku\n");
            sigprocmask(SIG_UNBLOCK,&mask,NULL);
            sigpending(&pend);
            if (sigismember(&pend,SIGNAL)==0){
                printf("Sygnał %d nie jest już oczekujący w potomku\n",SIGNAL);
            }
            exit(0);
        }
        else{
            wait(&status);
            printf("Odblokowuje sygnał w rodzicu\n");
            sigprocmask(SIG_UNBLOCK,&mask,NULL);
            sigpending(&pend);
            if (sigismember(&pend,SIGNAL)==0){
                printf("Sygnał %d nie jest już oczekujący w rodzicu\n", SIGNAL);
            }
        }
    }
    else if(a==EXEC){
        execl("./helper",arg,NULL);
    }

}

int main(int argc, char* argv[]) {
    if (argc!=2){
        printf("Niepoprawna ilość danych wejściowych\n");
        return 0;
    }
    if(strcmp(argv[1], "ignore")==0){
        printf("FORK:\n");
        ignore_function(FORK,argv[1]);
        printf("\nEXEC:\n");
        ignore_function(EXEC,argv[1]);
    }
    else if(strcmp(argv[1], "handler")==0){
        handler_function(FORK);
    }
    else if(strcmp(argv[1], "mask")==0){
        printf("FORK:\n");
        mask_function(FORK,argv[1]);
        printf("\nEXEC:\n");
        mask_function(EXEC,argv[1]);
    }
    else if(strcmp(argv[1], "pending")==0){
        printf("FORK:\n");
        pending_function(FORK,argv[1]);
        printf("\nEXEC:\n");
        pending_function(EXEC,argv[1]);
    }
    else{
        printf("Niepoprawne polecenie\n");
        return 0;
    }
    return 0;
}
