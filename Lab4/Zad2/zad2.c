#include <stdio.h>
//#include <bits/types/siginfo_t.h>
//#include <bits/sigaction.h>
//#include <bits/signum.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

void handler(int signum, siginfo_t *info, void *context){
    printf("Numer sygnału: %d, PID procesu wysyłającego: %d, Typ sygnału: %d, Kod błędu: %d, Identyfikator używkownika: %d\n\n",info->si_signo,info->si_pid, info->si_code,info->si_errno,info->si_uid);
}

void handler2(int signum, siginfo_t *info, void *context){
    int status;
    pid_t pid=fork();
    if (pid==0){
        printf("Dziecko zaczyna pracować\n");
        printf("Dziecko kończy prace\n");
        raise(SIGSTOP);
        exit(0);
    }
    else{
        printf("Rodzic zaczyna czekać\n");
        sleep(5);
        printf("Rodzic nie skończy czekac dopóki nie otrzyma sygnału od dziecka, że skończył działanie.\n");
        kill(pid,SIGCONT);
        wait(&status);
        printf("Rodzic skończył czekać\n\n");
    }
}

void handler3(int signum, siginfo_t *info, void *context){
    int status;
    pid_t pid=fork();
    if(pid==0){
        printf("\nDziecko zaczyna pracować\n");
        sleep(3);
        printf("Dziecko kończy prace\n");
    }
    else{
        printf("Rodzic zaczyna czekać.\n");
        printf("Rodzic kończy czekać.\n"); //ta linijka nigdy się nie wykona
    }
}


int main() {
    int signals[3]={SIGSEGV,SIGBUS,SIGILL};
    struct sigaction act1;
    act1.sa_sigaction= &handler;
    sigemptyset(&act1.sa_mask);

    act1.sa_flags=0;
    act1.sa_flags=SA_SIGINFO;
    sigaction(signals[0],&act1, NULL);
    raise(signals[0]);

    sigemptyset(&act1.sa_mask);
    act1.sa_flags=SA_NOCLDSTOP;
    act1.sa_sigaction= &handler2;
    sigaction(signals[2],&act1, NULL);
    raise(signals[2]);

    sigemptyset(&act1.sa_mask);
    act1.sa_flags=SA_NOCLDWAIT;
    act1.sa_sigaction=&handler3;
    sigaction(signals[1],&act1, NULL);
    raise(signals[1]);


    return 0;
}
