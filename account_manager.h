#ifndef __ACCOUNT_MANAGER_H__
#define __ACCOUNT_MANAGER_H__

#include <stdio.h>

#define USERNAME_SIZE 50
#define PASSWORD_SIZE 10
#define FILENAME_SIZE 30
#define MAX_LENGTH 1024
#define MAX_NODE_LIST 100
#define FRIEND_COUNT 30
#define MAX_FRIEND 30
#define MAX_CONSECUTIVE_FAIL 3


//Cau truc mot account
typedef struct Account {    
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char friends[MAX_FRIEND][USERNAME_SIZE];
    char friend_req[30][USERNAME_SIZE];
    char wait_add_friend[30][USERNAME_SIZE];
    // historyMatch history[5];
    int elo;
    int current_puzzle;
    int puzzle_point;
    int is_signed_in;
    int match_count;
    int win;
    int frie_count;
    int frie_req_count;
    int wait_add_friend_count;
    struct Account *next;
} Account;

typedef struct Ranking {
    char username[USERNAME_SIZE];
    int elo;
    int puzzle_point;
} Ranking;

typedef struct Account *node;

node create(char username[], char password[], int elo, int current_puzzle, int puzzle_point, int is_signed_in, int match_count, 
            int win, int frie_count, int frie_req_count, int wait_add_friend_count, char friend[][USERNAME_SIZE], char friend_req[][USERNAME_SIZE], char wait_add_friend[30][USERNAME_SIZE]);
node addtail(node head, node temp);
node search(node head, char username[]);
void printFriendList(node head);
void printLists(node head);
void readFileAccount(node *head);
void updateAccountFile(node head);
void addFileAccount(node head, char username[]);
int getUserCount(node head);

#endif