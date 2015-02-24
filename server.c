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
int playgame(){
    int servaction,useraction,i;
    char userinput[10];
 
    printf("Select: <ROCK>, <PAPER>, or <SCISSOR>\n");
    printf("Choice: ");
    fgets(userinput, sizeof(userinput), stdin);    
 
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
     else
        return INVALIDPLAY;

    srand(time(NULL)); 
    servaction = (rand()%3);
    if(servaction == MOVE_ROCK)
        printf("Server picked ROCK\n");
    else if(servaction == MOVE_PAPER)
        printf("Server picked PAPER\n");
    else
        printf("Server picked SCISSOR\n");

    return rules[servaction][useraction];
}

/*
Recieve from loop
*/
int recvinfo(int sockfd){
    struct sockaddr_in clientaddr;
    char * recv_buffer;
    char clientip[16];

//Recvloop-------------------------------------------------------------------------------------
    fprintf(stderr, "Server is waiting for new data...\n");
    for (;;){
        rps_recv(sockfd, &clientaddr, recv_buffer,4);
        printf("%s\n", recv_buffer);
       //---------------------------------------------------------------------------------------------
    }
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

/*
Sends the new user a welcome message and prompts if the user wants to play returns 1 for new game and 2 for goodbye message
*/
int sendwelcomemsg(int sockfd, struct sockaddr_in *clientaddr){
    rps_send(sockfd,clientaddr,"Welcome to my rock paper scissor server!\n");  
    rps_send(sockfd,clientaddr,"Want to play a game?\n");
}

int main(){
    struct gameinfo game;
    int sockfd, result;
   
    printf("Starting Game Server\n");
    create(&game);
    sockfd = createSocket(1337);
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

