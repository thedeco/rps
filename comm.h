#include <sys/socket.h>
#include <stdint.h>
#include <linux/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int createSocket(uint16_t port);
int rps_send(int sockfd,struct sockaddr_in *clientaddr, char * message);
int rps_recv(int sockfd, struct sockaddr_in *clientaddr, char * dst_buffer, int recv_amount); 
