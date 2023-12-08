#include <iostream>
#include <string>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>


#define TICK_INTERVAL 100
static Uint32 next_time;
Uint32 time_left(void)
{
    Uint32 now;

    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
// removing  "w KQkq - 0 1" for now
#define EMPTY_BOARD "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" 

using namespace std;

enum Piece{
    KING = 'K',
    QUEEN = 'Q',
    ROOK = 'R',
    BISHOP = 'B',
    KNIGHT = 'N',
    PAWN = 'P'
};

int** fen_to_board(string fen){
    // board is 2d 8x8 int array
    int** board = new int*[8];
    for (int i = 0; i < 8; i++){
        board[i] = new int[8];
    }
    int cursor = 0;
    for (char c : fen){
        if (c == '/'){
            continue;
        }
        // check if c can be converted to int
        // if so, convert it and put 0 for every place
        // else, it is a piece and put it in the board
        if (c >= '0' && c <= '9'){
            int space_count = c - '0';
            for (int i = 0; i < space_count; i++){
                board[cursor / 8][cursor % 8] = 0;
                cursor++;
            }
        } else {
            board[cursor / 8][cursor % 8] = c;
            cursor++;
        }
    }
    return board;
}


class Game {
    private:
        int** board;
        bool whites_turn = true;

        int selected_piece[2] = {-1, -1};
        int possible_moves[27][2];

        int prev_moves[100][2];

        bool quit = false;
        bool restart = false;
        int width;
        int height;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        
    public:


    public:
        Game(int width, int height){
            this->width = width;
            this->height = height;
            if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
                cout << "Error initializing SDL: " << SDL_GetError() << endl;
            }
            window = SDL_CreateWindow(
                "Chess",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                width,
                height,
                0
            );
            renderer = SDL_CreateRenderer(window, -1, 0);

            // cache image textures from "../data/white and "../data/black"



            cout << "Game initialized." << endl;

        }

        ~Game(){
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();
            cout << "Game deconstructed." << endl;
        }

        void create_chess(string fen = ""){
            if (fen == "") fen = EMPTY_BOARD;
            board = fen_to_board(fen);
            print_board();

        }

        void move_piece(int piece[2], int square[2]){

        }

        void print_board(){
            cout << endl;
            for (int y = 0; y < 8; y++){
                for (int x = 0; x < 8; x++){
                    cout << char(board[y][x]) << " ";
                }
                cout << endl;
            }
            cout << endl;
        }

        void handle_events(){
            SDL_Event event;
            while(SDL_PollEvent(&event)){
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_r:
                        restart = true;
                        break;
                    default:
                        break;
                    }
                    break;
                
                default:
                    break;
                }
            }

        }

        void render(){
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);



            SDL_RenderPresent(renderer);

        }

        void loop(){
            next_time = SDL_GetTicks() + TICK_INTERVAL;
            while (!quit)
            {
                if (restart){
                    cout << "Restarting the Game." << endl;
                    restart = false;
                    create_chess();
                }
                handle_events();
                render();
                SDL_Delay(time_left());
                next_time += TICK_INTERVAL;
            }
            cout << "Closing the Game." << endl;
        }

};


Game *game = nullptr;
int main(){
    game = new Game(WINDOW_WIDTH, WINDOW_HEIGHT);
    game->create_chess();
    game->loop();
    delete game;
    return 0;
}