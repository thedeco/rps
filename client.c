#include "client.h"

/*
Initiate handshake with the server
*/
void sendHelloMessage(int sockfd, char * ipaddr, uint16_t port){
    char * recv_buffer;

    //inet_ntop(AF_INET, &(*srvaddr.sin_addr.s_addr), srvip, 16);
    //printf("Connecting to: (%s:%hu)\n",srvip,ntohs(srvaddr.sin_port));
    rps_send(sockfd,ipaddr, port, "Hello");
    recv_buffer = rps_recv(sockfd, ipaddr, port, 5);
    printf("-------------------------------------------------------\n");    
    printf("Received the following:\n"); 
    printf("%s",recv_buffer); 
    

    /*
    printf("Entering Recv Loop...\n");
    for (;;){
        rps_recv(sockfd, ipaddr, port, recv_buffer, 4);
        printf("-------------------------------------------------------\n");    
        printf("Received the following:\n"); 
        printf("%s",recv_buffer); 
    }
    */
}

int main(int argc, char *argv[]){
    int sockfd, opt;
    struct sockaddr_in srvaddr;
    char *ipaddr, *buf;
    uint16_t port;
   
    ipaddr = "127.0.0.1";
    port = 0;

    while ((opt = getopt(argc, argv, "i:p:")) != -1) {
        switch (opt) {
            case 'i': //-i: server IP address
               if(inet_pton(AF_INET, optarg, &buf) == 1){  //inet_pton() returns 1 on success
                    memset(ipaddr, 0, 16);
                    snprintf(ipaddr, 16, "%s", optarg);
               }
                else{
                    fprintf(stderr, "Enter a valid IPv4 address\n");
                    return -1;
                }
                break;

            case 'p': 
               port = (uint16_t)atoi(optarg);
               if(port < 1024 || port > 65535){
                    fprintf(stderr, "Port must be a value between 1025 - 65534\n");
                    return -1;
                }
                break;
    
               default: /* 'Wrong usage fail' */
            fprintf(stderr, "Usage: %s [-i Game Server's IP Address] [-p Game Server's Port #]\n",argv[0]);
            return -1;
        }
    }
    


    printf("Starting Client Program\n");
    sockfd = createSocket(ipaddr, 0);
    sendHelloMessage(sockfd, ipaddr, port);
    printf("Closing Socket #: %d\n", sockfd);   
    close(sockfd);
}

