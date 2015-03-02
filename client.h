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


//Control Flow:
//#1 User connects>srv chks if user has played before>if yes goto #2 if no goto #3
//
//#2 Srv sends welcome msg, # of wins & fails, srv prompts if usr wants to play>
//usr sends "Y or "N">if no, Srv sends "Goodbye" if "Y", srv prompts how many games>usr send//s #>Srv prompts "move: ">usr sends move>
//srv generates random move and chks who won>Srv sends "Srv move: X You (WIN/FAIL)">
//srv update usr's win/fail stats>continues to play until X num of games>Srv sends "Goodbye"
//
// #3 srv creates new usr node, goto 2



