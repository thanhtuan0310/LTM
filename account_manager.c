// Account manager
#include "account_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Lưu dữ liệu vào link list

node create(char username[], char password[], int elo, int current_puzzle, int puzzle_point, int status) {
    node temp;
	temp = (node)malloc(sizeof(struct Account));
	temp->next = NULL;
	strcpy(temp->username, username);
	strcpy(temp->password, password);
    temp->elo = elo;
    temp->current_puzzle = current_puzzle;
    temp->puzzle_point = puzzle_point;
	temp->status = status;
	return temp;
}

node addtail(node head, node temp) {   // Them một node vào cuối danh sách liên kết
	node temp, p;
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

void readFileAccount(FILE *file, node head) {
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    int st;
    int elo;
    int current_puzzle;
    int puzzle_point;
    int status;
    node temp;
    if(file == NULL) {
        printf("no such file.");
        return;
    }
	while (!feof(file)) {
		fscanf(file, "%s %s %d\n", username, password, &status);
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
        }
        temp = create(username, password, elo, current_puzzle, puzzle_point, status);
		head = addtail(head, temp);
	}
}
