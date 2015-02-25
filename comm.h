#include <sys/socket.h>
#include <stdint.h>
#include <linux/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int createSocket(char * ip, uint16_t port);

int rps_send(int sockfd,char * ip, uint16_t port, char * message);
int rps_recv(int sockfd,char *ip, uint16_t port, char * dst_buffer, int recv_amount); 
