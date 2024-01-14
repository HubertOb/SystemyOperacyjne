#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "consts.h"



int main(){
    key_t key=ftok(PATH,SOME_NUMBER);
    int semaphores=semget(key,0,IPC_CREAT); //0:poczekalnia,1:fotel dla klienta, 2:fotel dla fryzjera

    key_t mem_key=ftok(MEMORY_PATH,MEM_NUMBER);
    int shared_memory=shmget(mem_key,0,IPC_CREAT);

    struct sembuf waiting,customer_chair,hairdresser_chair;
    waiting.sem_num=WAITING_ROOM;
    waiting.sem_op=-1;
    waiting.sem_flg=IPC_NOWAIT;
    int res=semop(semaphores, &waiting, 1); //wchodzi do poczekalni
    if (res==-1){
        printf("poczekalnia zajęta, brak miejsc\n");
        return 0;
    }
    else{
        printf("klient wchodzi do poczekalni\n");
    }

    customer_chair.sem_num=CUSTOMER_CHAIR;
    customer_chair.sem_op=-1;

    hairdresser_chair.sem_num=HAIRDRESSER_CHAIR;
    hairdresser_chair.sem_op=1;

    semop(semaphores,&customer_chair,1); //siada na fotel lub czeka na wolny fotel
    printf("klient siada na fotel\n");


    waiting.sem_op=1;
    waiting.sem_flg=0;
    semop(semaphores,&waiting,1); //zwieksza miejsca w poczekalni bo z niej wyszedł.

    char* shmadrr;
    shmadrr=shmat(shared_memory,NULL,0);

    int index=0;
    for (int i=0;i<N;i++){
        if(*(shmadrr+i)==0){
            index=i;
            *(shmadrr+i)=1; //zaznacza w tablicy klientów że zajął fotel
            *(shmadrr+N+i)=1; //zaznacza na tablicy fryzjerów które miejsce zajął
            break;
        }
    }

    semop(semaphores,&hairdresser_chair,1); //informuje fryzjera że na niego czeka
    printf("klient informuje fryzjera że na niego czeka\n");

    srand(time(NULL));
    int hair=rand()%F+2; //losuje jaką fryzure chce mieć

    printf("zaczyna robić fryzure\n");
    sleep(hair); //fryzura się robi
    printf("fryzura gotowa\n");

    *(shmadrr+index)=0; //zaznacza w tablicy klientów że zwolnił fotel

    customer_chair.sem_op=1;
    semop(semaphores,&customer_chair,1); //zwalnia fotel
    printf("klient zwalnia fotel\n");

    shmdt(shmadrr);

    printf("klient wychodzi\n");

    return 0;
}