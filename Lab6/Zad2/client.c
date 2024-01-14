#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <mqueue.h>

#include "header.h"

struct msg {
    long mtype;
    char mtext[MSGMAX];
    int sender_id;
    int receiver_id;
};

char data_to_send[50];
char data_received[50];
int my_id=-1;
mqd_t msgid;
mqd_t priv_msgid;
struct msg init,received_id,to_send,received;
bool to_break=false;

void handler(int signum){
    printf("wysyłam stop z handlera\n");
    to_send.mtype=STOP;
    to_send.sender_id=my_id;
    mq_send(msgid,data_to_send,sizeof(data_to_send),STOP);
    mq_close(priv_msgid);
    mq_unlink("/file_serv");
    mq_close(msgid);
    exit(0);
}


int main() {
    char* p=getenv("HOME");
    char* path=NULL;

    msgid= mq_open("/file_serv",O_WRONLY|O_CREAT,0666);
    if (msgid==-1){
        printf("nie udało się otworzyc kolejki servera\n");
        return 0;
    }
    printf("Klucz servera: %d\n",msgid);

    printf("tworzę kolejke\n");
    signal(SIGINT, handler);

    priv_msgid= mq_open("/file_client",O_RDONLY|O_CREAT,0666);

    printf("Klucz prywatny: %d\n",priv_msgid);

    init.mtype=INIT;
    snprintf(init.mtext,MSGMAX,"%d",priv_msgid);

    printf("Wysyłam sygnał init do servera\n");
    snprintf(data_to_send,48,"%d",priv_msgid);
    mq_send(msgid,data_to_send, sizeof(data_to_send),INIT);

    unsigned int priop;
    mq_receive(priv_msgid,data_received, sizeof(data_received),&priop);
    printf("Otrzymano moje id: \n");
    my_id=(int)strtol(data_received,NULL,10);
    to_send.sender_id=my_id;

    char polecenie[4];
    char wiadomosc[MSGMAX];
    to_send.sender_id=my_id;
    while (1){
        int stat=mq_receive(priv_msgid,data_received, sizeof(data_received),&priop);
        if (stat!=-1){
            if (priop==STOP){
                printf("Otrzymano wiadomość stop \n");
                to_send.mtype=STOP;
                printf("Wysyłam stop\n");
                mq_send(msgid,data_to_send, sizeof(data_to_send),STOP);
                mq_close(priv_msgid);
                mq_unlink("/file_client");
                mq_close(msgid);
                break;
            }
            else {
                printf("Otrzymano wiadomość: %s\n",data_received);
            }
            continue;
        }

        if(to_break){
            break;
        }
        printf("Podaj polecenie: ");
        scanf("%s",polecenie);
        if(strcmp(polecenie,"list")==0){
            printf("Wysyłam polecenie list\n");
            mq_send(msgid,data_to_send, sizeof(data_to_send),LIST);
        }
        else if(strcmp(polecenie,"2all")==0){
            printf("Wpisz wiadomość: ");
            scanf("%s",wiadomosc);
            strcpy(data_to_send,wiadomosc);
            printf("wysyłam wiadomość do wszystkich\n");
            mq_send(msgid,data_to_send, sizeof(data_to_send),ALL2);
        }
        else if(strcmp(polecenie,"2one")==0){
            printf("Wpisz id clienta: ");
            int temp=0;
            scanf("%d",&temp);
            printf("Wpisz wiadomość: ");
            scanf("%s",wiadomosc);
            strcpy(data_to_send,wiadomosc);
            data_to_send[48]=temp;
            printf("Wysyłam wiadomość do jednej osoby\n");
            mq_send(msgid,data_to_send, sizeof(data_to_send),ONE2);
        }
        else if(strcmp(polecenie,"stop")==0){
            printf("Otrzymano polecenie stop\n");
            printf("Wysyłam poledenie stop\n");
            data_to_send[49]=my_id;
            mq_send(msgid,data_to_send, sizeof(data_to_send),STOP);
            mq_close(priv_msgid);
            mq_unlink("/file_client");
            mq_close(msgid);
            break;
        }
        else{
            printf("złe polecenie\n");
        }
    }

    return 0;
}
