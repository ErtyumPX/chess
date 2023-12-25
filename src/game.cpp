#include <fstream>

#include "game.hpp"

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

// removing  "w KQkq - 0 1" for now
#define EMPTY_BOARD "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" 
#define WHITE_PIECE_DIR "/home/erthium/Projects/chess/assets/white/{}.png"
#define BLACK_PIECE_DIR "/home/erthium/Projects/chess/assets/black/{}.png"


Game::Game(){

}


Game::~Game(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    for (int i = 0; i < 8; i++){
        delete[] board[i];
    }
    delete[] board;
    cout << "Game deconstructed." << endl;
}


void Game::initialize(int width, int height){
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


void Game::create_board(string fen){
    if (fen == "") fen = EMPTY_BOARD;
    board = fen_to_board(fen);
    is_whites_turn = true;
    update_move_info();
}


void Game::print_board(){
    cout << endl;
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            cout << char(board[x][y]) << " ";
        }
        cout << endl;
    }
    cout << endl;
}


int Game::mouse_to_square(int mouse_x, int mouse_y){
    int square_x = (mouse_x - board_x) / board_unit;
    int square_y = (mouse_y - board_y) / board_unit;
    if (square_x < 0 || square_x > 7 || square_y < 0 || square_y > 7) return -1;
    return square_x * 8 + square_y;
}


void Game::select_piece(int piece[2]){
    if (is_white_piece(board[piece[0]][piece[1]]) != is_whites_turn) return;
    selected_piece[0] = piece[0];
    selected_piece[1] = piece[1];
}


void Game::handle_left_mouse(){
    int square_vector[2] = {current_square / 8, current_square % 8};
    int piece = board[square_vector[0]][square_vector[1]];
    if (selected_piece[0] != -1){
        if (selected_piece[0] != square_vector[0] || selected_piece[1] != square_vector[1]){
            int new_square[2] = {current_square / 8, current_square % 8};
            // TODO: check if move is valid
            move_selected(new_square);
        }
    }
    else if (piece != 0){
        select_piece(square_vector);
    }
}


void Game::handle_events(){
    // mouse events
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    current_square = mouse_to_square(mouse_x, mouse_y);
    
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
                if (current_square != -1){
                    handle_left_mouse();
                }
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_r:
                restart = true;
                break;
            case SDLK_LEFT:
                take_back();
                break;
            case SDLK_RIGHT:
                go_forward();
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


SDL_Texture* Game::get_texture(int piece){
    if (piece >= 'A' && piece <= 'Z'){
        return white_pieces[piece_to_int(piece)];
    } else {
        return black_pieces[piece_to_int(piece)];
    }
}


void Game::render(){
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
            if (board[x][y] == 0) continue;
            // Draw the figure
            SDL_Texture* texture = get_texture(board[x][y]);
            SDL_RenderCopy(renderer, texture, NULL, &current_rect);

        }
    }
    if (selected_piece[0] != -1){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
        current_rect = {board_x + selected_piece[0] * board_unit, board_y + selected_piece[1] * board_unit, board_unit, board_unit};
        SDL_RenderFillRect(renderer, &current_rect);
        Move move = move_info.take(selected_piece);
        for (int i = 0; i < move_info.p_size(selected_piece); i++){
            int x = move.possible[i][0];
            int y = move.possible[i][1];
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
            current_rect = {board_x + x * board_unit, board_y + y * board_unit, board_unit, board_unit};
            SDL_RenderFillRect(renderer, &current_rect);
        }
    }

    SDL_RenderPresent(renderer);

}


void Game::loop(){
    next_time = SDL_GetTicks() + TICK_INTERVAL;
    while (!quit)
    {
        if (restart){
            cout << "Restarting the Game." << endl;
            restart = false;
            create_board();
        }
        handle_events();
        render();
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
    cout << "Closing the Game." << endl;
}
