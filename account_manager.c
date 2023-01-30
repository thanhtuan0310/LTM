// Account manager
#include "account_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Lưu dữ liệu vào link list

node create(char username[], char password[], int elo, int current_puzzle, int puzzle_point, int status, int is_signed_in, int match_count, int win) {
    node temp;
	temp = (node)malloc(sizeof(struct Account));
	temp->next = NULL;
	strcpy(temp->username, username);
	strcpy(temp->password, password);
    temp->elo = elo;
    temp->current_puzzle = current_puzzle;
    temp->puzzle_point = puzzle_point;
	temp->status = status;
    temp->is_signed_in = is_signed_in;
    temp->match_count = match_count;
    temp->win = win;
	return temp;
}

node addtail(node head, node temp) {   // Them một node vào cuối danh sách liên kết
	node p;
	if (head == NULL) {
		head = temp;
	} else {
		p = head; 
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = temp;
	}
	return head;
}

node search(node head, char username[]) {
    // tìm kiếm account theo username
	node p = head;
	if (strcmp(p->username, username) == 0) {
		return p;
	}
	while (p->next->next != NULL && strcmp(p->next->username, username) != 0) p = p->next;
	if (strcmp(p->next->username, username) == 0) return p->next;
	else return NULL;
}

void printLists(node head) {
    node p = head;
    while(p != NULL) {
        printf("%s %s %d %d %d %d %d %d %d\n", p->username, p->password, p->elo, p->current_puzzle, p->puzzle_point, p->status, p->is_signed_in, p->match_count, p->win);
        p = p -> next;
    }
}

void printFriendList(node head) {
    node p = head;
    int i = 0;
    while(p != NULL) {
        while(p->friends[i]) {
            printf("%s\n", p->friends[i++]);
        }
        i = 0;
        p = p -> next;
    }
}

void readFileAccount(node *head) {
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    int st;
    int elo;
    int match_count;
    int win;
    // char friends[FRIEND_COUNT][USERNAME_SIZE];
    int current_puzzle;
    int puzzle_point;
    int status;
    int is_signed_in;
    node temp;
    int i = 0;
    int j = 0;
    FILE *file = fopen("account.txt", "r");
    if(file == NULL) {
        printf("no such file.");
        return;
    }
	while (!feof(file)) {
		fscanf(file, "%s %s %d %d\n", username, password, &status, &is_signed_in);
        char fileName[FILENAME_SIZE] = "./account/";
        char line[MAX_LENGTH];
        strcat(fileName, username);
        strcat(fileName, ".txt");
        FILE *f = fopen(fileName, "r");
        if(f == NULL) {
            printf("No such file.");
            return;
        }
        while(fgets(line, sizeof(line), f) != NULL) {
            i = 0; j = 0;
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
                    if(p != NULL) current_puzzle = atoi(p);
                }
                if(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) puzzle_point = atoi(p);
                }
            }
            if(strstr(line, "MATCH_COUNT")) {
                p = strtok(line, " ");
                while(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) match_count = atoi(p);
                }
            }
            if(strstr(line, "WIN")) {
                p = strtok(line, " ");
                while(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) win = atoi(p);
                }
            }
            if(strstr(line, "FRIENDS")) {
                p = strtok(line, " ");
                while(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) {
                        // printf("%s\n", p);
                        strcpy(temp->friends[i++], p);
                    }
                }
            }
            if(strstr(line, "FRIEND_REQUEST")) {
                p = strtok(line, " ");
                while(p != NULL) {
                    p = strtok(NULL, " ");
                    if(p != NULL) {
                        // printf("%s\n", p);
                        strcpy(temp->friends[j++], p);
                    }
                }
            }
        }
        temp = create(username, password, elo, current_puzzle, puzzle_point, status, is_signed_in, match_count, win);
		*head = addtail(*head, temp);
	}
}

void updateAccountFile(node head) {
    FILE *file = fopen("account.txt", "w");
    for (node p = head; p != NULL; p = p->next) {
		fprintf(file, "%s %s %d %d\n", p->username, p->password, p->status, p->is_signed_in);
	}
	fclose(file);
}

void addFileAccount(char username[]) {
    char fileName[FILENAME_SIZE] = "./account/";
    strcat(fileName, username);
    strcat(fileName, ".txt");
    FILE *file = fopen(fileName, "w+");
    fprintf(file, "%s %d\n", "ELO", 1000);
    fprintf(file, "%s %d %d\n", "PUZZLE", 1, 0);
    fprintf(file, "%s %d\n", "MATCH_COUNT", 0);
    fprintf(file, "%s %d\n", "WIN", 0);
    fprintf(file, "%s\n", "FRIEND");
    fprintf(file, "%s\n", "FRIEND_REQUEST");
    fprintf(file, "%s", "MATCH_HISTORY");
    fclose(file);
}
