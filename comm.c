#include "comm.h"

/*
Creates socket (sockfd)
Returns the socket file descriptor or -1 on error.
*/
int createSocket(char * ip, uint16_t port){
    int sockfd, bindRetCode;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
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
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; //IPv4
    addr.sin_port = htons(port);
    //srvaddr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);

//-------------------------------------------------------------------
//Bind to the Socket
    bindRetCode = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));        
    if(bindRetCode !=0){
        perror("bind");
        return -1;
    }
 if (getsockname(sockfd, (struct sockaddr *)&addr, &len) == -1)    
    perror("getsockname");
 else
    printf("Bound to IP: %s\n", ip);
    printf("Bound to Port: %d\n", ntohs(addr.sin_port));
    printf("-----------------------------------------------------------------\n");    
    
//-------------------------------------------------------------------
//sockfd is the file descriptor for the newly created socket
    return sockfd;
}

/*
Send to address in clientaddr given a socket file descriptor, clientaddr sockaddr_in struct, and a pointer to the message to be sent
*/
int rps_send(int sockfd,char * ip, uint16_t port, char * message){
    int bytes_sent;
    struct sockaddr_in sendaddr;

    memset(&sendaddr, 0, sizeof(sendaddr));
    sendaddr.sin_family = AF_INET;
    sendaddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &sendaddr.sin_addr.s_addr);
    
    bytes_sent = sendto(sockfd,message,strlen(message),0,(struct sockaddr *)&sendaddr,sizeof(sendaddr));
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
char *rps_recv(int sockfd,char * ip, uint16_t * port, int recv_amount){
    struct sockaddr_in recvaddr;
    char * dst_buffer;
   
    memset(&recvaddr, 0, sizeof(recvaddr));
    socklen_t len = sizeof(recvaddr);
  //  recvaddr.sin_family = AF_INET;
    //recvaddr.sin_port = htons(port);
   // recvaddr.sin_addr.s_addr =  inet_addr(ip);

    dst_buffer = (char *)malloc(recv_amount);
    recvfrom(sockfd,dst_buffer,recv_amount,0,(struct sockaddr *)&recvaddr,&len);
    (*port) = ntohs(recvaddr.sin_port);
    inet_ntop(AF_INET,&(recvaddr.sin_addr),ip, 16);
 
    return dst_buffer;
}
