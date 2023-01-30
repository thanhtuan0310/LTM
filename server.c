#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

Active_user user[MAX_USER];
Group group[MAX_GROUP];
Room room[MAX_ROOM];
Puzzle puzzle[15];
node acc_list;

int create_listen_socket()
{

    int listen_socket;
    struct sockaddr_in server_addr;

    if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        report_err(ERR_SOCKET_INIT);
        exit(0);
    }

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        report_err(ERR_SOCKET_INIT);
        exit(0);
    }

    if (listen(listen_socket, MAX_USER) < 0)
    {
        report_err(ERR_SOCKET_INIT);
        exit(0);
    }

    return listen_socket;
}

int accept_conn(int listen_socket)
{

    int conn_socket;
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(struct sockaddr);

    if ((conn_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_addr_size)) < 0)
    {
        report_err(ERR_CONN_ACCEPT);
        exit(0);
    }

    return conn_socket;
}

void make_server()
{
    
    int listen_socket;

    readFileAccount(&acc_list);
    listen_socket = create_listen_socket();
    for (int i = 0; i < MAX_USER; i++)
    {
        user[i].socket = -1;
        // 17/01/2023
        for (int j = 0; j < MAX_GROUP; j++)
            user[i].group_id[j] = -1;
    }
    // for (int i = 0; i < MAX_GROUP; i++)
    // {
    //     for (int j = 0; j < MAX_USER; j++)
    //     {
    //         group[i].group_member[j].socket = -1;
    //     }
    //     group[i].curr_num = 0;
    // }

    printf("Server created\n");

    while (1)
    {

        int conn_socket = accept_conn(listen_socket);

        pthread_t client_thr;
        if (pthread_create(&client_thr, NULL, pre_login_srv, (void *)&conn_socket) < 0)
        {
            report_err(ERR_CREATE_THREAD);
            exit(0);
        }
        pthread_detach(client_thr);
    }

    close(listen_socket);
}

void *pre_login_srv(void *param)
{

    int conn_socket = *((int *)param);
    Package pkg;

    while (1)
    {

        recv(conn_socket, &pkg, sizeof(pkg), 0);

        switch (pkg.ctrl_signal)
        {
        case LOGIN_REQ:
            handle_login(conn_socket, acc_list);
            break;
        case LOGUP_REQ:
            handle_signup(conn_socket, acc_list);
            break;
        case QUIT_REQ:
            close(conn_socket);
            printf("user quit\n");
            pthread_exit(NULL);
        }
    }
}

void handle_signup(int conn_socket, node acc_list) {
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    Package pkg;
    node target_account;
    int result;

    recv(conn_socket, &pkg, sizeof(pkg), 0);
    strcpy(username, pkg.msg);

    pkg.ctrl_signal = RECV_SUCC;
    send(conn_socket, &pkg, sizeof(pkg), 0);

    recv(conn_socket, &pkg, sizeof(pkg), 0);
    strcpy(password, pkg.msg);

    printf("%s\n", username);
    printf("%s\n", password);

    target_account = search(acc_list, username);
    if(target_account) result = EXISTS_ACC;
    else result = LOGUP_SUCC;

    if(result == LOGUP_SUCC) {
        printf("Your account is registed successfully\n");
        for (int i = 0; i < MAX_USER; i++)
        {
            if (user[i].socket < 0)
            {
                strcpy(user[i].username, username);
                user[i].socket = conn_socket;
                break;
            }
        }
    } else {
        printf("This account has been existed!\n");
    }
    
    pkg.ctrl_signal = result;
    send(conn_socket, &pkg, sizeof(pkg), 0);
    if(result == LOGUP_SUCC) {
        node temp = create(username, password, 1000, 1, 0, 1, 0);
        acc_list = addtail(acc_list, temp);
        updateAccountFile(acc_list);
        addFileAccount(username);
    }
}

void handle_login(int conn_socket, node acc_list)
{

    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    Package pkg;
    node target_acc;
    int result;

    recv(conn_socket, &pkg, sizeof(pkg), 0);
    strcpy(username, pkg.msg);

    pkg.ctrl_signal = RECV_SUCC;
    send(conn_socket, &pkg, sizeof(pkg), 0);

    recv(conn_socket, &pkg, sizeof(pkg), 0);
    strcpy(password, pkg.msg);

    printf("%s\n", username);
    printf("%s\n", password);

    target_acc = search(acc_list, username);
    if (target_acc)
    {
        if (target_acc->is_signed_in)
        {
            result = SIGNED_IN_ACC;
        }
        else
        {
            if (strcmp(target_acc->password, password) == 0)
            {
                result = LOGIN_SUCC;
            }
            else
            {
                result = INCORRECT_ACC;
            }
        }
    }
    else
    {
        result = INCORRECT_ACC;
    }

    if (result == LOGIN_SUCC)
    {
        printf("login success\n");
        target_acc->is_signed_in = 1;

        for (int i = 0; i < MAX_USER; i++)
        {
            if (user[i].socket < 0)
            {
                strcpy(user[i].username, username);
                user[i].socket = conn_socket;
                sv_update_port_group(&user[i], group);
                break;
            }
        }
    }
    else if (result == SIGNED_IN_ACC)
    {
        printf("already signed in acc\n");
    }
    else
    {
        printf("incorrect acc\n");
    }

    pkg.ctrl_signal = result;
    send(conn_socket, &pkg, sizeof(pkg), 0);
    if (result == LOGIN_SUCC)
        sv_user_use(conn_socket);
}

void sv_user_use(int conn_socket)
{

    Package pkg;
    int login = 1;
    while (login)
    {
        if (recv(conn_socket, &pkg, sizeof(pkg), 0) > 0) // printf("Receive from %d\n", conn_socket);
            printf("%d chooses %d \n", conn_socket, pkg.ctrl_signal);
        switch (pkg.ctrl_signal)
        {
        case PRIVATE_CHAT:
            sv_private_chat(conn_socket, &pkg);
            break;

        case CHOOSE_DIFFCULT:
            ChooseDiffcultServer(conn_socket, &pkg);
            break;

        case SHOW_PLAY_PLAYER_MENU:
            ShowPlayWithPlayer(conn_socket, &pkg);
            break;
        
        case SHOW_CHESS_PUZZLE_MENU:
            ShowChessPuzzleServer(conn_socket, &pkg);
            break;
        
        case SHOW_FRIEND_MENU:
            ShowFriendServer(conn_socket, &pkg);
            break;
        
        case SHOW_MATCH_HISTORY_MENU:
            ShowMatchHistoryServer(conn_socket, &pkg);
            break;
        case VIEW_INFORMATION:
            ViewInformationServer(conn_socket, &pkg);
            break;
        case VIEW_CHESS_PUZZLE_RANKING:
            ViewChessPuzzleRankServer(conn_socket, &pkg);
            break;
        case VIEW_RANKING:
            ViewChessRankServer(conn_socket, &pkg);
            break;
        case CHANGE_PASS_REQ:
            ChangePassServer(conn_socket, &pkg);
            break;
        case CREATE_ROOM:
            CreateRoomServer(conn_socket, &pkg);
            break;
        case SHOW_USER:
            sv_active_user(conn_socket, &pkg);
            break;
        
        case LOG_OUT:
            login = 0;
            sv_logout(conn_socket, &pkg);
            break;
        // case GROUP_CHAT_INIT:
        //     sv_group_chat_init(conn_socket, &pkg);
        //     break;
        // case SHOW_GROUP:
        //     sv_show_group(conn_socket, &pkg);
        //     break;
        // case NEW_GROUP:
        //     sv_new_group(conn_socket, &pkg);
        //     break;
        case JOINT_ROOM:
            JointRoomServer(conn_socket, &pkg);
            break;
        case VIEW_FRIEND:
            ViewFriendServer(conn_socket, &pkg);        
            break;
        case ADD_FRIEND:
            AddFriendServer(conn_socket, &pkg);
            break;
        case REMOVE_FRIEND:
            RemoveFriendServer(conn_socket, &pkg);
            break;
        // case HANDEL_GROUP_MESS:
        //     // hien ra thong tin phong
        //     break;
        // case INVITE_FRIEND:
        //     sv_invite_friend(conn_socket, &pkg);
        //     break;
        // case GROUP_CHAT:
        //     sv_group_chat(conn_socket, &pkg);
        //     break;
        // case GROUP_INFO:
        //     sv_show_group_info(conn_socket, &pkg);
        //     break;
        // case LEAVE_GROUP:
        //     sv_leave_group(conn_socket, &pkg);
        //     break;
        default:
            break;
        }
        printf("Done %d of %d\n", pkg.ctrl_signal, conn_socket);
    }

    for (int i = 0; i < MAX_USER; i++)
    {
        if (user[i].socket == conn_socket)
        {
            node target_acc = search(acc_list, user[i].username);
            target_acc->is_signed_in = 0;
            user[i].socket = -1;
            for (int j = 0; j < MAX_GROUP; j++)
            {
                if (user[i].group_id[j] >= 0)
                {
                    int group_id = user[i].group_id[j];
                    int user_id_group = sv_search_id_user_group(group[group_id], user[i].username);
                    if (user_id_group >= 0)
                    {
                        // printf("1\n");
                        // printf("%d %d\n", group_id, user_id_group);
                        group[group_id].group_member[user_id_group].socket = 0; // can cap nhat khi dang nhap lai
                    }
                    user[i].group_id[j] = -1;
                }
            }
            break;
        }
    }
}

void sv_active_user(int conn_socket, Package *pkg)
{

    char user_list[MSG_SIZE] = {0};
    for (int i = 0; i < MAX_USER; i++)
    {
        if (user[i].socket > 0)
        {
            strcat(user_list, user[i].username);
            int len = strlen(user_list);
            user_list[len] = ' ';
        }
    }
    strcpy(pkg->msg, user_list);
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void sv_private_chat(int conn_socket, Package *pkg)
{

    printf("%d: %s to %s: %s\n", pkg->ctrl_signal, pkg->sender, pkg->receiver, pkg->msg);

    int i = 0;

    for (i = 0; i < MAX_USER; i++)
    {
        if (strcmp(pkg->receiver, user[i].username) == 0 && user[i].socket > 0)
        {
            // recv_socket = user[i].socket;
            send(user[i].socket, pkg, sizeof(*pkg), 0);
            break;
        }
    }

    if (i == MAX_USER)
        pkg->ctrl_signal = ERR_INVALID_RECEIVER;
    else
        pkg->ctrl_signal = MSG_SENT_SUCC;

    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void sv_chat_all(int conn_socket, Package *pkg)
{
    printf("%d: %s to all: %s\n", pkg->ctrl_signal, pkg->sender, pkg->msg);

    int i = 0;

    for (i = 0; i < MAX_USER; i++)
    {
        if (user[i].socket > 0)
            send(user[i].socket, pkg, sizeof(*pkg), 0);
    }

    pkg->ctrl_signal = MSG_SENT_SUCC;

    send(conn_socket, pkg, sizeof(*pkg), 0);
}

// 17/1/2023
int search_user(int conn_socket)
{
    int i = 0;
    for (i = 0; i < MAX_USER; i++)
    {
        if (user[i].socket == conn_socket)
            return i;
    }
    return -1;
}
int sv_search_id_user(Active_user user[], char *user_name)
{
    int user_id = -1;
    int i = 0;
    for (i = 0; i < MAX_USER; i++)
    {
        if (strcmp(user[i].username, user_name) == 0 && user[i].socket >= 0)
        {
            user_id = i;
            return user_id;
        }
    }
    return -1;
}
int sv_search_id_user_group(Group group, char *user_name)
{
    int i = 0;
    for (i = 0; i < MAX_USER; i++)
    {
        if (strcmp(group.group_member[i].username, user_name) == 0)
        {
           // printf("%d %s\n", i, user_name);
            return i;
        }
    }
    return -1;
}

void sv_group_chat_init(int conn_socket, Package *pkg)
{
    strcpy(pkg->msg, "CHUC NANG CHAT NHOM\n");
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void sv_show_group(int conn_socket, Package *pkg)
{
    int user_id = search_user(conn_socket);
    char group_list[MSG_SIZE] = {0};
    int group_id;
    for (int i = 0; i < MAX_GROUP; i++)
    {
        if (user[user_id].group_id[i] >= 0)
        {
            int group_id = user[user_id].group_id[i];
            strcat(group_list, group[group_id].group_name);
            int len = strlen(group_list);
            group_list[len] = '\n';
        }
    }
    strcpy(pkg->msg, group_list);
    send(conn_socket, pkg, sizeof(*pkg), 0);
}
// new group

int check_user_in_group(Active_user user, int group_id)
{
    int i = 0;
    for (i = 0; i < MAX_GROUP; i++)
    {
        if (user.group_id[i] == group_id)
            return 1;
    }
    return 0;
}
int sv_add_group_user(Active_user *user, int group_id)
{
    for (int i = 0; i < MAX_GROUP; i++)
    {
        if (user->group_id[i] < 0)
        {
            user->group_id[i] = group_id;
            return 1;
        }
    }
    return 0;
}

int sv_add_user(Active_user user, Group *group)
{
    int i = 0;
    for (i = 0; i < MAX_USER; i++)
    {
        if (group->group_member[i].socket < 0)
        {
            group->group_member[i].socket = user.socket;
            strcpy(group->group_member[i].username, user.username);
            group->curr_num++;
            return i;
        }
    }
    return 0;
}

void print_members(Group group)
{
    printf("MEMBERS OF GROUP %s: \n", group.group_name);
    for (int i = 0; i < MAX_USER; i++)
    {
        if (group.group_member[i].socket > 0)
        {
            printf("%s\n", group.group_member[i].username);
        }
    }
}
void sv_new_group(int conn_socket, Package *pkg)
{
    int user_id = search_user(conn_socket);
    int group_id = -1;
    for (int i = 0; i < MAX_GROUP; i++)
    {
        if (group[i].curr_num == 0)
        {
            group_id = i;
            sv_add_group_user(&user[user_id], group_id);
            sv_add_user(user[user_id], &group[i]);
            sprintf(group[i].group_name, "Group_%d", group_id);
            break;
        }
    }
    strcpy(pkg->msg, group[group_id].group_name);
    pkg->ctrl_signal = MSG_MAKE_GROUP_SUCC;
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

// join group
int sv_search_id_group(Group group[], Active_user user, char *group_name)
{
    int i;
    int group_id = -1;
    for (i = 0; i < MAX_GROUP; i++)
    {
        if (user.group_id[i] >= 0)
        {
            group_id = user.group_id[i];
            if (strcmp(group[group_id].group_name, group_name) == 0)
            {
                // printf("%s\n",group[i].group_name);
                return group_id;
            }
        }
    }
    return -1;
}

void sv_join_group(int conn_socket, Package *pkg)
{
    char group_name[GROUP_NAME_SIZE];
    int group_id = -1;
    int user_id = -1;

    user_id = search_user(conn_socket);
    strcpy(group_name, pkg->msg);
    group_id = sv_search_id_group(group, user[user_id], group_name);
    if (group_id >= 0)
    {
        printf("%s JOIN GROUP %s\n", pkg->sender, group[group_id].group_name);
        strcpy(pkg->msg, group_name);
        pkg->ctrl_signal = JOIN_GROUP_SUCC;
        pkg->group_id = group_id;
        send(conn_socket, pkg, sizeof(*pkg), 0);
    }
    else
    {
        pkg->ctrl_signal = ERR_GROUP_NOT_FOUND;
        send(conn_socket, pkg, sizeof(*pkg), 0);
    }
}

void sv_invite_friend(int conn_socket, Package *pkg)
{
    char friend_name[USERNAME_SIZE];
    int user_id = search_user(conn_socket);
    int friend_id;
    int group_id;

    group_id = pkg->group_id;
    strcpy(friend_name, pkg->receiver);
    friend_id = sv_search_id_user(user, friend_name);
    if (friend_id >= 0)
    {
        if (friend_id == user_id)
        {
            pkg->ctrl_signal = ERR_IVITE_MYSELF;
            send(conn_socket, pkg, sizeof(*pkg), 0);
            return;
        }
        else if (group[group_id].curr_num > MAX_USER - 1)
        {
            pkg->ctrl_signal = ERR_FULL_MEM;
            send(conn_socket, pkg, sizeof(*pkg), 0);
            return;
        }
        else if (check_user_in_group(user[friend_id], group_id))
        {
            pkg->ctrl_signal = ERR_FULL_MEM;
            send(conn_socket, pkg, sizeof(*pkg), 0);
            return;
        }
        else // thanh cong
        {
            send(user[friend_id].socket, pkg, sizeof(*pkg), 0);
            printf("%s add %s to %s\n", user[user_id].username,
                   user[friend_id].username, group[group_id].group_name);
            sv_add_group_user(&user[friend_id], group_id);
            sv_add_user(user[friend_id], &group[group_id]);

            strcpy(pkg->msg, "Successful invite");
            pkg->ctrl_signal = INVITE_FRIEND_SUCC;
            send(conn_socket, pkg, sizeof(*pkg), 0);
        }
    }

    else
    {
        pkg->ctrl_signal = ERR_USER_NOT_FOUND;
        send(conn_socket, pkg, sizeof(*pkg), 0);
        return;
    }
}

// chat trong nhom
void sv_group_chat(int conn_socket, Package *pkg)
{
    int group_id = pkg->group_id;

    int i = 0;
    for (i = 0; i < MAX_USER; i++)
    {
        if (group[group_id].group_member[i].socket > 0 && group[group_id].group_member[i].socket != conn_socket)
        {
            send(group[group_id].group_member[i].socket, pkg, sizeof(*pkg), 0);
        }
    }
    pkg->ctrl_signal = MSG_SENT_SUCC;
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

// group info
void sv_show_group_info(int conn_socket, Package *pkg)
{
    int group_id = pkg->group_id;
    printf("Group name: %s\n", group[group_id].group_name);
    // gui group name
    strcpy(pkg->msg, group[group_id].group_name);
    pkg->ctrl_signal = SHOW_GROUP_NAME;
    send(conn_socket, pkg, sizeof(*pkg), 0);

    // gui ten thanh vien
    print_members(group[group_id]);

    sprintf(pkg->msg, "NUMBER OF MEMBERS: %d", group[group_id].curr_num);
    pkg->ctrl_signal = SHOW_GROUP_MEM;
    send(conn_socket, pkg, sizeof(*pkg), 0);

    strcpy(pkg->msg, "MEMBERS OF GROUP:");
    pkg->ctrl_signal = SHOW_GROUP_MEM;
    send(conn_socket, pkg, sizeof(*pkg), 0);
    for (int i = 0; i < MAX_USER; i++)
    {
        if (group[group_id].group_member[i].socket >= 0)
        {
            strcpy(pkg->msg, group[group_id].group_member[i].username);
            pkg->ctrl_signal = SHOW_GROUP_MEM;
            send(conn_socket, pkg, sizeof(*pkg), 0);
        }
    }
}

// thoat nhom
void sv_leave_group(int conn_socket, Package *pkg)
{
    int group_id = pkg->group_id;
    int user_id = search_user(conn_socket);
    int i = 0;
    for (i = 0; i < MAX_USER; i++)
    {
        Member mem = group[group_id].group_member[i];
        if (strcmp(mem.username, user[user_id].username) == 0)
        {
            group[group_id].group_member[i].socket = -1;
            group[group_id].curr_num--;
            if (sv_leave_group_user(&user[user_id], group_id))
            {
                // gui thong bao den cho moi nguoi
                strcpy(pkg->msg, "LEAVE GROUP ");
                pkg->ctrl_signal = GROUP_CHAT;
                sv_group_chat(conn_socket, pkg);

                // gui lai cho user
                strcpy(pkg->msg, "LEAVE GROUP SUCCESS: ");
                strcat(pkg->msg, group[group_id].group_name);
                pkg->ctrl_signal = LEAVE_GROUP_SUCC;
                send(conn_socket, pkg, sizeof(*pkg), 0);
            }
        }
    }
}

int sv_leave_group_user(Active_user *user, int group_id)
{
    for (int i = 0; i < MAX_GROUP; i++)
    {
        if (user->group_id[i] == group_id)
        {
            user->group_id[i] = -1;
            return 1;
        }
    }
    return 0;
}

void sv_update_port_group(Active_user *user, Group *group)
{
    int i = 0;
    int user_id_port;
    for (i = 0; i < MAX_GROUP; i++)
    {
        user_id_port = sv_search_id_user_group(group[i], user->username);
        if (user_id_port >= 0)
        {
            sv_add_group_user(user, i);
            group[i].group_member[user_id_port].socket = user->socket;
        }
    }
}


void sv_logout(int conn_socket, Package *pkg)
{
    printf("%d logout\n", conn_socket);
    pkg->ctrl_signal = LOG_OUT;
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void ViewInformationServer(int conn_socket, Package *pkg){
    //Ngoc
}

void ChessPuzzleServer(int conn_socket, Package *pkg){
    
}

void ViewChessRankServer(int conn_socket, Package *pkg){
    //Ngoc
}

void ViewChessPuzzleRankServer(int conn_socket, Package *pkg){
    //Ngoc
}

void ChangePassServer(int conn_socket, Package *pkg){
    //Ngoc   
}
void ChooseDiffcultServer(int conn_socket, Package *pkg){
    strcpy(pkg->msg, "Diffcult\n");
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void ShowPlayWithPlayer(int conn_socket, Package *pkg){
    strcpy(pkg->msg, "Player\n");
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void ShowChessPuzzleServer(int conn_socket, Package *pkg){
    strcpy(pkg->msg, "Chess Puzzle\n");
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void ShowFriendServer(int conn_socket, Package *pkg){
    strcpy(pkg->msg, "Friend\n");
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void ShowMatchHistoryServer(int conn_socket, Package *pkg){
    strcpy(pkg->msg, "Match\n");
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

void CreateRoomServer(int conn_socket, Package *pkg)
{
    int user_id = search_user(conn_socket);
    int room_id = -1;
    for (int i = 0; i < MAX_ROOM; i++)
    {
        if (room[i].curr_num == 0)
        {
            room_id = i;
            AddRoom(&user[user_id], room_id);
            AddPlayerInRoom(user[user_id], &room[i]);
            sprintf(room[i].name, "Room_%d", room_id);
            break;
        }
    }
    strcpy(pkg->msg, room[room_id].name);
    pkg->ctrl_signal = CREATE_ROOM_SUCC;
    send(conn_socket, pkg, sizeof(*pkg), 0);
}

int AddRoom(Active_user *user, int room_id)
{
    for (int i = 0; i < MAX_ROOM; i++)
    {
        if (user->room_id[i] < 0)
        {
            user->room_id[i] = room_id;
            return 1;
        }
    }
    return 0;
}

int AddPlayerInRoom(Active_user user, Room *room)
{
    int i = 0;
    for (i = 0; i < MAX_USER; i++)
    {
        if (room->member[i].socket < 0)
        {
            room->member[i].socket = user.socket;
            strcpy(room->member[i].username, user.username);
            room->curr_num++;
            return i;
        }
    }
    return 0;
}

void JointRoomServer(int conn_socket, Package *pkg)
{
    char room_name[ROOM_NAME_SIZE];
    int room_id = -1;
    int user_id = -1;

    user_id = search_user(conn_socket);
    strcpy(room_name, pkg->msg);
    room_id = SearchRoom(room, user[user_id], room_name);
    if (room_id >= 0)
    {
        printf("%s JOIN Room %s\n", pkg->sender, room[room_id].name);
        strcpy(pkg->msg, room_name);
        pkg->ctrl_signal = JOINT_ROOM_SUCC;
        pkg->group_id = room_id;
        send(conn_socket, pkg, sizeof(*pkg), 0);
    }
    else
    {
        pkg->ctrl_signal = ERR_ROOM_NOT_FOUND;
        send(conn_socket, pkg, sizeof(*pkg), 0);
    }
}

int SearchRoom(Room room[], Active_user user, char *name)
{
    int i;
    int room_id = -1;
    for (i = 0; i < MAX_ROOM; i++)
    {
        if (user.room_id[i] >= 0)
        {
            room_id = user.room_id[i];
            if (strcmp(room[room_id].name, name) == 0)
            {
                // printf("%s\n",group[i].group_name);
                return room_id;
            }
        }
    }
    return -1;
}

void ViewFriendServer(int conn_socket, Package *pkg){
    //Thai
}

void AddFriendServer(int conn_socket, Package *pkg){
    //Thai
}

void RemoveFriendServer(int conn_socket, Package *pkg){
     //Thai
}

// main
int main()
{
    make_server();
    return 0;
}