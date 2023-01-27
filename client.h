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

#endif

