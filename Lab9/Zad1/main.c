#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

int waiting_deers=0;
int waiting_elves=0;

int elves_tab[3];
sem_t sem_elves[3];

sem_t sem_deers;
sem_t sem_end;

pthread_mutex_t waiting_deers_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waiting_elves_mutex=PTHREAD_MUTEX_INITIALIZER;

int wakeup_santa=0;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t waiting_cond=PTHREAD_COND_INITIALIZER;


void* santa(void* args){
    srand(time(NULL));
    int delivered_packages=0;

    while (delivered_packages<3){
        pthread_mutex_lock(&mutex);
        while(wakeup_santa!=1){
            pthread_cond_wait(&waiting_cond,&mutex);
        }
        wakeup_santa=0;
        pthread_mutex_unlock(&mutex);
        printf("Mikołaj: budzę się\n");
        pthread_mutex_lock(&waiting_elves_mutex);
        if(waiting_elves==3){
            sleep(rand()%2+1);
            printf("Mikołaj: rozwiązuję problemy elfów %d %d %d\n",elves_tab[0],elves_tab[1],elves_tab[2]);
            for(int i=0;i<3;i++){
                sem_post(&sem_elves[i]);
            }
            waiting_elves=0;
        }
        pthread_mutex_unlock(&waiting_elves_mutex);
        pthread_mutex_lock(&waiting_deers_mutex);
        if(waiting_deers==9){
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(rand()%3+2);
            delivered_packages++;
            waiting_deers=0;
            for(int i=0;i<9;i++){
                sem_post(&sem_deers);
            }
        }
        pthread_mutex_unlock(&waiting_deers_mutex);
        printf("Mikołaj: zasypiam\n");

    }
    for (int i=0;i<3;i++){
        sem_destroy(&sem_elves[i]);
    }
    sem_destroy(&sem_deers);
    sem_post(&sem_end);
}

void* deer(void* args){
    int id=(int) args;

    while (1){
        srand(time(NULL));
        int sleep_time=rand()%6+5;
        sleep(sleep_time); //są na wakacjach

        pthread_mutex_lock(&waiting_deers_mutex);
        waiting_deers++;
        printf("Renifer: czeka %d reniferów na mikołaja, %d\n",waiting_deers,id);
        if(waiting_deers==9){
            printf("Renifer: wybudzam mikołaja, %d\n",id);
            pthread_mutex_lock(&mutex);
            wakeup_santa=1;
            pthread_cond_broadcast(&waiting_cond);
            pthread_mutex_unlock(&mutex);

        }
        pthread_mutex_unlock(&waiting_deers_mutex);
        sem_wait(&sem_deers);
    }
}

void* elf(void* args){
    int id=(int) args;

    while(1){
        srand(time(NULL));
        int sleep_time=rand()%4+2;
        sleep(sleep_time); //pracują

        int flag=0;
        int index;
        pthread_mutex_lock(&waiting_elves_mutex);
        if(waiting_elves==3){
            printf("Elf: Samodzielnie rozwiązuję swój problem, %d\n",id);
            flag=1;
        }
        else if(waiting_elves<3){
            index=waiting_elves;
            elves_tab[index]=id;
            waiting_elves++;
            printf("Elf: Czeka %d elfów na mikołaja, %d\n",waiting_elves,id);
        }
        if (waiting_elves==3 && flag==0){
            printf("Elf: Wybudzam mikołaja, %d\n",id);
            pthread_mutex_lock(&mutex);
            wakeup_santa=1;
            pthread_cond_broadcast(&waiting_cond);
            pthread_mutex_unlock(&mutex);

        }
        pthread_mutex_unlock(&waiting_elves_mutex);
        if (flag==0){
            sem_wait(&sem_elves[index]);
            printf("Elf: mikołaj rozwiązuje problem, %d\n",id);
        }

    }
}

int main() {
    for (int i=0;i<3;i++){
        sem_init(&sem_elves[i],0,0);
    }
    sem_init(&sem_deers,0,0);
    sem_init(&sem_end,0,0);

    pthread_t santa_thread;
    pthread_create(&santa_thread,NULL,santa,NULL);

    pthread_t deer_threads[9];
    for(int i=0;i<9;i++){
        pthread_create(&deer_threads[i],NULL,deer,(void*) i);
    }

    pthread_t elf_threads[10];
    for (int i=0;i<10;i++){
        pthread_create(&elf_threads[i],NULL,elf,(void*) i);
    }
    sem_wait(&sem_end);
    return 0;
}
