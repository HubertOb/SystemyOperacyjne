#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "header.h"

struct msg {
    long mtype;
    char mtext[MSGMAX];
    int sender_id;
    int receiver_id;
};

int my_id=-1;
int msgid;
int priv_msgid;
struct msg init,received_id,to_send,received;
bool to_break=false;

void handler(int signum){
    printf("wysyłam stop z handlera\n");
    to_send.mtype=STOP;
    to_send.sender_id=my_id;
    msgsnd(msgid,&to_send, sizeof(to_send.mtext)+ sizeof(to_send.receiver_id)+ sizeof(to_send.sender_id),0);
    msgctl(priv_msgid,IPC_RMID,NULL);
    exit(0);
}


int main() {
    char* p=getenv("HOME");
    char* path=NULL;
    key_t key=ftok(p,CONST_INT_SERV);
    printf("%d\n",key);
    msgid= msgget(key,0666|IPC_CREAT);
    if (msgid==-1){
        return 0;
    }
    printf("Klucz servera: %d\n",msgid);

    printf("tworzę kolejke\n");
    signal(SIGINT, handler);
    key_t priv_key=ftok(p,CONST_INT_CLIENT);
    printf("%d\n",priv_key);
    priv_msgid= msgget(priv_key,0666|IPC_CREAT);

    printf("Klucz prywatny: %d\n",priv_msgid);


    init.mtype=INIT;
    snprintf(init.mtext,MSGMAX,"%d",priv_msgid);

    printf("Wysyłam sygnał init do servera\n");
    msgsnd(msgid,&init, sizeof(init.mtext)+ sizeof(init.receiver_id)+ sizeof(init.sender_id),0);

    msgrcv(priv_msgid,&received_id, sizeof(received_id.mtext)+sizeof(received_id.receiver_id)+ sizeof(received_id.sender_id),INIT,0);
    printf("Otrzymano moje id: \n");
    my_id=(int)strtol(received_id.mtext,NULL,10);
    to_send.sender_id=my_id;

    char polecenie[4];
    char wiadomosc[MSGMAX];
    to_send.sender_id=my_id;
    while (1){
        int stat=msgrcv(priv_msgid,&received,sizeof(received.receiver_id)+ sizeof(received.sender_id)+ sizeof(received.mtext),STOP,IPC_NOWAIT);
        if (stat!=-1){
            if (received.mtype==STOP){
                printf("Otrzymano wiadomość stop \n");
                to_send.mtype=STOP;
                printf("Wysyłam stop\n");
                msgsnd(msgid,&to_send, sizeof(to_send.mtext)+ sizeof(to_send.receiver_id)+ sizeof(to_send.sender_id),0);
                msgctl(priv_msgid,IPC_RMID,NULL);
                break;
            }
            else{
                printf("Otrzymano wiadomość: %s\n",received.mtext);
            }
            continue;
        }
        stat=msgrcv(priv_msgid,&received,sizeof(received.receiver_id)+ sizeof(received.sender_id)+ sizeof(received.mtext),ONE2,IPC_NOWAIT);
        if (stat!=-1){
            printf("Otrzymano wiadomość: %s\n",received.mtext);
        }
        if(to_break){
            break;
        }
        printf("Podaj polecenie: ");
        scanf("%s",polecenie);
        if(strcmp(polecenie,"list")==0){
            printf("Wysyłam polecenie list\n");
            to_send.mtype=LIST;
            msgsnd(msgid,&to_send, sizeof(to_send.mtext)+ sizeof(to_send.receiver_id)+ sizeof(to_send.sender_id),0);
        }
        else if(strcmp(polecenie,"2all")==0){
            printf("Wpisz wiadomość: ");
            scanf("%s",wiadomosc);
            to_send.mtype=ALL2;
            strcpy(to_send.mtext,wiadomosc);
            printf("wysyłam wiadomość do wszystkich\n");
            msgsnd(msgid,&to_send, sizeof(to_send.mtext)+ sizeof(to_send.receiver_id)+ sizeof(to_send.sender_id),0);
        }
        else if(strcmp(polecenie,"2one")==0){
            printf("Wppisz id clienta: ");
            scanf("%d",&to_send.receiver_id);
            printf("Wpisz wiadomość: ");
            scanf("%s",wiadomosc);
            to_send.mtype=ONE2;
            strcpy(to_send.mtext,wiadomosc);
            printf("Wysyłam wiadomość do jednej osoby\n");
            msgsnd(msgid,&to_send, sizeof(to_send.mtext)+ sizeof(to_send.receiver_id)+ sizeof(to_send.sender_id),0);
        }
        else if(strcmp(polecenie,"stop")==0){
            printf("Otrzymano polecenie stop\n");
            to_send.mtype=STOP;
            printf("Wysyłam poledenie stop\n");
            msgsnd(msgid,&to_send, sizeof(to_send.mtext)+ sizeof(to_send.receiver_id)+ sizeof(to_send.sender_id),0);
            msgctl(priv_msgid,IPC_RMID,NULL);
            break;
        }
        else{
            printf("złe polecenie\n");
        }
    }

    return 0;
}
