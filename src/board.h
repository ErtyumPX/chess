#pragma once

#include <string>

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

bool is_white_piece(int piece){
    return piece >= 'A' && piece <= 'Z';
}

int** fen_to_board(std::string fen){
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

