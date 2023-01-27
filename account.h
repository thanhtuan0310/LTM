#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#define MAX_LEN 200


//Cau truc mot account
typedef struct Account {    
    char username[50];
    char password[10];
    char friends[10][50];
    // historyMatch history[5];
    int elo;
    int puzzle_point;
    int current_puzzle;
    int status;
} Account;

// typedef struct match {    
    
// } historyMatch;

typedef enum {
    INSERT_NAME,
    INSERT_PASS,
    INSERT_NEWPASS,
    LOGIN
} LoginStatus;

typedef struct AccountList {
    Account acc;
    struct sockaddr_in client;
    LoginStatus loginSt;
    struct AccountList *next;
    int failPassCount;
} AccountList;

// function 
Account initAccount(char *name, char *pass, int elo, int puzzle_point, int current_puzzle, int status);    // create account node
void printAccountList(AccountList *accList);              // print all account
AccountList *checkExistingAccount(AccountList *accList, char *name); // check if username is in account list
AccountList *searchByClient(AccountList *accList, struct sockaddr_in client);
void addAccountToList(AccountList **accList, Account acc); // add new account into list
void addLoginAccountList(AccountList **accList, struct sockaddr_in client);
void deleteList(AccountList **accList, char *name); // delete account from list
void deleteListByClient(AccountList **accList, struct sockaddr_in client);
void blockAccount(AccountList *accList, char *name); // block an account in list
void freeLst(AccountList **acc);                // free memory