#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// void make_server()
// {

//     int listen_socket;

//     acc_list = read_account_list();
//     listen_socket = create_listen_socket();
//     for (int i = 0; i < MAX_USER; i++)
//     {
//         user[i].socket = -1;
//         // 17/01/2023
//         for (int j = 0; j < MAX_GROUP; j++)
//             user[i].group_id[j] = -1;
//     }
//     for (int i = 0; i < MAX_GROUP; i++)
//     {
//         for (int j = 0; j < MAX_USER; j++)
//         {
//             group[i].group_member[j].socket = -1;
//         }
//         group[i].curr_num = 0;
//     }

//     printf("Server created\n");

//     while (1)
//     {

//         int conn_socket = accept_conn(listen_socket);

//         pthread_t client_thr;
//         if (pthread_create(&client_thr, NULL, pre_login_srv, (void *)&conn_socket) < 0)
//         {
//             report_err(ERR_CREATE_THREAD);
//             exit(0);
//         }
//         pthread_detach(client_thr);
//     }

//     close(listen_socket);
// }

// // main
// int main()
// {
//     make_server();
//     return 0;
// }