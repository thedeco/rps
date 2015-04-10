#include "server.h"

void printFlag(int sockfd, struct user * player)
{
    int nGames,winrate;
	char *results;
	
    nGames=player->wins+player->ties+player->losses;
    winrate=((player->wins)*100)/nGames;
	
    if((nGames > 50) && (winrate>=50) ){
        send(sockfd, MSG_FLAG, strlen(MSG_FLAG), 0);
    }
    else if (nGames<50){
        asprintf(&results,"Number of games you played: %d\n",nGames);
        send(sockfd,MSG_PLAYMORE MSG_FAIL,strlen(MSG_PLAYMORE)+strlen(MSG_FAIL),0);
		send(sockfd,results,strlen(results),0);
	}
    else if (winrate<50){
        asprintf(&results, "Your win rate: %d\n",winrate);
		send(sockfd,MSG_PLAYBETTER MSG_FAIL,strlen(MSG_PLAYBETTER)+strlen(MSG_FAIL),0);   
		send(sockfd,results,strlen(results),0);
    }
}

int playerSearch(struct gameinfo * game, char * ip)
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

void gameCreate(struct gameinfo * game)
{
   game->first = NULL;
   game->last = NULL;
   game->n = 0;
}

void playerAdd(struct gameinfo * game,char * ip)
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
            send(sockfd,MSG_BYE,strlen(MSG_BYE),0);
            return 1;
        }
        else {
            send(sockfd,MSG_INVALIDYN MSG_YN,strlen(MSG_INVALIDYN) + strlen(MSG_YN),0);
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
            send(sockfd,MSG_BADMOVE MSG_VALIDMOVES MSG_CHOICE,
				 strlen(MSG_BADMOVE)+strlen(MSG_VALIDMOVES)+strlen(MSG_CHOICE),0);
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
        send(sockfd,MSG_ROCK,strlen(MSG_ROCK),0);
    }
    else if(servaction == MOVE_PAPER){
        send(sockfd,MSG_PAPER,strlen(MSG_PAPER),0); 
    }
    else {
        send(sockfd,MSG_SCISSOR,strlen(MSG_SCISSOR),0);
    }
    return servaction;
}

void getgameresults(int sockfd, int servaction,int useraction, struct user * currentuser)
{
    if(rules[servaction][useraction] == 0){
        currentuser->ties++;
        send(sockfd,MSG_TIE,strlen(MSG_TIE),0);
    }
    else if(rules[servaction][useraction] == 1){ 
        currentuser->wins++;
        send(sockfd,MSG_WIN,strlen(MSG_WIN),0);
    }
    else if(rules[servaction][useraction] == -1){ 
        currentuser->losses++;
        send(sockfd,MSG_LOSE,strlen(MSG_LOSE),0);
    }     
}
   
void playGame(int sockfd,char * ip, struct gameinfo * game)
{
    int servaction,useraction,i,userindex;
    char *userinput,*usermove;
    int playMSG_FLAG = 0;
    int validyesno = 0;
    int validmove = 0;
    struct user * currentuser;
    userinput = (char *)malloc(1);
    usermove = (char *)malloc(15);
    memset(usermove, 0, 15);
    char buffer[100];

    userindex = playerSearch(game, ip);
    currentuser = game->first;
    for(i=0; userindex>i; i++){
        currentuser = currentuser->next;
    }

    send(sockfd,MSG_WELCOME MSG_PLAY MSG_YN,strlen(MSG_WELCOME)+
		 strlen(MSG_PLAY)+strlen(MSG_YN),0);  

    while(playMSG_FLAG == 0){ 
        while(validyesno == 0){
            recv(sockfd, userinput,2,0);
            validyesno = validateuserinput(sockfd, 1, userinput,&useraction);   
        }
        if(*userinput == 'n'){
            playMSG_FLAG = 1;
            snprintf(buffer, 100, "IP: %s wins: %d losses: %d ties: %d\n",ip, currentuser->wins,currentuser->losses, currentuser->ties);
            send(sockfd,buffer,strlen(buffer),0);
            printFlag(sockfd, currentuser);
            return;
        }
        validmove=0;
        validyesno=0;
        send(sockfd,MSG_SELECT MSG_CHOICE,strlen(MSG_SELECT)+strlen(MSG_CHOICE),0);
        while(validmove == 0){
            memset(usermove,0,15); 
            recv(sockfd,usermove, 15,0);
            validmove = validateuserinput(sockfd,2, usermove,&useraction); 
        }
        servaction=getservermove(sockfd);   
        getgameresults(sockfd, servaction,useraction, currentuser);
        send(sockfd,MSG_PLAY MSG_YN,strlen(MSG_REPLAY)+strlen(MSG_YN)-2,0);
    }
}
    
int recvInfo(int sockfd,struct gameinfo * game)
{
    struct sockaddr_in clientaddr;
    char * recv_buffer;
    uint16_t port;
    struct sockaddr_in addr;  
    socklen_t addr_size = sizeof(struct sockaddr_in);  
    getpeername(sockfd, (struct sockaddr *)&addr, &addr_size);
  
    if(playerSearch(game,inet_ntoa(addr.sin_addr)) == -1){
        playerAdd(game,inet_ntoa(addr.sin_addr));  
        playGame(sockfd,inet_ntoa(addr.sin_addr),game);
    }
    else
        playGame(sockfd,inet_ntoa(addr.sin_addr),game);
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

int startServer(void)
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
 
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
            gameCreate(&game);
        	recvInfo(new_fd,&game);
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
    
               default: /* 'Wrong usage MSG_FAIL' */
            fprintf(stderr, "Usage: %s [-i IP Address] [-p Port #]\n",argv[0]);
            return -1;
        }
    }
 
    printf("Game server is running...\n");
    startServer();
    return 0;  
  }
