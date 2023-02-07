#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

char my_username[USERNAME_SIZE];
char curr_room_name[ROOM_NAME_SIZE];
int join_succ = 0;
int curr_group_id = -1;
int playing = 0;

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

int logup(int client_socket)
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    Package pkg;

    printf("Username: ");
    scanf("%s", username);
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

    if (pkg.ctrl_signal == LOGUP_SUCC)
    {
        printf("Your account is registed successfully\n");
    }
    // strcpy(my_username, username);
    return pkg.ctrl_signal;
}

int login(int client_socket)
{
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
        sleep(1);
        MainMenu();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            // private_chat(client_socket);
            ViewInformation(client_socket);
            break;
        case 2:
            ShowPlayComputer(client_socket);
            break;
        case 3:
            ShowPlayPlayer(client_socket);
            // chat_all(client_socket);
            break;
        case 4:
            ShowChessPuzzle(client_socket);
            // chat_all(client_socket);
            break;
        case 5:
            ShowFriendMenu(client_socket);
            // chat_all(client_socket);
            break;
        case 6:
            ViewChessRank(client_socket);
            // chat_all(client_socket);
            break;
        case 7:
            ShowMatchHistoryMenu(client_socket);
            // chat_all(client_socket);
            break;
        case 8:
            ChangePassword(client_socket);
            break;
        case 9:
            login = 0;
            pkg.ctrl_signal = LOG_OUT;
            // // strcpy(pkg.sender, my_username);
            send(client_socket, &pkg, sizeof(pkg), 0);
            strcpy(my_username, "x");
            // strcpy(curr_group_name, "x");
            // curr_group_id = -1;
            sleep(1);
            break;
        case 29:
            // see_active_user(client_socket);
            break;
        // 17/01/2023
        case 30:
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

        case NOTIFY_NEW_FRIEND_REQ:
            printf("%s\n", pkg.msg);
            break;
            // case PRIVATE_CHAT:
            //     printf("%s: %s\n", pkg.sender, pkg.msg);
            //     break;

        case CHOOSE_DIFFCULT:
            printf("%s\n", pkg.msg);
            break;

        case SHOW_PLAY_PLAYER_MENU:
            printf("%s\n", pkg.msg);
            break;

        case SHOW_CHESS_PUZZLE_MENU:
            printf("%s\n", pkg.msg);
            break;

        case SHOW_FRIEND_MENU:
            printf("%s\n", pkg.msg);
            break;

        case SHOW_MATCH_HISTORY_MENU:
            printf("%s\n", pkg.msg);
            break;

        case VIEW_INFORMATION:
            printf("Your information: \n%s \n", pkg.msg);
            break;
        case VIEW_CHESS_PUZZLE_RANKING:
            printf("Chess puzzle rank: \n%s \n", pkg.msg);
            break;
        case VIEW_RANKING:
            printf("Chess rank: \n%s \n", pkg.msg);
            break;
        case CHANGE_PASS_SUCC:
            printf("%s\n", pkg.msg);
            break;
        case JOINT_ROOM_SUCC:
            printf("Joint Room %s Success\n", pkg.msg);
            strcpy(curr_room_name, pkg.msg);
            curr_group_id = pkg.group_id;
            join_succ = 1;
            // InRoom(client_socket);
            break;
        case CREATE_MATCH_SUCC:
            printf("Start Game!\n");
            printf("\n%s \n", pkg.msg);
            join_succ = 1;
            break;
        case VIEW_FRIEND:
            printf("\n%s \n", pkg.msg);
            break;
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
        case CREATE_ROOM_SUCC:
            printf("Complete create: %s \n", pkg.msg);
            curr_group_id = pkg.group_id;
            playing = 0;
            join_succ = 1;
            break;
        case LEAVE_ROOM_SUCC:
            printf("\n%s \n", pkg.msg);
            join_succ = 0;
            curr_group_id = -1;
            playing = 0;
            break;
        case SENT_FRIEND_REQUEST_SUCC:
            printf("\n%s \n", pkg.msg);
            break;
        case ADD_FRIEND_REQUEST:
            printf("\n%s \n", pkg.msg);
            break;
        case ADD_FRIEND_SUCC:
            printf("\nAdd friend sucessfully! \n");
            break;
        case START_GAME:
        printf("\n%s \n", pkg.msg);
            playing = 1;
            break;
        case PLAY_MOVE_SUCC:
            printf("\n%s \n", pkg.msg);
            break;
        case ERR_FULL_FRIEND:
            printf("Full Friend\n");
            break;
        case ERR_MOVE:
            printf("Command unknown:\n");
            break;
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
            if (result == LOGIN_SUCC)
            {
                user_use(client_socket);
            }
            else if (result == INCORRECT_ACC)
            {
                report_err(ERR_INCORRECT_ACC);
            }
            else
            {
                report_err(ERR_SIGNED_IN_ACC);
            }
            break;
        case 2:
            pkg.ctrl_signal = LOGUP_REQ;
            send(client_socket, &pkg, sizeof(pkg), 0);
            result = logup(client_socket);
            if (result == LOGUP_SUCC)
            {
                // user_use(client_socket);
                continue;
            }
            else
            {
                report_err(EXISTS_ACC);
            }
            break;
        case 3:
            pkg.ctrl_signal = QUIT_REQ;
            send(client_socket, &pkg, sizeof(pkg), 0);
            close(client_socket);
            exit(0);
        }
    }
}

// Giao diện đăng nhập
void LoginMenu()
{
    printf("------ Welcome to chess online------\n");
    printf("1. Login\n");
    printf("2. Sign up\n");
    printf("3. Exit\n\n");
}

// Menu chính
void MainMenu()
{
    printf("-------Play chess menu--------\n");
    printf("1. View information\n");
    printf("2. Play with computer\n");
    printf("3. Play with other player\n");
    printf("4. Chess puzzle\n");
    printf("5. Friends\n");
    printf("6. View chess ranking\n");
    printf("7. Match history\n");
    printf("8. Change password\n");
    printf("9. Log out\n\n");
}

// Chọn độ khó của máy
void ChooseDifficultyComputer()
{
    printf("-------Choose Difficulty--------\n");
    printf("1. Easy\n");
    printf("2. Normal\n");
    printf("3. Hard\n");
    printf("4. Return main menu\n\n");
}

void ChooseColorMenu()
{
    printf("-------Choose Color--------\n");
    printf("1. White\n");
    printf("2. Black\n");    
    printf("3. Return Choose Difficulty\n\n");
}

void PlayWithOtherPlayerMenu()
{
    printf("-------Play With Other Player--------\n");
    printf("1. Create Room\n");
    printf("2. Join Room\n");
    printf("3. Return main menu\n\n");
}

void ChessPuzzleMenu()
{
    printf("-------Chess Puzzle--------\n");
    printf("1. View chess puzzle ranking\n");
    printf("2. Play\n");
    printf("3. Return main menu\n\n");
}


void FriendMenu()
{
    printf("-------Friend--------\n");
    printf("1. View Friends List\n");
    printf("2. Add friend\n");
    printf("3. Remove friend\n");
    printf("4. Reply request friend\n");
    printf("5. Return main menu\n\n");
}

void MatchHistoryMenu()
{
    printf("-------Match history--------\n");
    printf("1. View Friends Match history\n");
    printf("2. View match history\n");
    printf("3. Return main menu\n\n");
}

void ViewInformation(int client_socket)
{
    // Ngoc
    printf("INFORMATION\n");
    Package pkg;
    pkg.ctrl_signal = VIEW_INFORMATION;
    send(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
}

void ChessPuzzle(int client_socket)
{

}

void ViewChessRank(int client_socket){
    //Ngoc
    Package pkg;
    pkg.ctrl_signal = VIEW_RANKING;
    send(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
}

void ViewChessPuzzleRank(int client_socket){
    //Ngoc
    Package pkg;
    pkg.ctrl_signal = VIEW_CHESS_PUZZLE_RANKING;
    send(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
}

void ChangePassword(int client_socket)
{
    // Ngoc
    Package pkg;
    char newPassword[PASSWORD_SIZE];
    pkg.ctrl_signal = CHANGE_PASS_REQ;
    printf("New password: ");
    scanf("%s", newPassword);
    clear_stdin_buff();
    strcpy(pkg.msg, newPassword);
    send(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
}

void ViewFriend(int client_socket)
{
    // Thai
    Package pkg;
    pkg.ctrl_signal = VIEW_FRIEND;
    strcpy(pkg.sender, my_username);
    send(client_socket, &pkg, sizeof(pkg), 0);
}

void AddFriend(int client_socket)
{
    // Thai
    Package pkg;
    char friends_name[USERNAME_SIZE];

    printf("Friends name: ");
    fgets(friends_name, USERNAME_SIZE, stdin);
    friends_name[strlen(friends_name) - 1] = '\0';

    strcpy(pkg.receiver, friends_name);
    strcpy(pkg.sender, my_username);
    strcpy(pkg.msg, my_username);
    // strcat(pkg.msg, " want to makes friend with you ");
    pkg.ctrl_signal = ADD_FRIEND;
    send(client_socket, &pkg, sizeof(pkg), 0);

    sleep(1);
}


void RemoveFriend(int client_socket)
{
    // Thai
}

void ReplyRequestFriend(int client_socket)
{
    // Thai

}

void ShowPlayComputer(int client_socket)
{
    Package pkg;
    pkg.ctrl_signal = CHOOSE_DIFFCULT;
    send(client_socket, &pkg, sizeof(pkg), 0);
    // xu ly
    int choice = 0;
    int diffcult = 0;
    while (1)
    {
        sleep(1);

        ChooseDifficultyComputer();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            // PlayWithComputer(client_socket);
            diffcult = COMPUTER_EASY;
            ShowChooseColor(client_socket, diffcult);
            // show_group(client_socket);
            break;
        case 2:
            diffcult = COMPUTER_NORMAL;
            // new_group(client_socket);
            ShowChooseColor(client_socket, diffcult);
            break;
        case 3:
            // join_group(client_socket);
            diffcult = COMPUTER_HARD;
            ShowChooseColor(client_socket, diffcult);
            break;
        default:
            return;
        }
    }
}

void ShowChooseColor(int client_socket, int diffcult)
{
    Package pkg;
    // pkg.ctrl_signal = CHOOSE_DIFFCULT;
    // send(client_socket, &pkg, sizeof(pkg), 0);
    // xu ly
    int choice = 0;    
    while (1)
    {
        sleep(1);

        ChooseColorMenu();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            PlayWithComputer(client_socket, diffcult, 0);
            break;
        case 2:
            PlayWithComputer(client_socket, diffcult, 1);             
            break;
        case 3:
                       
            break;
        default:
            return;
        }
    }
}

void ShowPlayPlayer(int client_socket)
{
    Package pkg;
    pkg.ctrl_signal = SHOW_PLAY_PLAYER_MENU;
    send(client_socket, &pkg, sizeof(pkg), 0);
    // xu ly
    int choice = 0;

    while (1)
    {
        sleep(1);

        PlayWithOtherPlayerMenu();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            CreateRoom(client_socket);
            // show_group(client_socket);
            break;
        case 2:
            JointRoom(client_socket);
            // new_group(client_socket);
            break;
        case 3:
            // join_group(client_socket);
            break;
        default:
            return;
        }
    }
}

void ShowChessPuzzle(int client_socket)
{
    Package pkg;
    pkg.ctrl_signal = SHOW_CHESS_PUZZLE_MENU;
    send(client_socket, &pkg, sizeof(pkg), 0);
    // xu ly
    int choice = 0;

    while (1)
    {
        sleep(1);

        ChessPuzzleMenu();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            // show_group(client_socket);
            ViewChessPuzzleRank(client_socket);
            break;
        case 2:
            // new_group(client_socket);
            break;
        case 3:
            // join_group(client_socket);
            break;        
        default:
            return;
        }
    }
}

void ShowFriendMenu(int client_socket)
{
    Package pkg;
    pkg.ctrl_signal = SHOW_FRIEND_MENU;
    send(client_socket, &pkg, sizeof(pkg), 0);
    // xu ly
    int choice = 0;

    while (1)
    {
        sleep(1);

        FriendMenu();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            ViewFriend(client_socket);
            break;
        case 2:
            AddFriend(client_socket);
            break;
        case 3:

            RemoveFriend(client_socket);
            break;

        default:
            return;
        }
    }
}

void ShowMatchHistoryMenu(int client_socket)
{
    Package pkg;
    pkg.ctrl_signal = SHOW_MATCH_HISTORY_MENU;
    send(client_socket, &pkg, sizeof(pkg), 0);
    // xu ly
    int choice = 0;

    while (1)
    {
        sleep(1);

        MatchHistoryMenu();
        printf("Your choice: \n");
        scanf("%d", &choice);
        clear_stdin_buff();

        switch (choice)
        {
        case 1:
            // show_group(client_socket);
            break;
        case 2:
            // new_group(client_socket);
            break;
        case 3:
            // join_group(client_socket);
            break;
        default:
            return;
        }
    }
}

void PlayWithComputer(int client_socket, int diffcult, int color){
    char diff[3], colo[3];
    Package pkg;
    pkg.ctrl_signal = CREATE_MATCH_WITH_COMPUTER;
    sprintf(diff, "%d", diffcult);
    strcpy(pkg.msg, diff);
    sprintf(colo, "%d", color);
    strcat(pkg.msg, " ");
    strcat(pkg.msg, colo);
    printf("MSG: %s\n", pkg.msg);
    send(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
    if (join_succ == 1)
    {
        InRoomWithComputer(client_socket);
    }
}

void CreateRoom(int client_socket)
{
    Package pkg;
    pkg.ctrl_signal = CREATE_ROOM;
    send(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
    if (join_succ == 1)
    {
        InRoom(client_socket);
    }
}

void JointRoom(int client_socket)
{
    // show_group(client_socket);
    // sleep(1);
    Package pkg;
    pkg.ctrl_signal = JOINT_ROOM;
    /* chon group*/
    char room_name[ROOM_NAME_SIZE];
    printf("Room Name (Room_n): \n");
    fgets(room_name, ROOM_NAME_SIZE, stdin);
    room_name[strlen(room_name) - 1] = '\0';
    strcpy(pkg.sender, my_username);
    strcpy(pkg.msg, room_name);
    send(client_socket, &pkg, sizeof(pkg), 0);
    sleep(1);
    if (join_succ == 1)
    {
        // printf("Joint success\n");
        InRoom(client_socket);
    }
    else
        return;
}

void InRoomWithComputer(int client_socket){
    Package pkg;
    int in_room = 1;
    char msg[MSG_SIZE];

    RoomTutorial();
    while (in_room)
    {    
        printf("\nYour turn: ");
		fflush(stdout);
		memset(&msg[0], 0, sizeof(msg));
		fflush(stdout);
		if (!fgets(msg, 80, stdin))
		continue;   
        if (strcmp(msg, "leave") == 0)
        {
            printf("leave\n");
            LeaveRoom(client_socket);
            in_room = 0;
            break;
        }        
        
        pkg.ctrl_signal = PLAY_MOVE;
        strcpy(pkg.msg, msg);
        send(client_socket, &pkg, sizeof(pkg), 0);
        sleep(1);
    }
}

void InRoom(int client_socket)
{
    int in_room = 1;
    char msg[MSG_SIZE] = "";    
    RoomTutorial();
    while (in_room)
    {
        while (!playing)
        {
            fgets(msg, USERNAME_SIZE, stdin);
            msg[strlen(msg) - 1] = '\0';
            if (strcmp(msg, "leave") == 0){
                in_room = 0;
                break;
            } else
        printf("%s\n", msg);              
        }
        if (strcmp(msg, "leave") == 0)
        {
            printf("leave\n");
            LeaveRoom(client_socket);
            in_room = 0;
        }
        else
        printf("%s\n", msg);
    }
}

void LeaveRoom(int client_socket)
{
    Package pkg;
    pkg.ctrl_signal = LEAVE_ROOM;
    pkg.group_id = curr_group_id;
    strcpy(pkg.sender, my_username);
    // curr_group_id = -1;
    send(client_socket, &pkg, sizeof(pkg), 0);
}

void RoomTutorial()
{
    printf("TUTORIALS\n");
    printf("Wait other player\n");
}

// main
int main()
{
    int client_socket = connect_to_server();
    ask_server(client_socket);
    return 0;
}