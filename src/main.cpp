#include "game.hpp"

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800


Game *game = nullptr;
int main(){
    game = new Game(WINDOW_WIDTH, WINDOW_HEIGHT);
    game->create_board();
    game->loop(); // bu biraz kötü olmuş ya loop'u main içerisinde kurmak daha tatlı bence
    delete game;
    return 0;
}