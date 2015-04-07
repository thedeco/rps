#include "server.h"

int playersearch(struct gameinfo * game, char * ip)
{
	int i;
	struct user * currentuser;
	currentuser = game-> first;
	for(i =0; i<game->n; i++){
		if(strncmp(currentuser->ipaddr, ip,strlen(ip)) == 0){
            return i;
        }
		else{
			currentuser = currentuser->next;
		}
	}
	return -1;
}

void gamecreate(struct gameinfo * game)
{
   game->first = NULL;
   game->last = NULL;
   game->n = 0;
}

void playeradd(struct gameinfo * game,char * ip)
{
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

int validateuserinput(int sockfd,int type, char * userinput,int *useraction)
{
    int flag = 0,i;
    
    //Change user input to all lowercase
    for(i = 0; userinput[i]; i++){
        userinput[i] = tolower(userinput[i]); 
    }

    //Type 1 is a yes/no message
    if(type == 1){
        if(*userinput == 'y'){
            return 1;
        }
        else if(*userinput == 'n'){
            send(sockfd,BYE_MSG,sizeof(BYE_MSG),0);
            return 1;
        }
        else {
            printf("Invalid Y/N selection\n");
            send(sockfd,INVALID_MOVE,sizeof(INVALID_MOVE),0);
            send(sockfd,REDO,sizeof(REDO) ,0); 
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
            return 2; 
        }
        else if((strncmp("scissor",userinput,strlen(userinput)-1) ==0)){
            *useraction = MOVE_SCISSOR;
            return 3;
        }
        else{
            printf("Invalid move!\n");
            send(sockfd,INVALID_PLAY,sizeof(INVALID_PLAY),0);
            send(sockfd,VALID_MOVES,sizeof(VALID_MOVES),0); 
            send(sockfd,CHOICE_PROMPT,sizeof(CHOICE_PROMPT),0);
            return 0;
        }
    }
}

int getservermove(int sockfd)
{
    int servaction;
    srand(time(NULL)); 
    servaction = (rand()%3);
    if(servaction == MOVE_ROCK){
        printf(SRV_ROCK);
        send(sockfd,SRV_ROCK,sizeof(SRV_ROCK),0);
    }
    else if(servaction == MOVE_PAPER){
        printf(SRV_PAPER);
        send(sockfd,SRV_PAPER,sizeof(SRV_PAPER),0); 
    }
    else {
        printf(SRV_SCISSOR);
        send(sockfd,SRV_SCISSOR,sizeof(SRV_SCISSOR),0);
    }
    return servaction;
}

void getgameresults(int sockfd, int servaction,int useraction, struct user * currentuser)
{
    if(rules[servaction][useraction] == 0){
        printf(TIE_MSG);
        currentuser->ties++;
        send(sockfd,TIE_MSG,sizeof(TIE_MSG),0);
    }
    else if(rules[servaction][useraction] == 1){ 
        printf(WIN_MSG);
        currentuser->wins++;
        send(sockfd,WIN_MSG,sizeof(WIN_MSG),0);
    }
    else if(rules[servaction][useraction] == -1){ 
        printf(LOSE_MSG);
        currentuser->losses++;
        send(sockfd,LOSE_MSG,sizeof(LOSE_MSG),0);
    }     
}
   
void playgame(int sockfd,char * ip, struct gameinfo * game)
{
    int servaction,useraction,i;
    char * userinput;
    char * usermove;
    int playflag = 0;
    int validyesno = 0;
    int validmove = 0;
    int userindex;
    struct user * currentuser;
    userinput = (char *)malloc(1);
    usermove = (char *)malloc(15);
    memset(usermove, 0, 15);
    char buffer[100];

    userindex = playersearch(game, ip);
    currentuser = game->first;
    for(i=0; userindex>i; i++){
        currentuser = currentuser->next;
    }

    send(sockfd,PLAY_PROMPT,sizeof(PLAY_PROMPT),0);
    send(sockfd,YN_PROMPT,sizeof(YN_PROMPT),0);

    while(playflag == 0){ 
        while(validyesno == 0){
            recv(sockfd, userinput,2,0);
            validyesno = validateuserinput(sockfd, 1, userinput,&useraction);   
        }
        if(*userinput == 'n'){
            playflag = 1;
            printf("Sending Goodbye.\n");
            printf("Sending User's Stats\n");
            snprintf(buffer, 100, "IP: %s wins: %d losses: %d ties: %d\n",ip, currentuser->wins,currentuser->losses, currentuser->ties);
            send(sockfd,buffer,strlen(buffer),0);
            printf("IP: %s wins: %d losses: %d ties: %d\n",ip, currentuser->wins,currentuser->losses, currentuser->ties);
            return;
        }
        validmove=0;
        validyesno=0;
        printf("Waiting for User's move...\n");
        send(sockfd,SELECT_PROMPT,sizeof(SELECT_PROMPT),0);
        send(sockfd,CHOICE_PROMPT,sizeof(CHOICE_PROMPT),0);
        while(validmove == 0){
            memset(usermove,0,15); 
            recv(sockfd,usermove, 15,0);
            validmove = validateuserinput(sockfd,2, usermove,&useraction); 
        }
        printf("User Picked: %s", usermove);
        servaction=getservermove(sockfd);   
        getgameresults(sockfd, servaction,useraction, currentuser);
       
        printf("Asking User if they want to play again...\n");  
        send(sockfd,REPLAY_PROMPT,sizeof(REPLAY_PROMPT),0);
        send(sockfd,YN_PROMPT,sizeof(YN_PROMPT),0);
    }
}
    
int sendwelcomemsg(int sockfd, char * ip, uint16_t port, struct gameinfo * game)
{
    printf("Sending Welcome Message to: %s on port: %hu\n", ip, port);
    send(sockfd,WELCOME_MSG,sizeof(WELCOME_MSG),0);  
    printf("Sending Game Prompt to: %s on port: %hu\n", ip, port);
    printf("Waiting on user's response to game prompt...\n");
    playgame(sockfd, ip, game);
}

int recvinfo(int sockfd,struct gameinfo * game)
{
    struct sockaddr_in clientaddr;
    char * recv_buffer;
    uint16_t port;
    struct sockaddr_in addr;  
    socklen_t addr_size = sizeof(struct sockaddr_in);  
    getpeername(sockfd, (struct sockaddr *)&addr, &addr_size);
  
    if(playersearch(game,inet_ntoa(addr.sin_addr)) == -1){
        printf("Adding New player to game list: %s\n",inet_ntoa(addr.sin_addr));
        playeradd(game,inet_ntoa(addr.sin_addr));  
        sendwelcomemsg(sockfd,inet_ntoa(addr.sin_addr), addr.sin_port, game);
    }
    else
        printf("other");
} 

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int startserver(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
    struct gameinfo game; 

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
        }

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("Listening for new connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("New player connected! %s\n", s);
 
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
            gamecreate(&game);
        	recvinfo(new_fd,&game);
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}
	return 0;
}

int main(int argc, char *argv[])
{    
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
    startserver();
    return 0;  
  }
