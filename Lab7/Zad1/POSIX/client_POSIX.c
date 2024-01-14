#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include "consts2.h"



int main(){
    sem_t *waiting_sem,*customer_sem,*hairdresser_sem;
    waiting_sem= sem_open(WAITING_ROOM,0,0666);
    perror("wait");
    customer_sem= sem_open(CUSTOMER_CHAIR,0,0666);
    hairdresser_sem= sem_open(HAIRDRESSER_CHAIR,0,0666);
    int shared_memory= shm_open(SH_MEM_NAME,O_CREAT|O_RDWR,0666);

    int res= sem_trywait(waiting_sem); //wchodzi do poczekalni
    if (res==-1){
        printf("poczekalnia zajęta, brak miejsc\n");
        return 0;
    }
    else{
        printf("klient wchodzi do poczekalni\n");
    }

    sem_wait(customer_sem);
    printf("klient siada na fotel\n");

    sem_post(waiting_sem);

    char* shmadrr;
    shmadrr=mmap(NULL,2*N,PROT_WRITE,MAP_SHARED,shared_memory,0);

    int index=0;
    for (int i=0;i<N;i++){
        if(*(shmadrr+i)==0){
            index=i;
            *(shmadrr+i)=1; //zaznacza w tablicy klientów że zajął fotel
            *(shmadrr+N+i)=1; //zaznacza na tablicy fryzjerów które miejsce zajął
            break;
        }
    }

    sem_post(hairdresser_sem);
    printf("klient informuje fryzjera że na niego czeka\n");

    srand(time(NULL));
    int hair=rand()%F+2; //losuje jaką fryzure chce mieć

    printf("zaczyna robić fryzure\n");
    sleep(hair); //fryzura się robi
    printf("fryzura gotowa\n");

    *(shmadrr+index)=0; //zaznacza w tablicy klientów że zwolnił fotel

    sem_post(customer_sem);
    printf("klient zwalnia fotel\n");

    munmap(shmadrr,2*N);

    printf("klient wychodzi\n");

    return 0;
}