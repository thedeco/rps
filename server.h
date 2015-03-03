#include "comm.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/ip.h> 
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>

#define MOVE_ROCK 0
#define MOVE_PAPER 1
#define MOVE_SCISSOR 2
#define USER_TIE 0
#define USER_WIN 1
#define USER_LOSS -1
#define INVALIDPLAY 2

struct user {
    char * ipaddr;
    int wins;
    int ties;
    int losses;
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


//Control Flow:
//#1 User connects>srv chks if user has played before>if yes goto #2 if no goto #3
//
//#2 Srv sends welcome msg, # of wins & fails, srv prompts if usr wants to play>
//usr sends "Y or "N">if no, Srv sends "Goodbye" if "Y", srv prompts how many games>usr send//s #>Srv prompts "move: ">usr sends move>
//srv generates random move and chks who won>Srv sends "Srv move: X You (WIN/FAIL)">
//srv update usr's win/fail stats>continues to play until X num of games>Srv sends "Goodbye"
//
// #3 srv creates new usr node, goto 2


