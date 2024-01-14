#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "consts.h"



union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int flag=1;
int semaphores;

int shared_memory;

void handler(int signum){
    printf("koniec działania\n");
    flag=0;
}

int main() {
    key_t key=ftok(PATH,SOME_NUMBER);
    semaphores=semget(key,3,0666|IPC_CREAT); //0:poczekalnia,1:fotel dla klienta, 2:fotel dla fryzjera

    printf("semaphores %d\n",semaphores);
    union semun waiting_room,customer_chair,hairdresser_chair;

    waiting_room.val=P;
    semctl(semaphores,WAITING_ROOM,SETVAL,waiting_room);

    customer_chair.val=N;
    semctl(semaphores,CUSTOMER_CHAIR,SETVAL,customer_chair);

    hairdresser_chair.val=0;
    semctl(semaphores,HAIRDRESSER_CHAIR,SETVAL,hairdresser_chair);


    key_t mem_key=ftok(MEMORY_PATH,MEM_NUMBER);
    shared_memory=shmget(mem_key,2*N,0666|IPC_CREAT);

    printf("shared memory %d\n",shared_memory);

    int k=0;
    char*shm=shmat(shared_memory,NULL,0);

    for(int i=0;i<2*N;i++){
        *(shm+i)=0;
    }

    signal(SIGINT,handler);

    while(flag){
        continue;
    }

    shmdt(shm);
    shmctl(shared_memory,IPC_RMID,NULL);
    semctl(semaphores,WAITING_ROOM,IPC_RMID,0);
    semctl(semaphores,CUSTOMER_CHAIR,IPC_RMID,0);
    semctl(semaphores,HAIRDRESSER_CHAIR,IPC_RMID,0);

    return 0;
}
