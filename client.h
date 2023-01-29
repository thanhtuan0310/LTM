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
* 1. Play with computer
* 2. Play with other player
* 3. Chess puzzle
* 4. Add friends
* 5. View chess ranking
* 6. View match history
* 7. Change password
* 8. Log out
*/
void UserMenu();


/**
 * 1. Easy
 * 2. Normal
 * 3. Hard
*/
void ChooseDifficultyComputer();

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
void *read_msg(void *param);

#endif

