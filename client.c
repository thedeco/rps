#include "client.h"

/*
Initiate handshake with the server
*/
void sendHelloMessage(int sockfd,struct sockaddr_in *srvaddr);
    char * recv_buffer;
    recv_buffer = (char *)malloc(4);

    inet_ntop(AF_INET, &(srvaddr.sin_addr.s_addr),srvip,16);
    printf("Connecting to: (%s:%hu)\n",srvip,ntohs(srvaddr.sin_port));
    rps_send(sockfd, 
    fprintf(stderr, "Entering Recv Loop...\n");
    for (;;){
        rps_recv(sockfd, srvaddr, recv_buffer, 4);
        printf("-------------------------------------------------------\n");    
        printf("Received the following:\n"); 
        printf("%s",recv_buffer); 
    }

void main(){
    int sockfd;
    struct sockaddr_in srvaddr;

    printf("Starting Client Program\n");
    sockfd = createSocket(0);
    sendHelloMessage(sockfd, &srvaddr);
    printf("Closing Socket #: %d\n", sockfd);   
    close(sockfd);
}

