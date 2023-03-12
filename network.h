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
#define SHOW_USER 99
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

#define LOG_OUT 404  /* Đăng xuất */

#define VIEW_INFORMATION 501  /* Xem thông tin cá nhan */
#define VIEW_MATCH_HISTORY 503  /* Xem lịch sử đấu */
#define SHOW_PLAY_PLAYER_MENU 504 /* Hiện Menu chơi */
#define CHOOSE_DIFFCULT 505 /* Chọn độ khó */
#define SHOW_CHESS_PUZZLE_MENU 506 /* Chess Puzzle Menu */
#define SHOW_FRIEND_MENU 507 /* Friend Menu */
#define SHOW_MATCH_HISTORY_MENU 508 /* Match Menu */
#define VIEW_RANKING 509 /*Xem bảng xếp hạng */
#define VIEW_CHESS_PUZZLE_RANKING 510 /* Bảng xếp hạng thế cờ */
#define PLAY_CHESS_PUZZLE 511 /* Giải thế cờ */
#define CREATE_ROOM 512 /* Tạo phòng */
#define CREATE_ROOM_SUCC 512 /* Tạo phòng thành công */
#define JOINT_ROOM 513 /* Vào phòng */
#define JOINT_ROOM_SUCC 514 /* Vào phòng thành công */
#define VIEW_FRIEND 515 /* Xem bạn bè */
#define ADD_FRIEND 516 /* Kết bạn */
#define REMOVE_FRIEND 517 /* Xóa bạn */
#define ADD_FRIEND_REQUEST 518 /* Yêu cầu kết bạn */
#define SENT_FRIEND_REQUEST_SUCC 519 /* Gửi yêu cầu kết bạn thành công */
#define ADD_FRIEND_SUCC 520 /* Kết bạn thành công */
#define NOTIFY_NEW_FRIEND_REQ 521 /* Thông báo nếu có yêu cầu kết bạn */
#define START_GAME 522 /* Bắt đầu chơi */
#define END_GAME_WIN 523 /* Thắng trânj */
#define END_GAME_LOSE 524 /* Thua trận */
#define LEAVE_ROOM 525 /* Yêu cầu Rời phòng */
#define LEAVE_ROOM_SUCC 526 /* Rời phòng thành công */
#define PLAY_COMPUTER 527 /* Chơi với máy */
#define CREATE_MATCH_WITH_COMPUTER 528 /* Tạo trận chơi với máy */
#define CREATE_MATCH_SUCC 529 /* Tạo trận thành công */
#define PLAY_MOVE_SUCC 530 /* Di chuyển quân thành công */
#define PLAY_MOVE 531 /* Di chuyển quân */
#define COMPUTER_EASY 1 /* Dễ */
#define COMPUTER_NORMAL 3 /* Thường */
#define COMPUTER_HARD 7 /* Khó */
#define LEAVE_COMPUTER_MATCH 532 /* Rời phòng đấu với máy */
#define LEAVE_COMPUTER_MATCH_SUCC 533 /* Rời phòng đấu với máy hành công */
#define SHOW_FRIEND_REQUEST 534 /* Thông tin yêu cầu kết bạn */
#define SHOW_FRIEND_REQUEST_2 535
#define ACCEPT 536 /* Chấp nhận kết bạn */
#define NO_ACCEPT 537 /* Từ chối kết bạn */
#define SHOW_FRIEND_REQUEST_ERROR 538
#define FRIEND_REQUEST_SUCC 539
#define REMOVE_FRIEND_SUCC 540
#define CHECK_TURN_PUZZLE 541
#define CHECK_TURN_PUZZLE_SUCC 542
#define END_GAME_DRAW 543
#define WAIT_MOVE 544 /* Chờ người chơi khác di chuyển */
#define PLAY_PLAYER_MOVE 545
#define OTHER_PLAYER_MOVE 546

#define YES_TO_ADD_FRIEND 611
#define DELETE_REQUEST 612

//* Cấu trúc gói tin
typedef struct Package_ {
    char msg[MSG_SIZE]; /* nội dung thông điệp */
    char sender[USERNAME_SIZE]; /* tên người gửi */
    char receiver[USERNAME_SIZE]; /* tên người nhận */
    int group_id; /*id group muốn gửi*/
    int ctrl_signal; /* kiểu thông điệp */
} Package;

#endif