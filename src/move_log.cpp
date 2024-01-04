#include "game.hpp"


void Game::log_move(int square_1[2], char piece_1, int square_2[2], char piece_2, int index, bool is_en_passant, bool is_promotion, bool is_castle, int en_passant_square[2]){
    // if the move is not the last move, delete all moves after it
    // (happens when a move is taken back and a new move is made)
    if (index < (int)prev_moves.size()){
        prev_moves.erase(prev_moves.begin() + index, prev_moves.end());
    }
    MoveLog log;
    log.x1 = square_1[0];
    log.y1 = square_1[1];
    log.piece1 = piece_1;
    log.x2 = square_2[0];
    log.y2 = square_2[1];
    log.piece2 = piece_2;
    log.index = index;
    log.w_left_castle = w_left_castle;
    log.w_right_castle = w_right_castle;
    log.b_left_castle = b_left_castle;
    log.b_right_castle = b_right_castle;
    log.is_en_passant = is_en_passant;
    log.is_promotion = is_promotion;
    log.is_castle = is_castle;
    log.en_passant_x = en_passant_square[0];
    log.en_passant_y = en_passant_square[1];
    prev_moves.push_back(log);
}


void Game::take_back(short times){
    for (int i = 0; i < times; i++){
        if (move_counter == 0) break;
        move_counter--;
        MoveLog log = prev_moves[move_counter];
        if (log.is_en_passant){
            board[log.x1][log.y1] = log.piece1;
            board[log.x2][log.y2] = 0;
            // put opposite side pawn back
            board[log.x2][log.y1] = is_white_piece(log.piece1) ? BLACK_PAWN : WHITE_PAWN;
            en_passant_square[0] = log.x2;
            en_passant_square[1] = log.y2;
        }
        else if (log.is_castle){
            if (log.x2 == 0 && log.y2 == 0){ // if it's a left castle
                board[0][0] = BLACK_ROOK;
                board[2][0] = 0;
                board[3][0] = 0;
                board[4][0] = BLACK_KING;
            }
            else if (log.x2 == 7 && log.y2 == 0){ // if it's a right castle
                board[7][0] = BLACK_ROOK;
                board[6][0] = 0;
                board[5][0] = 0;
                board[4][0] = BLACK_KING;
            }
            else if (log.x2 == 0 && log.y2 == 7){ // if it's a left castle
                board[0][7] = WHITE_ROOK;
                board[2][7] = 0;
                board[3][7] = 0;
                board[4][7] = WHITE_KING;
            }
            else if (log.x2 == 7 && log.y2 == 7){ // if it's a right castle
                board[7][7] = WHITE_ROOK;
                board[6][7] = 0;
                board[5][7] = 0;
                board[4][7] = WHITE_KING;
            }
        }
        else{
            board[log.x1][log.y1] = log.piece1;
            board[log.x2][log.y2] = log.piece2;
        }
 
        is_whites_turn = !is_whites_turn;
        MoveLog prev_log = prev_moves[move_counter - 1];
        w_left_castle = prev_log.w_left_castle;
        w_right_castle = prev_log.w_right_castle;
        b_left_castle = prev_log.b_left_castle;
        b_right_castle = prev_log.b_right_castle;
            en_passant_square[0] = prev_log.en_passant_x;
        en_passant_square[1] = prev_log.en_passant_y;
    }
    update_move_info();
}


void Game::go_forward(short times){
    for (int i = 0; i < times; i++){
        if (move_counter >= (int)prev_moves.size()) break;
        move_counter++;
        MoveLog log = prev_moves[move_counter - 1];
        if (log.is_en_passant){
            board[log.x2][log.y2] = log.piece1;
            board[log.x1][log.y1] = 0;
            // remove opposite side pawn
            board[log.x2][log.y1] = 0;
            en_passant_square[0] = -1;
            en_passant_square[1] = -1;
        }
        else if (log.is_castle){
            if (log.x2 == 0 && log.y2 == 0){ // if it's a left castle
                board[0][0] = 0;
                board[2][0] = BLACK_KING;
                board[3][0] = BLACK_ROOK;
                board[4][0] = 0;
            }
            else if (log.x2 == 7 && log.y2 == 0){ // if it's a right castle
                board[7][0] = 0;
                board[6][0] = BLACK_KING;
                board[5][0] = BLACK_ROOK;
                board[4][0] = 0;
            }
            else if (log.x2 == 0 && log.y2 == 7){ // if it's a left castle
                board[0][7] = 0;
                board[2][7] = WHITE_KING;
                board[3][7] = WHITE_ROOK;
                board[4][7] = 0;
            }
            else if (log.x2 == 7 && log.y2 == 7){ // if it's a right castle
                board[7][7] = 0;
                board[6][7] = WHITE_KING;
                board[5][7] = WHITE_ROOK;
                board[4][7] = 0;
            }

        }
        else if(log.is_promotion){
            board[log.x2][log.y2] = log.piece1;
            board[log.x1][log.y1] = 0;
            int piece[2] = {log.x2, log.y2};
            promote(piece);
        }
        else{
            board[log.x2][log.y2] = log.piece1;
            board[log.x1][log.y1] = 0;
        }

        is_whites_turn = !is_whites_turn;
    }
    update_move_info();
}
