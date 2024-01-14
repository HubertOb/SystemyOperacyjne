#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <stdlib.h>

#include "consts2.h"

char* shmadrr;

void handler(int signum){
    munmap(shmadrr,2*N);
    printf("Fryzjer kończy pracę\n");
    exit(0);
}

int main(){
    signal(SIGINT,handler);
    sem_t *waiting_sem,*customer_sem,*hairdresser_sem;
    hairdresser_sem= sem_open(HAIRDRESSER_CHAIR,0,0666);
    int shared_memory= shm_open(SH_MEM_NAME,O_CREAT|O_RDWR,0666);

    shmadrr=mmap(NULL,2*N,PROT_WRITE,MAP_SHARED,shared_memory,0);

    printf("fryzjer wchodzi do salonu\n");

    while(1){
        printf("fryzjer czeka na klienta\n");
        sem_wait(hairdresser_sem);
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