#include "server.h"
//getstats
 //printf("ties: %d wins: %d losses: %d\n", ties,wins,losses);
    
/*
Game result lookup table used by the return value of the playgame function
*/
int rules[3][3] = {{USER_TIE, USER_WIN, USER_LOSS},
               {USER_LOSS, USER_TIE, USER_WIN},
               {USER_WIN, USER_LOSS, USER_TIE}};
/*
The game takes user input converts it to lowercase, generates the server's move and returns the result of the user's input vs the server's
*/
int playgame(int sockfd, char *ip, uint16_t port){
    int servaction,useraction,i;
    char * userinput;
 
    rps_send(sockfd, ip, port,"Select: <ROCK>, <PAPER>, or <SCISSOR>\n");
    rps_send(sockfd, ip, port,"Choice: ");
    userinput = rps_recv(sockfd, ip, &port, 8);
    printf("User Picked: %s", userinput);

    //Change user input to all lowercase
    for(i = 0; userinput[i]; i++){
        userinput[i] = tolower(userinput[i]); 
    }
 
    if((strncmp("rock",userinput,strlen(userinput)-1) == 0))
        useraction = MOVE_ROCK;
    else if((strncmp("paper",userinput,strlen(userinput)-1) == 0))
        useraction = MOVE_PAPER;
    else if((strncmp("scissor",userinput,strlen(userinput)-1) ==0))
        useraction = MOVE_SCISSOR;
    else{
        printf("Invalid move!\n");
        rps_send(sockfd, ip, port, "Invalid Move!\n");
        rps_send(sockfd, ip, port, "Valid Moves: <Rock>, <Paper>, <Scissor>\n"); 
        return INVALIDPLAY;
    }
    
    srand(time(NULL)); 
    servaction = (rand()%3);
    if(servaction == MOVE_ROCK){
        printf("Server picked: Rock\n");
        rps_send(sockfd, ip, port,"Server picked Rock\n");
    }
    else if(servaction == MOVE_PAPER){
       printf("Server picked: Paper\n");
       rps_send(sockfd, ip, port,"Select: <ROCK>, <PAPER>, or <SCISSOR>\n"); 
    }
    else {
        printf("Server picked: Scissor\n");
        rps_send(sockfd, ip, port,"Select: <ROCK>, <PAPER>, or <SCISSOR>\n");
    }

    if(rules[servaction][useraction] == 0){
        printf("Tied Game!\n");
        rps_send(sockfd, ip, port, "Tied Game!");
    }
    else if(rules[servaction][useraction] == 1){ 
        printf("User wins!\n");
        rps_send(sockfd, ip, port, "You Win!");
    }
    else if(rules[servaction][useraction] == -1){ 
        printf("Server wins!\n");
        rps_send(sockfd, ip, port, "You Lose!");
    } 
   
    return rules[servaction][useraction];
}

/*
Sends the new user a welcome message and prompts if the user wants to play returns 1 for new game and 2 for goodbye message
*/
int sendwelcomemsg(int sockfd, char * ip, uint16_t port){
    char * recv_buffer;
    printf("Sending Welcome Message to: %s on port: %hu\n", ip, port);
    rps_send(sockfd,ip,port,"Welcome to my rock paper scissor server!");  
    printf("Sending Game Prompt to: %s on port: %hu\n", ip, port);
    rps_send(sockfd,ip,port,"Want to play a game?");
    rps_send(sockfd,ip,port,"(Y/N): ");
    recv_buffer = rps_recv(sockfd, ip, &port,1);
    if(*recv_buffer == 'Y')
        printf("Starting new game!, Waiting for User's move...\n");
        playgame(sockfd, ip, port);
//    printf("User answer: %s\n", recv_buffer);

}

/*
Recieve from loop
*/
int recvinfo(int sockfd){
    struct sockaddr_in clientaddr;
    char * recv_buffer;
    char ip[16];
    uint16_t port;

    memset(ip,0,16);   
 
    //Handshake
    recv_buffer = rps_recv(sockfd, ip, &port, 6);
    printf("New Message from: %s:%hu -> %s\n",ip,port ,recv_buffer);
    if(strncmp(recv_buffer, "Hello", 5)== 0){
        sendwelcomemsg(sockfd, ip, port);
    }
    else
        printf("other");
/*
//Recvloop-------------------------------------------------------------------------------------
    fprintf(stderr, "Server is waiting for new data...\n");
    for (;;){
        rps_recv(sockfd, ip,port , recv_buffer,4);
        printf("%s\n", recv_buffer);
       //---------------------------------------------------------------------------------------------
    }
} 
*/
}

/*
Adds a user to the linked list and updates the game struct with that new user
*/
void adduser(struct gameinfo * game,uint32_t ip){
	struct user * newuser;
	newuser = (struct user *)malloc(sizeof(struct user));
	
    newuser->ipaddr = ip;
    newuser->wins = 0;
    newuser->ties = 0;
    newuser->losses = 0;
	newuser->next = NULL;
	game->n++;

	if(game->first == NULL){
		game->first = newuser;
		game->last = newuser;
		newuser->prev = NULL;	 	
	}
	else{
		game->last->next = newuser;
		newuser->prev = game->last;
		game->last = newuser;
	}
}

/*
Instantiates a new empty game struct
*/
void create(struct gameinfo * game){
   game->first = NULL;
   game->last = NULL;
   game->n = 0;
}

/*
Cleans up the server's open connections.
*/
int cleanup(int sockfd){
    printf("Closing Socket #: %d\n", sockfd);   
    close(sockfd);
}

int main(int argc, char *argv[]){
    struct gameinfo game;
    int sockfd, result, opt;
    char * ipaddr,buf;
    uint16_t port;

    ipaddr = "127.0.0.1";
    port = 1337;
    //if(argc < 2 || argc > 2){
     //   fprintf(stderr, "Usage: %s [-i IP Address] [-p Port #]\n",argv[0]);
      //  return -1;
   // }
 
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
               port = atoi(optarg);
               if(port < 1024 || port > 65535){
                    fprintf(stderr, "Port must be a value between 1025 - 65534\n");
                    return -1;
                }
                break;
    
               default: /* 'Wrong usage fail' */
            fprintf(stderr, "Usage: %s [-i IP Address] [-p Port #]\n",argv[0]);
            return -1;
        }
    }
 

    printf("-----------------------------------------------------------------\n");     
    printf("Starting Game Server\n");
    create(&game);
    sockfd = createSocket(ipaddr, port);
    recvinfo(sockfd);
    //result = playgame();
    //printf("%d\n",result);
    //printf("Play Again <Y/N>: ");
    //fgets(userinput, sizeof(userinput), stdin);
    //if(strncmp("N", userinput, strlen(userinput)-1) == 0)
      //  return 0;
    //if(strncmp("Y", userinput, strlen(userinput)-1) == 0)
      //  playgame();
 
    return 0;  
  }

