#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "consts.h"

char* shmadrr;

void handler(int signum){
    shmdt(shmadrr);
    printf("Fryzjer kończy pracę\n");
    exit(0);
}

int main(){
    signal(SIGINT,handler);
    key_t key=ftok(PATH,SOME_NUMBER);
    int semaphores=semget(key,0,IPC_CREAT); //0:poczekalnia,1:fotel dla klienta, 2:fotel dla fryzjera


    key_t mem_key=ftok(MEMORY_PATH,MEM_NUMBER);
    int shared_memory=shmget(mem_key,0,IPC_CREAT);


    shmadrr=shmat(shared_memory,NULL,0);


    if(shmadrr==-1){
        printf("błąd\n");
    }

    struct sembuf hairdresser_chair;
    hairdresser_chair.sem_op=-1;
    printf("fryzjer wchodzi do salonu\n");
    hairdresser_chair.sem_num=HAIRDRESSER_CHAIR;

    while(1){
        printf("fryzjer czeka na klienta\n");
        int a=semop(semaphores,&hairdresser_chair,1);
        int nr_fotela=0;
        for (int i=0;i<N;i++){
            if(*(shmadrr+N+i)==1){
                nr_fotela=i;
                *(shmadrr+N+i)=0;
                printf("fryzjer podchodzi do fotela na którym siedzi klient\n");
                break;
            }
        }
        int flag=1;
        int flag2=1;
        while(flag){
            //fryzurka się robi
            if(flag2){
                printf("fryzjer robi klienta\n");
                flag2=0;
            }
            if(*(shmadrr+nr_fotela)==0){
                flag=0;
            }
        }
        printf("fryzjer kończy robić klienta i idzie na zaplecze\n");
    }
    return 0;
}
