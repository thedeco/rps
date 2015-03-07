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
