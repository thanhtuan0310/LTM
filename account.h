//Cau truc mot account
typedef struct student
{    
    char username[50];
    char password[10];
    char friends[10][50];
    historyMatch history[5];
    int elo;
    int puzzle_point;
    int current_puzzle;
}student;

typedef struct match
{    
    
}historyMatch;