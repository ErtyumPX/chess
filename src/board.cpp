#include "game.hpp"
#include <assert.h>
#include <regex>

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
    return 'A' <= piece && piece <= 'Z';
}


bool Game::is_black_piece(int piece){
    return 'a' <= piece && piece <= 'z';
}


// pieces in board, turn, castling, en passant, halfmove clock, fullmove number
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
// rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
// rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2
// rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2

// regex for fen

// ([rnbqkbnr]{8}\/){7}[rnbqkbnr]{8} [wb] (K?Q?k?q?|-) ([a-h][1-8]|-) \d+ \d+


FENRecord Game::fen_to_board(std::string fen){
    // board is 2d 8x8 int array
    int index = 0;

    FENRecord record;
    record.notation = fen;
    
    // board
    int** board = new int*[8];
    for (int i = 0; i < 8; i++){
        board[i] = new int[8];
    }
    int cursor = 0;
    for (char c : fen){
        index++;
        if (c == ' '){
            break;
        }
        if (c == '/'){
            continue;
        }
        if (c >= '0' && c <= '9'){
            int space_count = c - '0';
            for (int i = 0; i < space_count; i++){
                board[cursor % 8][cursor / 8] = 0;
                cursor++;
            }
        } 
        else {
            board[cursor % 8][cursor / 8] = c;
            cursor++;
        }
    }
    record.board = board;
    record.turn = fen[index] == 'w';
    index += 2;

    // castling
    record.white_castle_k = false;
    record.white_castle_q = false;
    record.black_castle_k = false;
    record.black_castle_q = false;
    if (fen[index] == '-'){
        index += 2;
    } 
    else {
        while (fen[index] != ' '){
            assert(fen[index] == 'K' || fen[index] == 'Q' || fen[index] == 'k' || fen[index] == 'q');
            if (fen[index] == 'K') record.white_castle_k = true;
            if (fen[index] == 'Q') record.white_castle_q = true;
            if (fen[index] == 'k') record.black_castle_k = true;
            if (fen[index] == 'q') record.black_castle_q = true;
            index++;
        }
        index++;
    }

    // en-passant
    if (fen[index] == '-'){
        record.en_passant_x = -1;
        record.en_passant_y = -1;
        index += 2;
    } 
    else {
        // first char is file, second char is rank
        assert(fen[index] >= 'a' && fen[index] <= 'h');
        assert(fen[index + 1] >= '1' && fen[index + 1] <= '8');
        record.en_passant_x = fen[index] - 'a';
        record.en_passant_y = fen[index + 1] - '1';
        index += 3;
    }

    int halfclock_int = fen[index] - '0';
    index += 2;
    int fullmove_int = fen[index] - '0';
    record.halfmove_clock = halfclock_int;
    record.fullmove_number = fullmove_int;

    assert(halfclock_int >= 0 && halfclock_int <= 99);
    assert(fullmove_int >= 1);
    assert(index + 1 == (int)fen.length());

    return record;
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
