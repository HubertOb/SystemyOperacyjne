#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>

#include "header.h"



struct msg {
    long mtype;
    char mtext[MSGMAX];
    int sender_id;
    int receiver_id;
};
bool end_flag=false;

void handler(int signum){
    printf("Otrzymano sygnał zakończenia\n");
    end_flag=true;
}

int main() {
    int clients[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    long clients_queue[10];
    int clients_count=0;

    signal(SIGINT,handler);
    FILE *file= fopen("./logs","w");

    time_t time1;
    char* string_time;


    char* p=getenv("HOME");
    char *path = NULL;

    key_t key=ftok(p,CONST_INT_SERV);
    mqd_t msgid= mq_open("/file_serv",O_RDONLY|O_CREAT,0666);
    printf("Klucz servera: %d\n",msgid);
    if (msgid==-1){
        return 0;
    }
    int current_id=0;
    printf("server tworzy kolejke\n");

    char data_to_send[50];
    char data_received[50];
    unsigned int priop;
    while (1==1){
        priop=0;
        int stat;
        if(end_flag) {
            for (int i = 0; i < 10; i++) {
                if (clients[i] != -1) {
                    printf("wysyłam clientom rozkaz stop\n");
                    mq_send(clients_queue[i],data_to_send, sizeof(data_to_send),STOP);
                }
            }
        }
        mq_receive(msgid,data_received, sizeof(data_received),&priop);
        int client_id=data_received[49];
        int destination_id=data_received[48];
        data_received[49]=0;
        data_received[48]=0;
        if (priop==INIT){
            printf("Otrzymano sygnał init\n");
            clients_queue[current_id]=strtol(data_received,NULL,10);
            clients[current_id]=current_id;
            current_id++;
            clients_count++;

            snprintf(data_to_send,48,"%d",clients[current_id-1]);
            printf("Wysyłam klientowi jego id\n");
            mq_send(clients_queue[current_id-1],data_to_send, sizeof(data_to_send),INIT);
        }
        else if(priop==STOP){
            printf("Otrzymano od klienta %d że kończy działanie\n",data_received[49]);
            time(&time1);
            string_time=ctime(&time1);
            fprintf(file,"Czas: %s, client_id: %d, content: %s\n",string_time,client_id,data_received);
            for (int i=0;i<10;i++){
                if(clients[i]==client_id){
                    msgctl(clients_queue[i],IPC_RMID,NULL);
                    mq_close(clients_queue[i]);
                    clients[i]=-1;
                    clients_count--;
                }
            }
        }
        else if(priop==LIST){
            printf("Otrzymano rozkaz wylistowania użytkowników\n");
            time(&time1);
            string_time=ctime(&time1);
            fprintf(file,"Czas: %s, client_id: %d, content: %s\n",string_time,client_id,data_received);
            for (int j=0;j<10;j++){
                if (clients[j]!=-1){
                    printf("Id klienta: %d\n",clients[j]);
                }
            }
        }
        else if(priop==ALL2){
            time(&time1);
            string_time = ctime(&time1);
            fprintf(file, "Czas: %s, client_id: %d, content: %s\n", string_time, client_id, data_received);
            printf("Otrzymano rozkaz wysłania wiadomości do wszystkich\n");
            for (int j = 0; j < 10; j++) {
                if (clients[j] != -1) {
                    strcpy(data_to_send, data_received);
                    printf("wysyłam wiadomośc do wszystkich\n");
                    mq_send(clients_queue[j],data_to_send,sizeof(data_to_send),ALL2);
                }
            }
        }
        else if(priop==ONE2){
            time(&time1);
            string_time = ctime(&time1);
            fprintf(file, "Czas: %s, client_id: %d, content: %s\n", string_time, client_id, data_received);
            printf("Otrzymano rozkaz wysłania wiadomości do jednego użytkownika\n");
            for (int j=0;j<10;j++){
                if (clients[j]!=-1 && clients[j]==destination_id){
                    strcpy(data_to_send,data_received);
                    printf("wysyłam wiadomośc do jednego\n");
                    mq_send(clients_queue[j],data_to_send, sizeof(data_to_send),ONE2);
                }
            }
        }
        if (clients_count==0 && end_flag){
            msgctl(msgid,IPC_RMID,NULL);
            mq_close(msgid);
            mq_unlink("/file_serv");
            printf("Kończe działanie\n");
            break;
        }
    }
    fclose(file);

    return 0;
}
