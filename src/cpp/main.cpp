#include <iostream>
#include <string>
#include <fstream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

using namespace std;

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

// 118,150,86,255
// 238,238,210,255

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

// removing  "w KQkq - 0 1" for now
#define EMPTY_BOARD "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" 

#define WHITE_PIECE_DIR "/home/erthium/Projects/chess/src/data/white/{}.png"
#define BLACK_PIECE_DIR "/home/erthium/Projects/chess/src/data/black/{}.png"


enum Piece{
    WHITE_KING = 'K',
    WHITE_QUEEN = 'Q',
    WHITE_ROOK = 'R',
    WHITE_BISHOP = 'B',
    WHITE_KNIGHT = 'N',
    WHITE_PAWN = 'P',
    BLACK_KING = 'k',
    BLACK_QUEEN = 'q',
    BLACK_ROOK = 'r',
    BLACK_BISHOP = 'b',
    BLACK_KNIGHT = 'n',
    BLACK_PAWN = 'p'
};

int piece_to_int(char piece){
    if (piece == 'K' || piece == 'k') return 0;
    if (piece == 'Q' || piece == 'q') return 1;
    if (piece == 'R' || piece == 'r') return 2;
    if (piece == 'B' || piece == 'b') return 3;
    if (piece == 'N' || piece == 'n') return 4;
    if (piece == 'P' || piece == 'p') return 5;
    return -1;
}

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
    private: // private variables
        // window Rendering
        int width;
        int height;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        // board Rendering
        SDL_Texture* white_pieces[6];
        SDL_Texture* black_pieces[6];
        int** board;
        int board_x;
        int board_y;
        int board_width;
        int board_unit;
        
        // game Logic
        bool whites_turn = true;
        int selected_piece[2] = {-1, -1};
        int possible_moves[27][2];
        int prev_moves[100][2];

        // game Loop
        bool quit = false;
        bool restart = false;
    
    public: // public variables


    public: // public methods
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
            board_width = width;
            board_unit = height / 8;
            board_x = 0;
            board_y = 0;
            // cache image textures from "../data/white and "../data/black"
            for (int i = 0; i < 6; i++){
                string path = WHITE_PIECE_DIR;
                path.replace(path.find("{}"), 2, string(1, char(i + '0')));
                SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
                white_pieces[i] = texture;
            }
            for (int i = 0; i < 6; i++){
                string path = BLACK_PIECE_DIR;
                path.replace(path.find("{}"), 2, string(1, char(i + '0')));
                SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
                black_pieces[i] = texture;
            }

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
        }

        void move_piece(int piece[2], int square[2]){
            board[square[0]][square[1]] = board[piece[0]][piece[1]];
            board[piece[0]][piece[1]] = 0;
        }

        void move_selected(int square[2]){
            move_piece(selected_piece, square);
            selected_piece[0] = -1;
            selected_piece[1] = -1;
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
        
        int mouse_to_square(int mouse_x, int mouse_y){
            int square_x = (mouse_x - board_x) / board_unit;
            int square_y = (mouse_y - board_y) / board_unit;
            if (square_x < 0 || square_x > 7 || square_y < 0 || square_y > 7) return -1;
            return square_y * 8 + square_x;
        }

        void handle_events(){
            // mouse events
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);
            int square = mouse_to_square(mouse_x, mouse_y);
            
            // SDL events
            SDL_Event event;
            while(SDL_PollEvent(&event)){
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT){
                        if (square != -1){
                            int piece = board[square / 8][square % 8];
                            if (selected_piece[0] != -1 && selected_piece[0] != square / 8 && selected_piece[1] != square % 8){
                                int new_square[2] = {square / 8, square % 8};
                                move_selected(new_square);
                            }
                            else if (piece != 0){
                                selected_piece[0] = square / 8;
                                selected_piece[1] = square % 8;
                            }
                        }
                    }
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

        SDL_Texture* get_texture(int piece){
            if (piece >= 'A' && piece <= 'Z'){
                return white_pieces[piece_to_int(piece)];
            } else {
                return black_pieces[piece_to_int(piece)];
            }
        }

        void render(){
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderClear(renderer);

            // render board
            SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255);
            SDL_Rect current_rect = {board_x, board_y, board_width, board_width};
            SDL_RenderFillRect(renderer, &current_rect);
            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    current_rect = {board_x + x * board_unit, board_y + y * board_unit, board_unit, board_unit};
                    if ((x + y) % 2 == 0){
                        SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255);
                        SDL_RenderFillRect(renderer, &current_rect);
                    }
                    if (board[y][x] == 0) continue;
                    // Draw the figure
                    SDL_Texture* texture = get_texture(board[y][x]);
                    SDL_RenderCopy(renderer, texture, NULL, &current_rect);

                }
            }
            

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