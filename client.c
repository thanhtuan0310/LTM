#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int connect_to_server()
{

    int client_socket;
    struct sockaddr_in server_addr;

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        report_err(ERR_SOCKET_INIT);
        exit(0);
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        report_err(ERR_CONNECT_TO_SERVER);
        exit(0);
    }

    return client_socket;
}

void ask_server(int client_socket)
{

    int choice, result;
    Package pkg;

    while (1)
    {
        sleep(1);
        LoginMenu();
        printf("Your choice: ");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            // pkg.ctrl_signal = LOGIN_REQ;
            // send(client_socket, &pkg, sizeof(pkg), 0);
            // result = login(client_socket);
            // if (result == LOGIN_SUCC)
            // {
            //     user_use(client_socket);
            // }
            // else if (result == INCORRECT_ACC)
            // {
            //     report_err(ERR_INCORRECT_ACC);
            // }
            // else
            // {
            //     report_err(ERR_SIGNED_IN_ACC);
            // }
            break;
        case 2:
            break;    
        case 3:
            pkg.ctrl_signal = QUIT_REQ;
            send(client_socket, &pkg, sizeof(pkg), 0);
            close(client_socket);
            exit(0);
        }
    }
}

void LoginMenu(){
 printf("------ Welcome to chess ------\n");
    printf("1. Login\n");
    printf("2. Sign up\n");
    printf("3. Exit\n");
}

void UserMenu(){

}

void ChooseDifficultyComputer(){

}

// main
int main()
{
    int client_socket = connect_to_server();
    ask_server(client_socket);
    return 0;
}