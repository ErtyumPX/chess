#include "game.hpp"

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800


Game *game = nullptr;
int main(){
    game = new Game(WINDOW_WIDTH, WINDOW_HEIGHT);
    game->create_board();
    game->loop(); // we can think about making the loop directly in main
    delete game;
    return 0;
}