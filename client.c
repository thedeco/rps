#include "client.h"

/*
Initiate handshake with the server
*/
void sendHelloMessage(int sockfd, char * ipaddr, uint16_t port){
    char * recv_buffer;
    char ip[16];
    uint16_t tempport;
    char answer[10];

    printf("Connecting to server: %s, on port %hu\n", ipaddr, port);
    rps_send(sockfd,ipaddr, port, "Hello");
    recv_buffer = rps_recv(sockfd, ip, &tempport, 60); //60 Bytes to recv welcome msg from server
    printf("%s\n",recv_buffer); 
    recv_buffer = rps_recv(sockfd, ip, &tempport, 60); //60 Bytes to recv prompt msg from server
    printf("%s\n",recv_buffer); 
    recv_buffer = rps_recv(sockfd, ip, &tempport, 25);
    printf("%s",recv_buffer); 
    fgets(answer, 3, stdin);
    if(strncmp(answer, "Y", 1) == 0){
        rps_send(sockfd, ipaddr, port, answer);
        recv_buffer = rps_recv(sockfd, ip, &tempport, 50);
        printf("%s", recv_buffer);
        recv_buffer = rps_recv(sockfd, ip, &tempport, 12);
        printf("%s", recv_buffer);  
        fgets(answer, 10, stdin);
        rps_send(sockfd, ipaddr, port, answer);
        recv_buffer = rps_recv(sockfd, ip, &tempport, 40);
        printf("%s", recv_buffer);
        recv_buffer = rps_recv(sockfd, ip, &tempport, 40);
        printf("%s\n", recv_buffer);
    }   
    else if(strncmp(answer, "N", 1) == 0){
        rps_send(sockfd, ipaddr, port , answer);
        recv_buffer = rps_recv(sockfd, ip, &tempport, 50); //Get the stats from the server before closing
        printf("Goodbye\n"); //get stats
    }

    
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
   
    printf("-----------------------------------------------------------------\n");    
    printf("Starting Client Program\n");
    sockfd = createSocket(ipaddr, 0);
    sendHelloMessage(sockfd, ipaddr, port);
//    printf("Closing Socket #: %d\n", sockfd);   
    close(sockfd);
}

