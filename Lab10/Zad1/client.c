#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/un.h>
#include "header.h"

bool flag=true;

void handler(int signum){
    flag=false;
}

int main(int argc, char*argv[]) {
    signal(SIGINT,handler);
    if(argc!=4){
        printf("Niepoprawna ilość danych wejściowych\n");
        return 0;
    }
    char*clientName=argv[1];
    char*connectionType=argv[2];
    if (strcmp(connectionType,"net")==0){
        char*ipv4AndPort=argv[3];
        char*ip;
        int port;
        ip= strtok(ipv4AndPort,":");
        port= strtol(strtok(NULL,":"),NULL,10);
        struct sockaddr_in server,client;
        server.sin_family=AF_INET;
        server.sin_port= htons(port);
        inet_pton(AF_INET,ip,&server.sin_addr);
        const int socket_=socket(AF_INET,SOCK_STREAM,0);
        socklen_t len= sizeof(server);
        connect(socket_, (struct sockaddr*)&server, len);
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        char buffer[256];
        sprintf(buffer,"%d%s",INIT,clientName);
        int result=send(socket_,buffer,strlen(buffer),0);
        if(result==-1){
            perror("send");
        }
        printf("Czekam na zaakceptowanie połączenia\n");
        recv(socket_,buffer,sizeof(buffer),0);
        printf("Zaakceptowano połączenie\n");
        int myID= atoi(&buffer[0]);
        printf("Moje ID: %d\n",myID);
        fcntl(socket_,F_SETFL,O_NONBLOCK);
        while(true){
            int a=recv(socket_,buffer,sizeof(buffer),0);
            if (a>=0){
                printf("Otrzymano wiadomość: %s\n",buffer);
            }
            char polecenie[10];
            printf("Wpisz polecenie: \n");
            scanf("%s",polecenie);
            if(strcmp(polecenie,"list")==0){
                printf("Wysyłam polecenie list\n");
                sprintf(buffer,"%d",LIST);
                send(socket_,buffer,strlen(buffer),0);
            }
            else if(strcmp(polecenie,"2all")==0){
                char wiadomosc[255];
                printf("Wpisz wiadomość: ");
                scanf("%s",wiadomosc);
                printf("wysyłam wiadomość do wszystkich\n");
                sprintf(buffer,"%d%s",ALL2,wiadomosc);
                send(socket_,buffer,strlen(buffer),0);
            }
            else if(strcmp(polecenie,"2one")==0){
                char wiadomosc[254];
                printf("Wpisz id clienta: ");
                int temp=0;
                scanf("%d",&temp);
                printf("Wpisz wiadomość: ");
                scanf("%s",wiadomosc);
                printf("Wysyłam wiadomość do jednej osoby\n");
                sprintf(buffer,"%d%d%s",ONE2,temp,wiadomosc);
                send(socket_,buffer,strlen(buffer),0);
            }
            else if(strcmp(polecenie,"stop")==0){
                printf("Otrzymano polecenie stop\n");
                printf("Wysyłam poledenie stop\n");
                sprintf(buffer,"%d%d",STOP,myID);
                send(socket_,buffer,strlen(buffer),0);
                break;
            }
            else{
                printf("złe polecenie\n");
            }
            if (!flag){
                printf("Otrzymano polecenie stop\n");
                printf("Wysyłam poledenie stop\n");
                sprintf(buffer,"%d%d",STOP,myID);
                send(socket_,buffer,strlen(buffer),0);
                break;
            }
        }
        recv(socket_,buffer,sizeof(buffer),0);
        printf("%s\n",buffer);
        shutdown(socket_,SHUT_RDWR);

    }
    else if(strcmp(connectionType,"local")==0){
        char *path=argv[3];
        const int socket_= socket(AF_UNIX,SOCK_STREAM,0);
        struct sockaddr_un server;
        server.sun_family=AF_UNIX;
        strcpy(server.sun_path,path);
        connect(socket_,(struct sockaddr*)&server, sizeof(server));
        char buffer[256];
        sprintf(buffer,"%d%s",INIT,clientName);
        int result=send(socket_,buffer,strlen(buffer),0);
        if(result==-1){
            perror("send");
        }
        printf("Czekam na zaakceptowanie połączenia\n");
        recv(socket_,buffer,sizeof(buffer),0);
        printf("Zaakceptowano połączenie\n");
        int myID= atoi(&buffer[0]);
        printf("Moje ID: %d\n",myID);
        fcntl(socket_,F_SETFL,O_NONBLOCK);
        while(true){
            int a=recv(socket_,buffer,sizeof(buffer),0);
            if (a>=0){
                printf("Otrzymano wiadomość: %s\n",buffer);
            }
            char polecenie[10];
            printf("Wpisz polecenie: \n");
            scanf("%s",polecenie);
            if(strcmp(polecenie,"list")==0){
                printf("Wysyłam polecenie list\n");
                sprintf(buffer,"%d",LIST);
                send(socket_,buffer,strlen(buffer),0);
            }
            else if(strcmp(polecenie,"2all")==0){
                char wiadomosc[255];
                printf("Wpisz wiadomość: ");
                scanf("%s",wiadomosc);
                printf("wysyłam wiadomość do wszystkich\n");
                sprintf(buffer,"%d%s",ALL2,wiadomosc);
                send(socket_,buffer,strlen(buffer),0);
            }
            else if(strcmp(polecenie,"2one")==0){
                char wiadomosc[254];
                printf("Wpisz id clienta: ");
                int temp=0;
                scanf("%d",&temp);
                printf("Wpisz wiadomość: ");
                scanf("%s",wiadomosc);
                printf("Wysyłam wiadomość do jednej osoby\n");
                sprintf(buffer,"%d%d%s",ONE2,temp,wiadomosc);
                send(socket_,buffer,strlen(buffer),0);
            }
            else if(strcmp(polecenie,"stop")==0){
                printf("Otrzymano polecenie stop\n");
                printf("Wysyłam poledenie stop\n");
                sprintf(buffer,"%d%d",STOP,myID);
                send(socket_,buffer,strlen(buffer),0);
                break;
            }
            else{
                printf("złe polecenie\n");
            }
            if (!flag){
                printf("Otrzymano polecenie stop\n");
                printf("Wysyłam poledenie stop\n");
                sprintf(buffer,"%d%d",STOP,myID);
                send(socket_,buffer,strlen(buffer),0);
                break;
            }
        }
        recv(socket_,buffer,sizeof(buffer),0);
        printf("%s\n",buffer);
        shutdown(socket_,SHUT_RDWR);
    }
    else{
        printf("Niepoprawny typ połączenia\n");
        return 0;
    }
    return 0;
}
