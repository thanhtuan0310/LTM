#ifndef __SERVER_H__
#define __SERVER_H__

#include "network.h"
#include "error.h"
#include "account_manager.h"
#include "defs.h"


#define WAC1 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define PERFT "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_USER 30
#define MAX_GROUP 10
#define MAX_ROOM 10
#define EMPTY_STRING "EMPTY_STRING"
#define GROUP_NAME_SIZE 30
#define ROOM_NAME_SIZE 30

//* Người dùng hoạt động
typedef struct Active_user_ {
    char username[USERNAME_SIZE]; /* Tên đăng nhập của người dùng */
    int socket; /* Socket người dùng dùng để kết nối đến server */
    int group_id[MAX_GROUP]; /*Group hien tai*/ 
    int room_id;
    int computer_id; /*Id dùng để chơi với máy hiện tại*/
} Active_user;

//* Group
typedef struct Member_{
    char username[USERNAME_SIZE]; /* Tên đăng nhập của người dùng */
    int socket; /* Socket người dùng dùng để kết nối đến server */
} Member;

typedef struct Group_ {
    Member group_member[MAX_USER]; /* Thành viên trong nhóm */
    int curr_num; /* Số người hiện tại trong nhóm */
    char group_name[GROUP_NAME_SIZE]; 
} Group;

typedef struct Room {
    Member member[MAX_USER]; /* Số người trong phòng */
    int curr_num; /* Số người hiện tại trong phòng */
    char name[GROUP_NAME_SIZE]; 
} Room;

typedef struct Puzzle {
    char board[9][1024];
    char move[10]; 
} Puzzle;


//* Khởi tạo server
/**
 * Tạo socket của server lắng nghe kết nối từ client
 * @return file descriptor của socket
*/
int create_listen_socket();

/**
 * Chấp nhận kết nối từ client
 * @param listen_socket socket lắng nghe kết nối
 * @return file descriptor của socket đã kết nối
*/
int accept_conn(int listen_socket);

/**
 * Đọc danh sách tài khoản
 * Tạo listen socket và chấp nhận kết nối
 * Phân luồng cho từng user
*/
void make_server();

//* Trước khi người dùng đăng nhập
/**
 * Tiếp nhận các yêu cầu: đăng nhập, thoát,...
 * @param param socket kết nối đến client
*/
void *pre_login_srv(void *param);

/**
 * Nhận tên đăng nhập, mật khẩu và kiểm tra
 * @param conn_socket socket kết nối đến client
 * @param acc_list danh sách tài khoản
*/
void handle_login(int conn_socket, Account *acc_list);

void handle_signup(int conn_socket, node acc_list);
/**
 * Search user by conn_socket
 * @param conn_socket socket kết nối đến client
 * @param acc_list danh sách tài khoản
*/
int search_user(int conn_socket);


//* Sau khi người dùng đăng nhập
void sv_user_use(int conn_socket);

/**
 * Gửi cho người dùng danh sách người dùng đang online
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_active_user(int conn_socket, Package *pkg);

/**
 * Xử lý chức năng chat riêng
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_private_chat(int conn_socket, Package *pkg);

/**
 * Xử lý chức năng chat all
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_chat_all(int conn_socket, Package *pkg);

/**
 * Xử lý chức năng chat nhóm
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_group_chat_init(int conn_socket, Package *pkg);

/**
 * Gửi cho người dùng nhóm hiện tại
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_show_group(int conn_socket, Package *pkg);

/**
 * Tạo group mới
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_new_group(int conn_socket, Package *pkg);

/**
 * Thêm group cho user
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
int sv_add_group_user(Active_user *user, int group_id);

/**
 * Xoa group cho user
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
int sv_leave_group_user(Active_user *user, int group_id);

/**
 * Thêm user vao group
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
int sv_add_user(Active_user user, Group *group);

/**
 * user join group
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_join_group(int conn_socket, Package *pkg);

/**
 * Tìm ID group theo tên
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
int  sv_search_id_group(Group group[],Active_user user, char *group_name);

/**
 * Tìm ID user theo tên
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
int  sv_search_id_user(Active_user user[], char *user_name);

/**
 * Tìm ID user theo tên
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
int  sv_search_id_user_group(Group group, char *user_name);

/**
 * Moi ban
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_invite_friend(int conn_socket, Package *pkg);

/**
 * Chat trong nhom
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_group_chat(int conn_socket, Package *pkg);

/**
 * Hiển thị thông tin phòng
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_show_group_info(int conn_socket, Package *pkg);

/**
 * Thoat phòng
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_leave_group(int conn_socket, Package *pkg);

/**
 * Cap nhat cong cac group khi moi dang nhap
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_update_port_group(Active_user *user, Group *group);

/**
 * In ra thanh vien cua nhom
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void print_members(Group group);
/**
 * Kiem tra nguoi dung co o trong nhom khong
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
int check_user_in_group(Active_user user, int group_id);
/**
 * Xử lý chức năng đăng xuất
 * @param conn_socket socket kết nối đến client
 * @param pkg con trỏ đến gói tin nhận được từ client
*/
void sv_logout(int conn_socket, Package *pkg);

/**
 * Xử lý chức năng hiển thị thông tin cá nhân
*/
void ViewInformationServer(int conn_socket, Package *pkg);

/**
* Lưu thông tin các thế cờ
*/
void readFileChessPuzzle();

/**
* In thế cờ
*/
void printPuzzle();

/**
 * Xử lý chức năng giải thế cờ
*/
void ChessPuzzleServer(int conn_socket, Package *pkg);

/**
 Xử lý bảng xếp hạng theo elo
*/
void ViewChessRankServer(int conn_socket, Package *pkg);

/**
Xử lý bảng xếp hạng theo điểm chess puzzle
*/
void ViewChessPuzzleRankServer(int conn_socket, Package *pkg);

/**
Chọn độ khó của máy
*/
void ChooseDiffcultServer(int conn_socket, Package *pkg);

/**
Xử lý đổi mật khẩu
*/
void ChangePassServer(int conn_socket, Package *pkg);

/**
Lấy thông tin user theo elo và puzzle_point
*/
void getListUserRanking(node head);

/**
Sắp xếp user theo elo và puzzle_point
*/
void sortUserRanking(int type);

/**
Chơi với người
*/
void ShowPlayWithPlayer();

void ShowChessPuzzleServer(int conn_socket, Package *pkg);

void ShowFriendServer(int conn_socket, Package *pkg);

void ShowMatchHistoryServer(int conn_socket, Package *pkg);

void CreateMatchWithPlayer(int conn_socket, Package *pkg);

void PlayWithPlayer(int conn_socket, Package *pkg);

void LeavePlayComputerServer(int conn_socket, Package *pkg);

void CreateRoomServer(int conn_socket, Package *pkg);

int AddRoom(Active_user *user, int room_id);

int AddPlayerInRoom(Active_user user, Room *room);

void JointRoomServer(int conn_socket, Package *pkg);

void LeaveRoomServer(int conn_socket, Package *pkg);

void ViewFriendServer(int conn_socket, Package *pkg);

void AddFriendServer(int conn_socket, Package *pkg);

void RemoveFriendServer(int conn_socket, Package *pkg);

void DeleteFriendRequest(node account_friend, char sender_name[]);

int CheckWaitRequestFriend(node account_friend, char sender_name[]);

void ReplyFriendServer(int conn_socket, Package *pkg);

int SearchRoom(Room room[], Active_user user, char *name);

void CreateNewBoard();

node getAccountBySocket(int conn_socket);
void CheckFriendRequest(node user, int conn_socket);
#endif