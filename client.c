#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

char my_username[USERNAME_SIZE];

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

int login(int client_socket) {
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    Package pkg;

    printf("Username: ");
    scanf("%s", username);
    clear_stdin_buff();
    printf("Password: ");
    scanf("%s", password);
    clear_stdin_buff();
    strcpy(pkg.msg, username);
    send(client_socket, &pkg, sizeof(pkg), 0);

    recv(client_socket, &pkg, sizeof(pkg), 0);

    strcpy(pkg.msg, password);
    send(client_socket, &pkg, sizeof(pkg), 0);

    recv(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
    if (pkg.ctrl_signal == LOGIN_SUCC)
        strcpy(my_username, username);
    return pkg.ctrl_signal;
}

void user_use(int client_socket)
{
    printf("Login successfully!\n");
    int login = 1;
    int choice, result;
    Package pkg;

    pthread_t read_st;
    if (pthread_create(&read_st, NULL, read_msg, (void *)&client_socket) < 0)
    {
        report_err(ERR_CREATE_THREAD);
        exit(0);
    }
    pthread_detach(read_st);

    // see_active_user(client_socket);

    while (login)
    {
        UserMenu();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            // private_chat(client_socket);
            break;

        case 2:
            // chat_all(client_socket);
            break;

        case 3:
            // login = 0;
            // pkg.ctrl_signal = LOG_OUT;
            // // strcpy(pkg.sender, my_username);
            // send(client_socket, &pkg, sizeof(pkg), 0);
            // strcpy(my_username, "x");
            // strcpy(curr_group_name, "x");
            // curr_group_id = -1;
            // sleep(1);
            break;
        case 4:
            // see_active_user(client_socket);
            break;
        // 17/01/2023
        case 5:
            // group_chat_init(client_socket);
            break;
        default:
            printf("Ban nhap sai roi !\n");
            break;
        }
    }
}

void *read_msg(void *param)
{
    int *c_socket = (int *)param;
    int client_socket = *c_socket;
    // printf("\nmysoc: %d\n", client_socket);
    // int client_socket = my_socket;
    Package pkg;
    while (1)
    {
        recv(client_socket, &pkg, sizeof(pkg), 0);
        // printf("receive %d from server\n", pkg.ctrl_signal);
        switch (pkg.ctrl_signal)
        {
        case SHOW_USER:
            printf("Current online users: %s \n", pkg.msg);
            break;

        // case PRIVATE_CHAT:
        //     printf("%s: %s\n", pkg.sender, pkg.msg);
        //     break;

        // case CHAT_ALL:
        //     printf("%s to all: %s\n", pkg.sender, pkg.msg);
        //     break;

        // case ERR_INVALID_RECEIVER:
        //     report_err(ERR_INVALID_RECEIVER);
        //     break;
        // case MSG_SENT_SUCC:
        //     printf("Message sent!\n");
        //     break;
        // case GROUP_CHAT_INIT:
        //     printf("%s\n", pkg.msg);
        //     break;
        // case SHOW_GROUP:
        //     printf("Your group: \n%s \n", pkg.msg);
        //     break;

        // case MSG_MAKE_GROUP_SUCC:
        //     printf("Your new group: %s \n", pkg.msg);
        //     break;
        // case JOIN_GROUP_SUCC:
        //     printf("Current group: %s \n", pkg.msg);
        //     strcpy(curr_group_name, pkg.msg);
        //     curr_group_id = pkg.group_id;
        //     join_succ = 1;
        //     break;
        // case INVITE_FRIEND:
        //     printf("Attention: %s \n", pkg.msg);
        //     break;
        // case ERR_GROUP_NOT_FOUND:
        //     report_err(ERR_GROUP_NOT_FOUND);
        //     break;
        // case ERR_IVITE_MYSELF:
        //     report_err(ERR_IVITE_MYSELF);
        //     break;
        // case ERR_USER_NOT_FOUND:
        //     report_err(ERR_USER_NOT_FOUND);
        //     break;
        // case ERR_FULL_MEM:
        //     report_err(ERR_FULL_MEM);
        //     break;
        // case INVITE_FRIEND_SUCC:
        //     printf("%s\n", pkg.msg);
        //     break;
        // case GROUP_CHAT:
        //     if (curr_group_id == pkg.group_id)
        //     {
        //         printf("%s: %s\n", pkg.sender, pkg.msg);
        //     }
        //     else
        //     {
        //         printf("%s sent to Group_%d: %s\n", pkg.sender, pkg.group_id, pkg.msg);
        //     }
        //     break;
        // case SHOW_GROUP_NAME:
        //     printf("GROUP NAME: %s\n", pkg.msg);
        //     break;
        // case SHOW_GROUP_MEM:
        //     printf("%s\n", pkg.msg);
        //     break;
        // case LEAVE_GROUP_SUCC:
        //     printf("%s\n", pkg.msg);
        //     break;
        case LOG_OUT:
            sleep(1);
            pthread_exit(NULL);
            break;
        default:
            break;
        }
    }
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
            pkg.ctrl_signal = LOGIN_REQ;
            send(client_socket, &pkg, sizeof(pkg), 0);
            result = login(client_socket);
            if (result == LOGIN_SUCC) {
                user_use(client_socket);
            }
            else if (result == INCORRECT_ACC) {
                report_err(ERR_INCORRECT_ACC);
            }
            else {
                report_err(ERR_SIGNED_IN_ACC);
            }
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
    printf("3. Exit\n\n");
}

void UserMenu(){
    printf("-------Play chess menu--------\n");
    printf("1. Play with computer\n ");
    printf("2. Play with other player\n ");
    printf("3. Chess puzzle\n");
    printf("4. Add friends\n");
    printf("5. View chess ranking\n");
    printf("6. View match historyn");
    printf("7. Change password\n");
    printf("8. Log out\n\n");
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