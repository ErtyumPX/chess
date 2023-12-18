#include <fstream>

#include "game.hpp"
#include "board.hpp"

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
#define WHITE_PIECE_DIR "assets/white/{}.png"
#define BLACK_PIECE_DIR "assets/black/{}.png"

int king_moves[8][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
int knight_moves[8][2] = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, -2}, {-1, 2}, {-2, -1}, {-2, 1}};
int bishop_moves[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int rook_moves[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

Game::Game(int width, int height){
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

void Game::create_board(string fen){
    if (fen == "") fen = EMPTY_BOARD;
    board = fen_to_board(fen);
}

bool Game::move_piece(int piece[2], int square[2]){
    bool is_move_valid = false;
    for (int i = 0; i < 27; i++){
        if(possible_moves[piece[0]][piece[1]][i][0] == -1) break;
        if (possible_moves[piece[0]][piece[1]][i][0] == square[0] && possible_moves[piece[0]][piece[1]][i][1] == square[1]){
            is_move_valid = true;
            break;
        }
    }
    if (!is_move_valid) return false;
    board[square[0]][square[1]] = board[piece[0]][piece[1]];
    board[piece[0]][piece[1]] = 0;
    return true;
}

void Game::move_selected(int square[2]){
    move_piece(selected_piece, square);
    selected_piece[0] = -1;
    selected_piece[1] = -1;
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

void Game::update_possible_moves(){
    // spare kings so that they may face threats at last
    int white_king[2] = {-1, -1};
    int black_king[2] = {-1, -1};

    for (int x = 0; x < 8; x++){
        for (int y = 0; y < 8; y++){
            if (board[x][y] == 0) continue;
            if (board[x][y] == WHITE_KING){
                white_king[0] = x;
                white_king[1] = y;
            }  
            else if (board[x][y] == BLACK_KING){
                black_king[0] = x;
                black_king[1] = y;
            }
            else{
                int piece[2] = {x, y};
                get_valid_moves(piece);
            }
        }
    }

    // calculate king moves at last
    get_valid_moves(white_king);
    get_valid_moves(black_king);
}

void Game::get_valid_moves(int piece[2]){
    // TODO: add every valid move to:
    // this->possible_moves
    for (short z = 0; z < 27; z++){
        possible_moves[piece[0]][piece[1]][z][0] = -1;
        possible_moves[piece[0]][piece[1]][z][1] = -1;
    }
    int cursor = 0;
    char piece_char = board[piece[0]][piece[1]];
    // get if black or white
    bool is_white = is_white_piece(piece_char);
    char normal_piece = tolower(piece_char);
    // for pawns
    int starting_y = is_white ? 6 : 1;
    int direction = is_white ? -1 : 1;  
    int attack_moves[2][2] = {{1, direction}, {-1, direction}};  
    switch (normal_piece)
    {
    case BLACK_KING: // only the king and pawn has differs from white to black
        if (is_white){
            
        }
        else {

        }
        break;
    case BLACK_QUEEN:
        cursor = 0;
        for (int i = 0; i < 4; i++){
            int move[2] = {bishop_moves[i][0], bishop_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            while (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7){
                if (board[new_x][new_y] == 0){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                    break;
                }
                else break;
                new_x += move[0];
                new_y += move[1];
            }
        }
        for (int i = 0; i < 4; i++){
            int move[2] = {rook_moves[i][0], rook_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            while (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7){
                if (board[new_x][new_y] == 0){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                    break;
                }
                else break;
                new_x += move[0];
                new_y += move[1];
            }
        }
        break;
    case BLACK_ROOK:
        cursor = 0;
        for (int i = 0; i < 4; i++){
            int move[2] = {rook_moves[i][0], rook_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            while (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7){
                if (board[new_x][new_y] == 0){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                    break;
                }
                else break;
                new_x += move[0];
                new_y += move[1];
            }
        }
        break;
    case BLACK_BISHOP:
        cursor = 0;
        for (int i = 0; i < 4; i++){
            int move[2] = {bishop_moves[i][0], bishop_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            while (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7){
                if (board[new_x][new_y] == 0){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                    break;
                }
                else break;
                new_x += move[0];
                new_y += move[1];
            }
        }
        break;
    case BLACK_KNIGHT:
        cursor = 0;
        for (int i = 0; i < 8; i++){
            int move[2] = {knight_moves[i][0], knight_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) continue;
            
            if (is_white_piece(board[new_x][new_y]) != is_white){
                possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                cursor++;
            }
        }
        break;
    case BLACK_PAWN:
        // by rule, pawns cannot make it out of the board, they would promote
        // so horizontal moves will not be checked if they are out of the board
        cursor = 0;
        if (board[piece[0]][piece[1] + direction] == 0){
            possible_moves[piece[0]][piece[1]][0][0] = piece[0];
            possible_moves[piece[0]][piece[1]][0][1] = piece[1] + direction;
            cursor++;
            if (piece[1] == starting_y && board[piece[0]][piece[1] + 2 * direction] == 0){
                // check if can move 2 squares
                possible_moves[piece[0]][piece[1]][cursor][0] = piece[0];
                possible_moves[piece[0]][piece[1]][cursor][1] = piece[1] + 2 * direction;
                cursor++;
            }
        }
        // check for attacking moves
        for (int i = 0; i < 2; i++){
            int new_x = piece[0] + attack_moves[i][0];
            int new_y = piece[1] + attack_moves[i][1];
            if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) continue;
            if (is_white_piece(board[new_x][new_y]) != is_white && board[new_x][new_y] != 0){
                possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                cursor++;
            }
        }
        break;
    default:
        cout << "Invalid piece: " << piece_char << endl;
        break;
    }
}

void Game::select_piece(int piece[2]){
    selected_piece[0] = piece[0];
    selected_piece[1] = piece[1];
    get_valid_moves(piece);
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
        for (int i = 0; i < 27; i++){
            if (possible_moves[selected_piece[0]][selected_piece[1]][i][0] == -1) break;
            int x = possible_moves[selected_piece[0]][selected_piece[1]][i][0];
            int y = possible_moves[selected_piece[0]][selected_piece[1]][i][1];
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
