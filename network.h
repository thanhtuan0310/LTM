#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "account_manager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MSG_SIZE 2048

//* Tín hiệu điều khiển
// chung
// server
#define LOGUP_SUCC 100 /* Đăng ký thành công */
#define LOGIN_SUCC 101 /* Đăng nhập thành công */
#define INCORRECT_ACC 102 /* Tài khoản hoặc mật khẩu không chính xác */
#define SIGNED_IN_ACC 103 /* Tài khoản đã được đăng nhập bởi người khác */
#define RECV_SUCC 104 /* Nhận dữ liệu thành công */
#define EXISTS_ACC 105 /* Tài khoản đã tồn tại */ 
#define CHANGE_PASS_SUCC 106 /* Đổi mật khẩu thành công*/
// client
#define LOGIN_REQ 201 /* Yêu cầu đăng nhập */
#define LOGUP_REQ 203 /* Yêu cầu đăng ký */
#define QUIT_REQ 202 /* Thoát ứng dụng */
#define CHANGE_PASS_REQ 204 /* Đổi mật khẩu */

#define MSG_SENT_SUCC 301
#define END_CHAT 302

#define SHOW_USER 401
#define PRIVATE_CHAT 402
#define CHAT_ALL 403
#define LOG_OUT 404

#define GROUP_CHAT_INIT 405
#define SHOW_GROUP 406
#define NEW_GROUP  407
#define MSG_MAKE_GROUP_SUCC 408
#define MSG_MAKE_GROUP_ERR 409
#define JOIN_GROUP 410
#define JOIN_GROUP_SUCC 411
#define HANDEL_GROUP_MESS 412
#define INVITE_FRIEND 413
#define INVITE_FRIEND_SUCC 414
#define GROUP_CHAT 415
#define GROUP_INFO 416
#define SHOW_GROUP_NAME 417
#define SHOW_GROUP_MEM 418
#define LEAVE_GROUP 419
#define LEAVE_GROUP_SUCC 420

#define VIEW_INFORMATION 501
#define VIEW_MATCH_HISTORY_FRIEND 502
#define VIEW_MATCH_HISTORY 503
#define SHOW_PLAY_PLAYER_MENU 504
#define CHOOSE_DIFFCULT 505
#define SHOW_CHESS_PUZZLE_MENU 506
#define SHOW_FRIEND_MENU 507
#define SHOW_MATCH_HISTORY_MENU 508
#define VIEW_RANKING 509
#define VIEW_CHESS_PUZZLE_RANKING 510
#define PLAY_CHESS_PUZZLE 511
#define GROUP_INFO 416
#define SHOW_GROUP_NAME 417
#define SHOW_GROUP_MEM 418
#define LEAVE_GROUP 419
#define LEAVE_GROUP_SUCC 420

//* Cấu trúc gói tin
typedef struct Package_ {
    char msg[MSG_SIZE]; /* nội dung thông điệp */
    char sender[USERNAME_SIZE]; /* username người gửi */
    char receiver[USERNAME_SIZE]; /* username người nhận */
    int group_id; /*id group muốn gửi*/
    int ctrl_signal; /* mã lệnh */
} Package;

#endif