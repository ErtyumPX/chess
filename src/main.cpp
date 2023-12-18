#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

#include "game.h"

using namespace std;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

Game *game = nullptr;
int main(){
    game = new Game(WINDOW_WIDTH, WINDOW_HEIGHT);
    game->create_board();
    game->loop();
    delete game;
    return 0;
}