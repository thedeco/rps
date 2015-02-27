#include "client.h"


void getmove(char * answer){
    char move[10];
    int i;
    int flag=0;

    while (flag == 0){
        fgets(move, 8, stdin);
        for(i=0; move[i]; i++){
            move[i] = tolower(move[i]);
        }
        if(strncmp(move, "rock", strlen(move)-1) == 0){
            memcpy(answer, move, 4);
            flag = 1;
        }
        else if(strncmp(move, "paper", strlen(move)-1) == 0){
            memcpy(answer, move, 5);
            flag = 1;
        }
        else if(strncmp(move, "scissor", strlen(move)-1) == 0){
            memcpy(answer, move, 7);
            flag = 1;
        }
        else{
            printf("Incorrect move please try again\n");
            printf("Choice: ");
        }
    }
}


/*
Initiate handshake with the server
*/
void sendHelloMessage(int sockfd, char * ipaddr, uint16_t port){
    char * recv_buffer;
    char ip[16];
    uint16_t tempport;
    char answer[10];
    int playflag = 1, i;

//Sends Hello Message to server and gets response & prints it--------------
    printf("Connecting to server: %s, on port %hu\n", ipaddr, port);
    printf("-------------------------------------------------------------------\n");
    rps_send(sockfd,ipaddr, port, "Hello");
    recv_buffer = rps_recv(sockfd, ip, &tempport, 60);  //Welcome Msg
    printf("%s\n",recv_buffer); 
//-------------------------------------------------------------------------

    recv_buffer = rps_recv(sockfd, ip, &tempport, 60);  //Game Prompt
    printf("%s\n",recv_buffer); 
    recv_buffer = rps_recv(sockfd, ip, &tempport, 25);  //Choice: 
    printf("%s",recv_buffer); 
//-------------------------------------------------------------------------
    while(playflag !=0){
       fgets(answer, 3, stdin);                              // Gets the user's Y or N input
       for(i =0; answer[i]; i++){
           answer[i] = tolower(answer[i]);
       }
       if(*answer == 'y'){
           rps_send(sockfd, ipaddr, port, answer);            // Send user's answer to server
           recv_buffer = rps_recv(sockfd, ip, &tempport, 50); // Server's response of allowable moves
           printf("%s", recv_buffer);                         
           recv_buffer = rps_recv(sockfd, ip, &tempport, 12); // Sever's prompt
           printf("%s", recv_buffer);  
           getmove(answer);                        // User's move
           rps_send(sockfd, ipaddr, port, answer);            // Send's user's move to server
           recv_buffer = rps_recv(sockfd, ip, &tempport, 40); // Server's Move
           printf("%s", recv_buffer);
           recv_buffer = rps_recv(sockfd, ip, &tempport, 40); // Result of Game
           printf("%s\n", recv_buffer);
           printf("-------------------------------------------------------------------\n");
           recv_buffer = rps_recv(sockfd, ip, &tempport, 40); // Server asks if user wants to continue to play?
           printf("%s\n", recv_buffer);
           recv_buffer = rps_recv(sockfd, ip, &tempport, 40); // Server send choice: 
           printf("%s", recv_buffer);             
       }   
       else if(*answer == 'n'){
           rps_send(sockfd, ipaddr, port , answer);
           recv_buffer = rps_recv(sockfd, ip, &tempport, 50); //Get the stats from the server before closing
           printf("Goodbye\n"); //get stats
           playflag =0;
       }
       else {
           printf("Invalid Choice, Please select Y/N: ");
       }
    }
}

int main(int argc, char *argv[]){
    int sockfd, opt;
    char ipaddr[16] = "127.0.0.1";
    char  buf[16];
    uint16_t port = 0;

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

