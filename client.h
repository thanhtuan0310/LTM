#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "server.h"
#include "network.h"
#include "error.h"
#include "util.h"

//* Menu
/**
 * Menu đăng nhập
 * 1. Login
 * 2. CreateAccount
 * 3. Quit
*/
void LoginMenu();

/**
  Giao diện menu chính
*/
void UserMenu();


/**
 Chọn độ khó của máy
*/
void ChooseDifficultyComputer();

/**
 Giao diện chơi với người
*/
void PlayWithOtherPlayerMenu();

/**
 Giao diện giải thế cờ
*/
void ChessPuzzleMenu();

/**
 Menu bạn bè
*/
void FriendMenu();

/**
 Menu lịch sử đấu
*/
void MatchHistoryMenu();

/**
 Giải thế cờ
*/
void ChessPuzzle();

/**
 Xem bảng xếp hạng theo elo
*/
void ViewChessRank();

/**
 Xem bảng xếp hạng theo điểm chess puzzle
*/
void ViewChessPuzzleRank();



/**
 * Đăng nhập với tên đăng nhập và mật khẩu
 * @param client_socket socket đã kết nối đến server
 * @return 0: đăng nhập thất bại
 * @return 1: đăng nhập thành công
*/
int login(int client_socket);

//* Chức năng sau đăng nhập
/**
 * Cho người dùng nhập lựa chọn, thực hiện chức năng tương ứng
 * Phân luồng đọc-ghi
 * @param client_socket socket đã kết nối đến server
*/
void user_use(int client_socket);

/**
 * Đọc nội dung tin nhắn
 * @param param socket kết nối đến server
*/

/**
 * Xem thông tin cá nhân 
*/
void ViewInformation();

//* Chức năng sau đăng nhập
/**
 * Cho người dùng nhập lựa chọn, thực hiện chức năng tương ứng 
 * @param client_socket socket đã kết nối đến server
*/
void ShowPlayComputer(int client_socket);

void ShowPlayPlayer(int client_socket);

void ShowChessPuzzle(int client_socket);

void ShowFriendMenu(int client_socket);

void ShowMatchHistoryMenu(int client_socket);



void *read_msg(void *param);

#endif

