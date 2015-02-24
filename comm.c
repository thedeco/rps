#include "comm.h"

/*
Creates socket (sockfd)
Returns the socket file descriptor or -1 on error.
*/
int createSocket(uint16_t port){
    int sockfd, bindRetCode;
    struct sockaddr_in srvaddr;

//-------------------------------------------------------------------
//Create the Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1){
        perror("socket");
        return -1;
    }
    printf("Socket Created. File Descriptor #:%d\n", sockfd);
 
//-------------------------------------------------------------------
//Populate the srvaddr struct with Family,Port and IP addr
    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET; //IPv4
    srvaddr.sin_port = htons(port);
    srvaddr.sin_addr.s_addr = INADDR_ANY;

//-------------------------------------------------------------------
//Bind to the Socket
    bindRetCode = bind(sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr));        
    if(bindRetCode !=0){
        perror("bind");
        return -1;
    }
    printf("Bound to Port: %hu\n", port);

//-------------------------------------------------------------------
//sockfd is the file descriptor for the newly created socket
    return sockfd;
}

/*
Send to address in clientaddr given a socket file descriptor, clientaddr sockaddr_in struct, and a pointer to the message to be sent
*/
int rps_send(int sockfd,struct sockaddr_in *sendaddr, char * message){
    int bytes_sent;
    bytes_sent = sendto(sockfd,message,strlen(message),0,(struct sockaddr *)sendaddr,sizeof(:wq
*clientaddr));
    if(bytes_sent == -1){
        perror("sendto");
        return -1;
    }
    return bytes_sent;
}

/*
Recieves from the address in clientaddr given a socket file descriptor, clientaddr sockaddr_in struct, and the amount to recieve
returns a pointer to the buffer where the information will be stored
*/
int rps_recv(int sockfd, struct sockaddr_in *recvaddr,char * dst_buffer, int recv_amount){
    socklen_t len;
    len = sizeof(*recvaddr);
    dst_buffer = (char *)malloc(recv_amount);
    recvfrom(sockfd,dst_buffer,recv_amount,0,(struct sockaddr *)recvaddr,&len);
    return 0;
}
