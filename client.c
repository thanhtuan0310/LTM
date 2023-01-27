#include <stdio.h> /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SERV_PORT 5550
#define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024

int client_sock;
char buff[BUFF_SIZE];
struct sockaddr_in server_addr;
int bytes_sent, bytes_received, sin_size;

void *printMessage(void *vargp) {
    char buff2[BUFF_SIZE];
    while (1) {
        bytes_received = recvfrom(client_sock, buff2, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
        if (bytes_received > 0) {
            buff2[bytes_received] = '\0';
            printf("%s\n", buff2);
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Invalid Parameter! \n./client IPaddress portNumber username password\n");
        exit(0);
    }
    pthread_t thread_id;
    // Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Define the address of the server
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sin_size = sizeof(struct sockaddr);

    strcpy(buff, "Init");
    bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
    if (bytes_sent < 0) {
        perror("Error: ");
        close(client_sock);
        return 0;
    }

    // Step 3: Communicate with server
    // creat thread for listening reply from server
    pthread_create(&thread_id, NULL, printMessage, NULL);

    //send message
    while (1) {
        memset(buff, '\0', (strlen(buff)) + 1);
        fgets(buff, BUFF_SIZE, stdin);
        buff[strlen(buff) - 1] = '\0';
        if (strcmp(buff, "") == 0) {
            strcpy(buff, "Exit");
            bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
            if (bytes_sent < 0) {
                perror("Error: ");
                close(client_sock);
                return 0;
            }
            // IF enter space exit
            close(client_sock);
            exit(0);
        }

        bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
        if (bytes_sent < 0) {
            perror("Error: ");
            close(client_sock);
            return 0;
        }
        // fprintf(f,"%s\n", buff);
    }
    pthread_join(thread_id, NULL);
    close(client_sock);
    return 0;
}