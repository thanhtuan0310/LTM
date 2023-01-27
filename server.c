#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "account.h"

#define PORT 5550 /* Port that will be opened */
#define BUFF_SIZE 1024
char buff[BUFF_SIZE];
char letter[BUFF_SIZE];
char num[BUFF_SIZE];

AccountList *lst = NULL;        // Account list
AccountList *logLst = NULL;     // Login Account list
AccountList *clientNode = NULL; // current client node
AccountList *node = NULL;       // current account node

void readFileAccount() {
    char name[MAX_LEN];
    char pass[MAX_LEN];
    int st;
    int elo;
    int puzzle_point;
    int current_point;
    Account acc;
    FILE *fptr = fopen("account.txt", "r");
    if (fptr == NULL) {
        printf("no such file.");
        return;
    }
    while (fscanf(fptr, "%s %s %d", name, pass, &st) != EOF) {
        char fileName[256] = "./account/";
        char line[30];
        strcat(fileName, name);
        strcat(fileName, ".txt");
        FILE *f = fopen(fileName, "r");
        if(f == NULL) {
            printf("No such file.");
            return;
        }
        while(fgets(line, sizeof(line), f) != NULL) {
            char *p;
            if(strstr(line, "ELO")) {
                p = strtok(line, " ");
                while(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) elo = atoi(p);
                }
            }
            if(strstr(line, "PUZZLE")) {
                p = strtok(line, " ");
                if(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) puzzle_point = atoi(p);
                }
                if(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) current_point = atoi(p);
                }
            }
        }
        acc = initAccount(name, pass, elo, puzzle_point, current_point, st);
        addAccountToList(&lst, acc);
    }
    fclose(fptr);
}

void saveFileAccount() {
    AccountList *ptr = lst;
    FILE *fptr = fopen("account.txt", "w+");
    if (fptr == NULL) {
        printf("no such file.");
        return;
    }
    while (ptr != NULL) {
        fprintf(fptr, "%s %s %d\n", ptr->acc.username, ptr->acc.password, ptr->acc.status);
        ptr = ptr->next;
    }
    fclose(fptr);
}

void editResult() {
    strcpy(letter, "");
    strcpy(num, "");
    int j = 0;
    for (int i = 0; i < strlen(buff); i++) {
        if ((buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >= 'A' && buff[i] <= 'Z')) {
            letter[j] = buff[i];
            j++;
        }
        else if (buff[i] >= '0' && buff[i] <= '9')
            continue;
        else {
            strcpy(letter, "Error");
            return;
        }
    }
    letter[j] = '\0';
    j = 0;
    for (int i = 0; i < strlen(buff); i++) {
        if ((buff[i] >= '0' && buff[i] <= '9')) {
            num[j] = buff[i];
            j++;
        }
    }
    num[j] = '\0';
}

int checkValidPassword(char *str) {
    int i = 0;
    for (i = 0; i < strlen(str); i++) {
        if (!((buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >= 'A' && buff[i] <= 'Z') || buff[i] >= '0' && buff[i] <= '9')) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    readFileAccount();
    AccountList *ptr;
    int server_sock; /* file descriptors */
    int bytes_sent, bytes_received;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */

    int sin_size;
    if (argc != 2) {
        printf("Invalid Parameter! \n./server portNumber\n");
        exit(0);
    }

    // Step 1: Construct a UDP socket
    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Bind address to socket
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1])); /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = INADDR_ANY;    /* INADDR_ANY puts your IP address automatically */
    bzero(&(server.sin_zero), 8);           /* zero the rest of the structure */

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) { /* calls bind() */
        perror("\nError: ");
        exit(0);
    }
    sin_size = sizeof(struct sockaddr_in);

    // Step 3: Communicate with clients
    FILE *chat;      
    chat = fopen("groupchat.txt","w");
    int messs=0;
    char mess[100][100];

    while (1) {
        bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&client, &sin_size);
        if (bytes_received >= 0) buff[bytes_received] = '\0';
        if (bytes_received < 0) perror("\nError: ");
        else if (!strcmp(buff, "Exit")) {    // if client exit, delete client form login list
            deleteListByClient(&logLst, client);
            clientNode->loginSt = INSERT_NAME;
            clientNode->failPassCount = 0;
            strcpy(buff, "Goodbye ");
            strcat(buff, clientNode->acc.username);
            break;
        } else {
            buff[bytes_received] = '\0';
            clientNode = searchByClient(logLst, client);
            if (clientNode == NULL) {   // if client have not connected before, create one node for client
                addLoginAccountList(&logLst, client);
                printf("Successfull conect to client [%s:%d]\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                strcpy(buff, "Enter username:");
                bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                if (bytes_sent < 0) {
                    perror("Error: ");
                    close(server_sock);
                    return 0;
                }
            } else {
                switch (clientNode->loginSt) {       // check client status for process
                    case INSERT_NAME: // hander username
                        node = checkExistingAccount(lst, buff);
                        if (node == NULL) {
                            strcpy(buff, "Account not found \nEnter username:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        } else if (node->acc.status == 0) {
                            strcpy(buff, "Account not ready \nEnter username:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        } else {
                            clientNode->acc = node->acc;
                            clientNode->loginSt = INSERT_PASS;
                            strcpy(buff, "Enter password:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        }
                        break;
                    case INSERT_PASS: // hander inserted password
                        if (!strcmp(clientNode->acc.password, buff)) {
                            clientNode->loginSt = LOGIN;
                            strcpy(buff, "OK\nLoggin Successfully. Enter \"change password\" to change password, \"bye\" to exit. \nEnter message:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            for (int i = 0; i < messs; i++) {
                                strcpy(buff,mess[i]);
                                bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            }
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        } else {
                            clientNode->failPassCount++;
                            if (clientNode->failPassCount >= 3) {
                                // clientNode->loginSt = INSERT_NAME;
                                node = checkExistingAccount(lst, clientNode->acc.username);
                                if (node != NULL) node->acc.status = 0;
                                saveFileAccount();
                                clientNode->loginSt = INSERT_NAME;
                                clientNode->failPassCount = 0;
                                strcpy(buff, "Wrong password! Acount is blocked! \nEnter username:");
                                bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                                if (bytes_sent < 0) {
                                    perror("Error: ");
                                    close(server_sock);
                                    return 0;
                                }
                            } else {
                                strcpy(buff, "Wrong password!Enter password:");
                                bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                                if (bytes_sent < 0) {
                                    perror("Error: ");
                                    close(server_sock);
                                    return 0;
                                }
                            }
                        }
                        break;
                    case LOGIN: // hander functions after login
                        if (!strcmp(buff, "change password")) {
                            clientNode->loginSt = INSERT_NEWPASS;
                            strcpy(buff, "Enter new password:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        } else if (!strcmp(buff, "bye")) {
                            clientNode->loginSt = INSERT_NAME;
                            clientNode->failPassCount = 0;
                            strcpy(buff, "Goodbye ");
                            strcat(buff, clientNode->acc.username);
                            strcat(buff, "\nEnter username:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        } else {
                            ptr = logLst;
                            //save chat
                            if (chat == NULL) {
                                bytes_sent = sendto(server_sock, "eropen", 6, 0, (struct sockaddr *)&client, sin_size);
                            }
                            strcpy(mess[messs++],buff);
                            editResult();
                            while (ptr != NULL) {
                                if ((strcmp(inet_ntoa(client.sin_addr), inet_ntoa(ptr->client.sin_addr)) || ntohs(client.sin_port) != ntohs(ptr->client.sin_port)) && ptr->loginSt == LOGIN) {
                                    bytes_sent = sendto(server_sock, letter, strlen(letter), 0, (struct sockaddr *)&ptr->client, sin_size);
                                    if (bytes_sent < 0) {
                                        perror("Error: ");
                                        close(server_sock);
                                        return 0;
                                    }
                                    bytes_sent = sendto(server_sock, num, strlen(num), 0, (struct sockaddr *)&ptr->client, sin_size);
                                    if (bytes_sent < 0) {
                                        perror("Error: ");
                                        close(server_sock);
                                        return 0;
                                    }
                                }
                                ptr = ptr->next;
                            }
                        }
                        break;
                    case INSERT_NEWPASS: // hander inserted new password
                        if (checkValidPassword(buff)) {
                            clientNode->loginSt = LOGIN;
                            node = checkExistingAccount(lst, clientNode->acc.username);
                            if (node != NULL) strcpy(node->acc.password, buff);
                            strcpy(clientNode->acc.password, buff);
                            saveFileAccount();
                            editResult();
                            strcpy(buff, "New password:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                            bytes_sent = sendto(server_sock, letter, strlen(letter), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                            bytes_sent = sendto(server_sock, num, strlen(num), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                            strcpy(buff, "Enter message:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        } else {
                            strcpy(buff, "Invalid password. Enter new password:");
                            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *)&client, sin_size);
                            if (bytes_sent < 0) {
                                perror("Error: ");
                                close(server_sock);
                                return 0;
                            }
                        }
                        break;

                    default:
                        break;
                }
            }
        }
    }
    for (int i = 0; i < messs; i++) {
        fprintf(chat,"%s\n",mess[i]);
    }
    fclose(chat);
    close(server_sock);
    saveFileAccount();
    return 0;
}