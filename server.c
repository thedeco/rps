#include "server.h"
//getstats
//printf("ties: %d wins: %d losses: %d\n", ties,wins,losses)

//returns -1 if ther user doesn't exit or the index of the user
int playersearch(struct gameinfo * game, char * ip){
	int i;
	struct user * currentuser;
	currentuser = game-> first;
	for(i =0; i<game->n; i++){
		if(strncmp(currentuser->ipaddr, ip,strlen(ip)) == 0)
			return i;
		else{
			currentuser = currentuser->next;
		}
	}
	return -1;
}

/*
Instantiates a new empty game struct
*/
void gamecreate(struct gameinfo * game){
   game->first = NULL;
   game->last = NULL;
   game->n = 0;
}

/*
Adds a user to the linked list and updates the game struct with that new user
*/
void playeradd(struct gameinfo * game,char * ip){
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

//return 1 for valid input 0 for bad input
int validateuserinput(int sockfd, char * ip, uint16_t port,int type, char * userinput,int *useraction){
    int flag = 0,i;
    
    //Change user input to all lowercase
    for(i = 0; userinput[i]; i++){
        userinput[i] = tolower(userinput[i]); 
    }

    //Type 1 is a yes/no message
    if(type == 1){
        if(*userinput == 'y'){
            printf("User said Yes! Starting new game!\n");
            return 1;
        }
        else if(*userinput == 'n'){
            printf("User does not want to play\n");
            rps_send(sockfd, ip, port, "Goodbye!");
            return 1;
        }
        else {
            printf("Invalid Y/N selection\n");
            rps_send(sockfd, ip, port, "Invalid Y/N Selection!\n");
            rps_send(sockfd, ip, port, "Please select <Y/N>: "); 
            return 0;
        }
    }

    //Type 2 is a User move message
    else if (type == 2){
        if((strncmp("rock",userinput,strlen(userinput)-1) == 0)){
            *useraction = MOVE_ROCK;
            return 1;
        }
        else if((strncmp("paper",userinput,strlen(userinput)-1) == 0)){
            *useraction = MOVE_PAPER;
            return 1; 
        }
        else if((strncmp("scissor",userinput,strlen(userinput)-1) ==0)){
            *useraction = MOVE_SCISSOR;
            return 1;
        }
        else{
            printf("Invalid move!\n");
            rps_send(sockfd, ip, port, "Invalid Move!\n");
            rps_send(sockfd, ip, port, "Valid Moves: <Rock>, <Paper>, <Scissor>\n"); 
            return 0;
        }
    }
}
    
/*
Game result lookup table used by the return value of the playgame function
*/
int rules[3][3] = {{USER_TIE, USER_WIN, USER_LOSS},
               {USER_LOSS, USER_TIE, USER_WIN},
               {USER_WIN, USER_LOSS, USER_TIE}};
/*
The game takes user input converts it to lowercase, generates the server's move and returns the result of the user's input vs the server's
*/
void playgame(int sockfd, char *ip, uint16_t port, struct gameinfo * game){
    int servaction,useraction,i;
    char * userinput;
    int playflag = 0;
    int validyesno = 0;
    int validmove = 0;
    int userindex;
    struct user * currentuser;

    userindex = playersearch(game, ip);
    currentuser = game->first;
    for(i=0; userindex>i; i++){
        currentuser = currentuser->next;
    }

    rps_send(sockfd,ip,port,"Want to play a game?");
    rps_send(sockfd,ip,port,"(Y/N): ");
    while(validyesno == 0){
        userinput = rps_recv(sockfd, ip, &port,1);
        validyesno = validateuserinput(sockfd, ip, port, 1, userinput,&useraction);   
       }

    while(playflag == 0){ 
        validmove = 0;
        printf("Waiting for User's move...\n");
        rps_send(sockfd, ip, port,"Select: <ROCK>, <PAPER>, or <SCISSOR>\n");
        rps_send(sockfd, ip, port,"Choice: ");
        while(validmove == 0){
            userinput = rps_recv(sockfd, ip, &port, 15);
            validmove = validateuserinput(sockfd, ip, port,2, userinput,&useraction); 
        }
        printf("User Picked: %s", userinput);

        srand(time(NULL)); 
        servaction = (rand()%3);
        if(servaction == MOVE_ROCK){
            printf("Server picked: rock\n");
            rps_send(sockfd, ip, port,"Server picked rock\n");
        }
        else if(servaction == MOVE_PAPER){
           printf("Server picked: paper\n");
           rps_send(sockfd, ip, port,"Server picked paper\n"); 
        }
        else {
            printf("Server picked: scissor\n");
            rps_send(sockfd, ip, port,"Server picked scissor\n");
        }
    
        if(rules[servaction][useraction] == 0){
            printf("Tied Game!\n");
            currentuser->ties++;
            rps_send(sockfd, ip, port, "Tied Game!");
        }
        else if(rules[servaction][useraction] == 1){ 
            printf("User wins!\n");
            currentuser->wins++;
            rps_send(sockfd, ip, port, "You Win!");
        }
        else if(rules[servaction][useraction] == -1){ 
            printf("Server wins!\n");
            currentuser->losses++;
            rps_send(sockfd, ip, port, "You Lose!");
        } 
        printf("Asking User if they want to play again...\n");  
        rps_send(sockfd, ip, port, "Do you want to play again?");
        rps_send(sockfd, ip, port, "(Y/N): ");
        userinput = rps_recv(sockfd, ip, &port, 3);
        if(*userinput == 'n'){
            printf("User said no.\n");
            playflag = 1;
            printf("Sending Goodbye.\n");
            rps_send(sockfd, ip, port, "Goodbye User!\n");
            printf("Sending User's Stats\n");
            rps_send(sockfd, ip,port, (char *)currentuser);
        }
    }
}
    
/*  
Sends the new user a welcome message and prompts if the user wants to play returns 1 for new game and 2 for goodbye message
*/
int sendwelcomemsg(int sockfd, char * ip, uint16_t port, struct gameinfo * game){
    char * recv_buffer;

    printf("Sending Welcome Message to: %s on port: %hu\n", ip, port);
    rps_send(sockfd,ip,port,"Welcome to my rock paper scissor server!");  
    printf("Sending Game Prompt to: %s on port: %hu\n", ip, port);
    printf("Waiting on user's response to game prompt...\n");
    playgame(sockfd, ip, port, game);
}

/*
Recieve from loop
*/
int recvinfo(int sockfd,struct gameinfo * game){
    struct sockaddr_in clientaddr;
    char * recv_buffer;
    char ip[16];
    uint16_t port;

    memset(ip,0,16);   
 
    //Handshake
    recv_buffer = rps_recv(sockfd, ip, &port, 6);
    printf("New Message from: %s:%hu -> %s\n",ip,port ,recv_buffer);
    if(strncmp(recv_buffer, "Hello", 5)== 0){
        if(playersearch(game, ip) == -1){
            printf("Adding New player to game list: %s\n", ip);
            playeradd(game, ip);
        }  
        sendwelcomemsg(sockfd, ip, port, game);
    }
    else
        printf("other");
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
    char ipaddr[16] = "127.0.0.1";
    char buf[16];
    uint16_t port = 1337;
 
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
    gamecreate(&game);
    sockfd = createSocket(ipaddr, port);
    recvinfo(sockfd, &game);
    return 0;  
  }
