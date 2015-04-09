#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/ip.h> 
#include <linux/tcp.h>
#include <linux/udp.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT "1337" 
#define BACKLOG 100	 // how many pending connections queue will hold
#define WELCOME_MSG  "Welcome to my rock paper scissor server!\n" 
#define PLAY_PROMPT  "Want to play a game?\n"
#define YN_PROMPT    "(Y/N): "   
#define SELECT_PROMPT "Select: <ROCK>, <PAPER>, or <SCISSOR>\n"
#define CHOICE_PROMPT "Choice: "
#define REPLAY_PROMPT "Do you want to play again?\n"
#define TIE_MSG      "Tied Game!\n"
#define WIN_MSG      "You Win!\n"
#define LOSE_MSG     "You Lose!\n"
#define SRV_ROCK     "Server picked: rock.\n"
#define SRV_PAPER    "Server picked: paper.\n"
#define SRV_SCISSOR  "Server picked: scissor.\n"
#define BYE_MSG      "Goodbye! Thanks for Playing!\n"
#define MOVE_ROCK 0
#define MOVE_PAPER 1
#define MOVE_SCISSOR 2
#define USER_TIE 0
#define USER_WIN 1
#define USER_LOSS -1
#define INVALIDPLAY 2
#define INVALID_MOVE "Invalid Y/N Selection!\n"
#define REDO         "Please select <Y/N>: "
#define INVALID_PLAY "Invalid Move!\n"
#define VALID_MOVES  "Valid Moves: <Rock>, <Paper>, <Scissor>\n"
#define FLAG         "flag{FLAG REMOVED}\n"
#define FAIL         "No flag for you. Better luck next time!\n"
#define PLAYMORE     "You need to play at least 50 games!\n"
#define PLAYBETTER   "You need a win % rate > 50%\n"

struct user {
    uint16_t wins;
    uint16_t ties;
    uint16_t losses;
    char * ipaddr;
    struct user * next;
    struct user * prev;
}; 

struct gameinfo {
    struct user * first;
    struct user * last;
    int n; 
}; 

/*
Game result lookup table used by the return value of the playgame function
*/
int rules[3][3] = {{USER_TIE, USER_WIN, USER_LOSS},
               {USER_LOSS, USER_TIE, USER_WIN},
               {USER_WIN, USER_LOSS, USER_TIE}};

void sigchld_handler(int s);
int startserver(void);
int playersearch(struct gameinfo * game, char * ip);
void gamecreate(struct gameinfo * game);
void playeradd(struct gameinfo * game,char * ip);
int validateuserinput(int sockfd,int type, char * userinput,int *useraction);
int getservermove(int sockfd);
void getgameresults(int sockfd, int servaction,int useraction, struct user * currentuser);
void playgame(int sockfd,char * ip, struct gameinfo * game);
int sendwelcomemsg(int sockfd, char * ip, uint16_t port, struct gameinfo * game);
int recvinfo(int sockfd,struct gameinfo * game);





