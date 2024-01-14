#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#include "consts2.h"

int flag=1;
sem_t *waiting_sem,*customer_sem,*hairdresser_sem;
int shared_memory;

void handler(int signum){
    printf("koniec działania\n");
    flag=0;
}

int main() {
    waiting_sem= sem_open(WAITING_ROOM,O_CREAT,0777,P);
    customer_sem= sem_open(CUSTOMER_CHAIR,O_CREAT,0777,N);
    hairdresser_sem= sem_open(HAIRDRESSER_CHAIR,O_CREAT,0777,0);

    shared_memory= shm_open(SH_MEM_NAME,O_CREAT|O_RDWR,0666);
    int a=ftruncate(shared_memory,2*N);

    printf("shared memory %d\n",shared_memory);

    char*shm= mmap(NULL,2*N,PROT_WRITE,MAP_SHARED,shared_memory,0);

    for(int i=0;i<2*N;i++){
        *(shm+i)=0;
    }


    signal(SIGINT,handler);

    while(flag){
        continue;
    }

    munmap(shm,2*N);
    shm_unlink(SH_MEM_NAME);
    sem_close(waiting_sem);
    sem_close(customer_sem);
    sem_close(hairdresser_sem);

    sem_unlink(WAITING_ROOM);
    sem_unlink(CUSTOMER_CHAIR);
    sem_unlink(HAIRDRESSER_CHAIR);

    return 0;
}
