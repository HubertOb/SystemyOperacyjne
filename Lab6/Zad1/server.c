#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

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
    int msgid= msgget(key,0666|IPC_CREAT);
    printf("%d\n",key);
    printf("Klucz servera: %d\n",msgid);
    if (msgid==-1){
        return 0;
    }
    int current_id=0;
    printf("server tworzy kolejke\n");

    while (1==1){
        struct msg received,to_send;
        int stat;
        bool done=false;
        if(end_flag){
            for (int i=0;i<10;i++){
                if(clients[i]!=-1){
                    to_send.mtype=STOP;
                    printf("wysyłam clientom rozkaz stop\n");
                    msgsnd((int)clients_queue[i],&to_send, sizeof(to_send.mtext)+ sizeof(to_send.sender_id)+
                                                                      sizeof(to_send.receiver_id),0);
                }
            }
        }
        stat=msgrcv(msgid,&received,sizeof(received.mtext)+ sizeof(received.sender_id)+ sizeof(received.receiver_id),INIT,IPC_NOWAIT);
        if (stat!=-1){
            printf("Otrzymano sygnał init\n");
            clients_queue[current_id]=strtol(received.mtext,NULL,10);
            clients[current_id]=current_id;
            current_id++;
            clients_count++;

            snprintf(to_send.mtext,MSGMAX,"%d",clients[current_id-1]);
            printf("Wysyłam klientowi jego id\n");
            to_send.mtype=INIT;
            msgsnd((int)clients_queue[current_id-1],&to_send, sizeof(to_send.mtext)+ sizeof(to_send.sender_id)+
                                                              sizeof(to_send.receiver_id),0);
        }
        if (!done){
            stat=msgrcv(msgid,&received, sizeof(received.mtext)+ sizeof(received.sender_id)+ sizeof(received.receiver_id),STOP,IPC_NOWAIT);
            if (stat!=-1){
                printf("Otrzymano od klienta %d że kończy działanie\n",received.sender_id);
                time(&time1);
                string_time=ctime(&time1);
                fprintf(file,"Czas: %s, client_id: %d, content: %s\n",string_time,received.sender_id,received.mtext);
                for (int i=0;i<10;i++){
                    if(clients[i]==received.sender_id){
                        msgctl(clients_queue[i],IPC_RMID,NULL);
                        clients[i]=-1;
                        clients_count--;
                        done=true;
                    }
                }
            }
        }
        if(!done){
            stat=msgrcv(msgid,&received, sizeof(received.mtext)+ sizeof(received.sender_id)+ sizeof(received.receiver_id),LIST,IPC_NOWAIT);
            if (stat!=-1){
                printf("Otrzymano rozkaz wylistowania użytkowników\n");
                time(&time1);
                string_time=ctime(&time1);
                fprintf(file,"Czas: %s, client_id: %d, content: %s\n",string_time,received.sender_id,received.mtext);
                for (int j=0;j<10;j++){
                    if (clients[j]!=-1){
                        printf("Id klienta: %d\n",clients[j]);
                    }
                }
                done=true;
            }
        }
        if(!done){
            stat=msgrcv(msgid,&received, sizeof(received.mtext)+ sizeof(received.sender_id)+ sizeof(received.receiver_id),ALL2,IPC_NOWAIT);
            if (stat!=-1) {
                time(&time1);
                string_time = ctime(&time1);
                fprintf(file, "Czas: %s, client_id: %d, content: %s\n", string_time, received.sender_id,
                        received.mtext);
                    printf("Otrzymano rozkaz wysłania wiadomości do wszystkich\n");
                    for (int j = 0; j < 10; j++) {
                        if (clients[j] != -1) {
                            strcpy(to_send.mtext, received.mtext);
                            to_send.sender_id = received.sender_id;
                            to_send.mtype = ONE2;
                            printf("wysyłam wiadomośc do wszystkich\n");
                            msgsnd(clients_queue[j], &to_send, sizeof(to_send.mtext) + sizeof(to_send.sender_id) +
                                                               sizeof(to_send.receiver_id), 0);
                        }
                    }

            }
            stat=msgrcv(msgid,&received, sizeof(received.mtext)+ sizeof(received.sender_id)+ sizeof(received.receiver_id),ONE2,IPC_NOWAIT);
            if (stat!=-1){
                time(&time1);
                string_time = ctime(&time1);
                fprintf(file, "Czas: %s, client_id: %d, content: %s\n", string_time, received.sender_id,
                        received.mtext);
                    printf("Otrzymano rozkaz wysłania wiadomości do jednego użytkownika\n");
                    for (int j=0;j<10;j++){
                        if (clients[j]!=-1 && clients[j]==received.receiver_id){
                            strcpy(to_send.mtext,received.mtext);
                            to_send.sender_id=received.sender_id;
                            to_send.mtype=ONE2;
                            printf("wysyłam wiadomośc do jednego\n");
                            msgsnd(clients_queue[j],&to_send, sizeof(to_send.mtext)+ sizeof(to_send.sender_id)+
                                                              sizeof(to_send.receiver_id),0);
                        }
                    }

            }
            done=true;

        }
        if (clients_count==0 && end_flag){
            msgctl(msgid,IPC_RMID,NULL);
            printf("Kończe działanie\n");
            break;
        }

    }
    fclose(file);

    return 0;
}
