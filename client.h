#include "comm.c"
#include <unistd.h>

#define ROCK 0
#define PAPER 1
#define SCISSOR 2

struct user {
    char * ipaddr;
    int wins;
    int ties;
    int losses;
    struct user * next;
    struct user * prev;
}; 
