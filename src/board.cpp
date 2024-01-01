#include "game.hpp"


int Game::piece_to_int(char piece){
    if (piece == 'K' || piece == 'k') return 0;
    if (piece == 'Q' || piece == 'q') return 1;
    if (piece == 'R' || piece == 'r') return 2;
    if (piece == 'B' || piece == 'b') return 3;
    if (piece == 'N' || piece == 'n') return 4;
    if (piece == 'P' || piece == 'p') return 5;
    return -1;
}


bool Game::is_white_piece(int piece){
    return piece >= 'A' && piece <= 'Z';
}


int** Game::fen_to_board(std::string fen){
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
                board[cursor % 8][cursor / 8] = 0;
                cursor++;
            }
        } else {
            board[cursor % 8][cursor / 8] = c;
            cursor++;
        }
    }
    return board;
}


string Game::board_to_fen(int** board){
    string fen = "";
    for (int y = 0; y < 8; y++){
        int space_count = 0;
        for (int x = 0; x < 8; x++){
            if (board[x][y] == 0){
                space_count++;
            } 
            else {
                if (space_count != 0){
                    fen += to_string(space_count);
                    space_count = 0;
                }
                fen += board[x][y];
            }
        }
        if (space_count != 0){
            fen += to_string(space_count);
        }
        if (y != 7){
            fen += "/";
        }
    }
    return fen;
}
