#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

Account initAccount(char *username, char *password, int elo, int current_puzzle, int puzzle_point, int status) {
    Account acc;
    strcpy(acc.username, username);
    strcpy(acc.password, password);
    acc.elo = elo;
    acc.current_puzzle = current_puzzle;
    acc.puzzle_point = puzzle_point;
    acc.status = status;
    return acc;
}

void printAccountList(AccountList *accList) {
    AccountList *ptr = accList;
    while (ptr != NULL) {
        printf("%s %s %d \n", ptr->acc.username, ptr->acc.password, ptr->acc.status);
        ptr = ptr->next;
    }
}

AccountList *checkExistingAccount(AccountList *accList, char *name) {
    AccountList *ptr = accList;
    while (ptr != NULL) {
        if (strcmp(ptr->acc.username, name) == 0) return ptr;
        ptr = ptr->next;
    }
    return NULL;
}

AccountList *searchByClient(AccountList *accList, struct sockaddr_in client) {
    AccountList *ptr = accList;
    while (ptr != NULL) {
        if (!strcmp(inet_ntoa(client.sin_addr), inet_ntoa(ptr->client.sin_addr)) && ntohs(client.sin_port) == ntohs(ptr->client.sin_port))
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}

void addAccountToList(AccountList **accList, Account acc) {
    AccountList *new = (AccountList *)malloc(sizeof(AccountList));
    new->acc = acc;
    new->next = NULL;
    // new->loginSt = INSERT_NAME;
    new->failPassCount = 0;

    // if head is NULL, it is an empty list
    if (*accList == NULL) *accList = new;
    else {
        AccountList *lastNode = *accList;
        while (lastNode->next != NULL) {
            lastNode = lastNode->next;
        }
        lastNode->next = new;
    }
}

void addLoginAccountList(AccountList **accList, struct sockaddr_in client) {
    AccountList *new = (AccountList *)malloc(sizeof(AccountList));
    new->acc = initAccount("", "", 1000, 1, 0, 1);
    new->next = NULL;
    new->client = client;
    // new->loginSt = INSERT_NAME;
    new->failPassCount = 0;

    if (*accList == NULL) *accList = new;
    else {
        AccountList *lastNode = *accList;
        while (lastNode->next != NULL) {
            lastNode = lastNode->next;
        }
        lastNode->next = new;
    }
}

void deleteList(AccountList **accList, char *name) {
    AccountList *ptr = *accList;
    AccountList *delete;
    if (ptr != NULL && strcmp(ptr->acc.username, name) == 0) {
        *accList = ptr->next;
        free(ptr);
        return;
    }
    while (ptr->next != NULL) {
        if (strcmp(ptr->next->acc.username, name) == 0) {
            delete = ptr->next;
            ptr->next = ptr->next->next;
            free(delete);
            return;
        }
        ptr = ptr->next;
    }
}

void deleteListByClient(AccountList **accList, struct sockaddr_in client) {
    AccountList *ptr = *accList;
    AccountList *delete;
    if (ptr != NULL && !strcmp(inet_ntoa(client.sin_addr), inet_ntoa(ptr->client.sin_addr)) && ntohs(client.sin_port) == ntohs(ptr->client.sin_port)) {
        *accList = ptr->next;
        free(ptr);
        return;
    }
    while (ptr->next != NULL) {
        if (!strcmp(inet_ntoa(client.sin_addr), inet_ntoa(ptr->client.sin_addr)) && ntohs(client.sin_port) == ntohs(ptr->client.sin_port)) {
            delete = ptr->next;
            ptr->next = ptr->next->next;
            free(delete);
            return;
        }
        ptr = ptr->next;
    }
}

void blockAccount(AccountList *accList, char *name) {
    AccountList *accNode = checkExistingAccount(accList, name);
    if (accNode != NULL) accNode->acc.status = 0;
}

void freeLst(AccountList **acc) {
    AccountList *ptr = *acc;
    AccountList *temp = NULL;
    while (ptr != NULL) {
        temp = ptr->next;
        free(ptr);
        ptr = temp;
    }
    *acc = NULL;
}
