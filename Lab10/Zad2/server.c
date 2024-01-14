#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include "header.h"

int main(int argc, char*argv[]) {
    if(argc!=3){
        printf("Niepoprawna ilość danych wejściowych\n");
        return 0;
    }
    int port=strtol(argv[1],NULL,10);
    char*path=argv[2];

    struct sockaddr_in netSocket,client;
    struct sockaddr_un server;

    netSocket.sin_family=AF_INET;
    netSocket.sin_port= htons(port);
    netSocket.sin_addr.s_addr= htons(INADDR_ANY);

    server.sun_family=AF_UNIX;
    strcpy(server.sun_path,path);

    int unixDesc,netDesc;

    inet_pton(AF_INET,"127.0.0.1",&netSocket.sin_addr);
    const int servSocket=socket(AF_INET,SOCK_DGRAM,0);
    const int unixServSocket=socket(AF_UNIX,SOCK_DGRAM,0);

    bind(servSocket,(struct sockaddr *)&netSocket,sizeof(netSocket));
    bind(unixServSocket,(struct sockaddr*)&server,sizeof(server));
    socklen_t len=sizeof(netSocket);

    char buffer[256];

    char *clientNames[15];
    int clientsID[15]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int clientsSockets[15];
    struct sockaddr_in clients[15];
    int numberOfClients=0;
    fcntl(servSocket,F_SETFL,O_NONBLOCK);
    fcntl(unixServSocket,F_SETFL,O_NONBLOCK);
    int epollFd= epoll_create1(0);
    struct epoll_event event;
    struct epoll_event events[17];

    event.events=EPOLLIN;
    event.data.fd=unixServSocket;
    epoll_ctl(epollFd,EPOLL_CTL_ADD,unixServSocket,&event);

    event.events=EPOLLIN;
    event.data.fd=servSocket;
    epoll_ctl(epollFd,EPOLL_CTL_ADD,servSocket,&event);

    while(true){
        int clientSocket;
        int countOfEvents= epoll_wait(epollFd,events,17,-1);
        for(int i=0;i<countOfEvents;i++){
            printf("for loop\n");
            if(events[i].data.fd==unixServSocket){
                printf("unixData\n");
                recvfrom(unixServSocket,buffer, sizeof(buffer),0,( struct sockaddr * ) & client, & len );
            }
            else if(events[i].data.fd==servSocket){
                printf("netData\n");
                recvfrom(servSocket,buffer, sizeof(buffer),0,( struct sockaddr * ) & client, & len );
            }
            else{
                printf("cosik inne\n");
                recv(events[i].data.fd,buffer, sizeof(buffer),0);
            }
            sleep(1);
            char a=buffer[0];
            int polecenie=atoi(&a);
            if(polecenie==INIT){
                printf("Otrzymano polecenie INIT\n");
                char name[256];
                strncpy(name,buffer+1,255);
                if(numberOfClients<=15){
                    for (int i=0;i<15;i++){
                        if(clientsID[i]==-1){
                            clientsID[i]=i;
                            clientsSockets[i]=clientSocket;
                            clients[i]=client;
                            event.events=EPOLLIN;
                            event.data.fd=clientSocket;
                            epoll_ctl(epollFd,EPOLL_CTL_ADD,clientSocket,&event);
                            clientNames[i]=name;
                            numberOfClients++;
                            sprintf(buffer,"%d",clientsID[i]);
                            sendto( servSocket, buffer, strlen( buffer ), 0,( struct sockaddr * ) & clients[i], len );
                            break;
                        }
                    }
                }
            }
            else if(polecenie==LIST){
                printf("Otrzymano rozkaz wylistowania użytkowników\n");
                for (int j=0;j<15;j++){
                    if (clientsID[j]!=-1){
                        printf("Id klienta: %d, nazwa klienta: %s\n",clientsID[j],clientNames[j]);
                    }
                }
            }
            else if(polecenie==ALL2){
                printf("Otrzymano rozkaz wysłania do wszystkich\n");
                char wiadomosc[255];
                strncpy(wiadomosc,buffer+1,255);
                sprintf(buffer,"%s",wiadomosc);
                for(int i=0;i<15;i++){
                    if (clientsID[i]!=-1){
                        sendto( servSocket, buffer, strlen( buffer ), 0,( struct sockaddr * ) & clients[i], len );
                    }
                }
            }
            else if(polecenie==ONE2){
                printf("Otrzymano rozkaz wysłania do jednej\n");
                char wiadomosc[254];
                int receiverID= atoi(&buffer[1]);
                strncpy(wiadomosc,buffer+2,254);
                for(int i=0;i<15;i++){
                    if(clientsID[i]==receiverID){
                        sprintf(buffer,"%s",wiadomosc);
                        sendto( servSocket, buffer, strlen( buffer ), 0,( struct sockaddr * ) & clients[i], len );
                        break;
                    }
                }
            }
            else if(polecenie==STOP){
                printf("Otrzymano koniec pracy klienta\n");
                int clientid=atoi(&buffer[1]);
                printf("Rozłączanie klienta o id: %d\n",clientid);
                for(int i=0;i<15;i++){
                    if(clientsID[i]==clientid){
                        epoll_ctl(epollFd,EPOLL_CTL_DEL,clientsSockets[i],&event);
                        shutdown(clientsSockets[i],SHUT_RDWR);
                        close(clientsSockets[i]);
                        clientsID[i]=-1;
                        break;
                    }
                }
            }
            polecenie=-1;
            buffer[0]='9';
        }
    }
    shutdown(servSocket,SHUT_RDWR);
    close(servSocket);

    return 0;
}
