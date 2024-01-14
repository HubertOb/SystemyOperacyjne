#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

int licznik=0;
int end_loop=0;
int inner_loop=0;

void handler(int signum, siginfo_t *info, void *context){
    inner_loop=0;
    printf("Otrzymano w catcherze\n");
    printf("cosik %d\n",info->si_status);
    licznik++;
    printf("pid: %d\n",info->si_pid);
    kill(info->si_pid,SIGUSR1);
    time_t t;
    switch (info->si_value.sival_int) {
        case 1:
            for (int i=1;i<101;i++){
                printf("%d\n",i);
            }
            break;
        case 2:
            time(&t);
            printf("Aktualny czas: %ld\n",t);
            break;
        case 3:
            printf("Liczba otrzymanyc rządań: %d\n",licznik);
            break;
        case 4:
            inner_loop=1;
            while (inner_loop){
                sigset_t set;
                int sig;
                time(&t);
                printf("Aktualny czas: %ld\n",t);
                sleep(1);
                sigwait(&set,&sig);
                if (sig==SIGUSR1){
                    inner_loop=0;
                }
            }
            break;
        case 5:
            end_loop=1;
            break;
        default:
            printf("Niepoprawna przeslana dana.\n");
            break;
    }
}

int main() {
    printf("Catcher PID: %d\n", getpid());

    struct sigaction act;
    sigset_t set;
    sigaddset(&set,SIGUSR1);

    act.sa_sigaction=handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO;
    sigaction(SIGUSR1,&act,NULL);


    while(end_loop==0){};


    return 0;
}
