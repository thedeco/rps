#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "1337" 
#define BACKLOG 100	    // how many pending connections queue will hold

#define MSG_WELCOME  	"Welcome to my rock paper scissor server!\n"
#define MSG_PLAY  		"Want to play a new game?\n"
#define MSG_YN    		"(Y/N): "
#define MSG_SELECT 		"Select: <ROCK>, <PAPER>, or <SCISSOR>\n"
#define MSG_CHOICE 		"Choice: "
#define MSG_REPLAY 		"Do you want to play again?\n"
#define MSG_TIE     	"Tied Game!\n"
#define MSG_WIN      	"You Win!\n"
#define MSG_LOSE     	"You Lose!\n"
#define MSG_BYE      	"Goodbye! Thanks for Playing!\n"
#define MSG_INVALIDYN 	"Invalid Y/N Selection!\n"
#define MSG_BADMOVE  	"Invalid Move!\n"
#define MSG_VALIDMOVES  "Valid Moves: <Rock>, <Paper>, <Scissor>\n"

#define MSG_ROCK     	"Server picked: rock.\n"
#define MSG_PAPER    	"Server picked: paper.\n"
#define MSG_SCISSOR  	"Server picked: scissor.\n"

#define MSG_FLAG        "flag{FLAG REMOVED}\n"
#define MSG_FAIL        "No flag for you. Better luck next time!\n"
#define MSG_PLAYMORE    "You need to play at least 50 games!\n"
#define MSG_PLAYBETTER  "You need a win % rate > 50%\n"

#define MOVE_ROCK 0
#define MOVE_PAPER 1
#define MOVE_SCISSOR 2
#define USER_TIE 0
#define USER_WIN 1
#define USER_LOSS -1
#define INVALIDPLAY 2



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



