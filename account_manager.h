#ifndef __ACCOUNT_MANAGER_H__
#define __ACCOUNT_MANAGER_H__

#define USERNAME_SIZE 50
#define PASSWORD_SIZE 10
#define FILENAME_SIZE 30
#define MAX_LENGTH 30
#define FRIEND_COUNT 10
#define MAX_CONSECUTIVE_FAIL 3


//Cau truc mot account
typedef struct Account {    
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char friends[FRIEND_COUNT][USERNAME_SIZE];
    // historyMatch history[5];
    int elo;
    int current_puzzle;
    int puzzle_point;
    int status;
    struct Account *next;
} Account;

typedef struct Account *node;

node create(char username[], char password[], int elo, int current_puzzle, int puzzle_point, int status);
node addtail(node head, node temp);
node search(node head, char username[]);
void readFileAccount(FILE *file, node head);

#endif