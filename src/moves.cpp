#include "game.hpp"

int king_moves[8][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
int knight_moves[8][2] = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, -2}, {-1, 2}, {-2, -1}, {-2, 1}};
int bishop_moves[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int rook_moves[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
// castling squares
int w_left_castle_squares[3][2] = {{1, 7}, {2, 7}, {3, 7}};
int w_right_castle_squares[2][2] = {{5, 7}, {6, 7}};
int b_left_castle_squares[3][2] = {{1, 0}, {2, 0}, {3, 0}};
int b_right_castle_squares[2][2] = {{5, 0}, {6, 0}};


void Game::log_move(int square_1[2], char piece_1, int square_2[2], char piece_2, int index, bool is_en_passant, bool is_promotion, bool is_castle){
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
    prev_moves.push_back(log);
}


bool is_castle_move(int piece[2], int square[2]){
    if (piece[0] == 4 && piece[1] == 0 && square[0] == 0 && square[1] == 0) return true;
    if (piece[0] == 4 && piece[1] == 0 && square[0] == 7 && square[1] == 0) return true;
    if (piece[0] == 4 && piece[1] == 7 && square[0] == 0 && square[1] == 7) return true;
    if (piece[0] == 4 && piece[1] == 7 && square[0] == 7 && square[1] == 7) return true;
    return false;
}


bool Game::move_piece(int piece[2], int square[2]){
    bool is_move_valid = false;
    for (size_t i = 0; i < move_info.p_size(piece); i++){
        if (move_info.take(piece).possible[i][0] == square[0] && move_info.take(piece).possible[i][1] == square[1]){
            is_move_valid = true;
            break;
        }
    }
    if (!is_move_valid) return false;

    if (board[piece[0]][piece[1]] == BLACK_ROOK){ // if it's a black rook
        if (piece[0] == 0 && piece[1] == 0 && b_left_castle) b_left_castle = false;
        else if (piece[0] == 7 && piece[1] == 0 && b_right_castle) b_right_castle = false;
    }

    if (board[piece[0]][piece[1]] == WHITE_ROOK){ // if it's a white rook
        if (piece[0] == 0 && piece[1] == 7 && w_left_castle) w_left_castle = false;
        else if (piece[0] == 7 && piece[1] == 7 && w_right_castle) w_right_castle = false;
    }

    if (board[piece[0]][piece[1]] == BLACK_KING){ // if it's black king, cancel all castles
        b_left_castle = false;
        b_right_castle = false;
    }

    if (board[piece[0]][piece[1]] == WHITE_KING){ // if it's white king, cancel all castles
        w_left_castle = false;
        w_right_castle = false;
    }

    // logging variables
    char piece_1 = board[piece[0]][piece[1]];
    char piece_2 = board[square[0]][square[1]];
    bool is_promotion = false;
    bool is_en_passant = false;
    bool is_castle = false;
    
    int en_passant_cache[2] = {en_passant_square[0], en_passant_square[1]};

    if (tolower(board[piece[0]][piece[1]]) == BLACK_PAWN){ // if it's a pawn
        if (abs(piece[0] - square[0]) == 1 && board[square[0]][square[1]] == 0){ 
            // if it's an en-passant move
            SDL_assert(en_passant_square[0] == square[0]);
            SDL_assert(en_passant_square[1] == piece[1]);
            board[square[0]][piece[1]] = 0;
            is_en_passant = true;
        }
        if(abs(piece[1] - square[1]) == 2){
            // if the pawn made a double move, hold its square
            en_passant_square[0] = square[0];
            en_passant_square[1] = square[1];
        }
        int starting_y = is_white_piece(board[piece[0]][piece[1]]) ? 6 : 1;
        if (abs(starting_y - square[1]) == 6){ // promoting
            board[piece[0]][piece[1]] = 0;
            promote(square);
            is_promotion = true;
        }
        else{ // if not promoting, then continue normally
            board[square[0]][square[1]] = board[piece[0]][piece[1]];
            board[piece[0]][piece[1]] = 0;
        }
    }
    else if(board[piece[0]][piece[1]] == BLACK_KING && is_castle_move(piece, square)){ // if it's black king
        if (square[0] == 0 && square[1] == 0){ // if it's a left castle
            board[2][0] = board[piece[0]][piece[1]];
            board[3][0] = board[0][0];
            board[0][0] = 0;
        }
        else if (square[0] == 7 && square[1] == 0){ // if it's a right castle
            board[6][0] = board[piece[0]][piece[1]];
            board[5][0] = board[7][0];
            board[7][0] = 0;
        }
        board[piece[0]][piece[1]] = 0;
        is_castle = true;
    }
    else if(board[piece[0]][piece[1]] == WHITE_KING && is_castle_move(piece, square)){ // if it's white king
        if (square[0] == 0 && square[1] == 7){ // if it's a left castle
            board[2][7] = board[piece[0]][piece[1]];
            board[3][7] = board[0][7];
            board[0][7] = 0;
        }
        else if (square[0] == 7 && square[1] == 7){ // if it's a right castle
            board[6][7] = board[piece[0]][piece[1]];
            board[5][7] = board[7][7];
            board[7][7] = 0;
        }
        board[piece[0]][piece[1]] = 0;
        is_castle = true;
    }
    else // any other piece
    {
        board[square[0]][square[1]] = board[piece[0]][piece[1]];
        board[piece[0]][piece[1]] = 0; 
    }
    
    if (en_passant_square[0] == en_passant_cache[0] && en_passant_square[1] == en_passant_cache[1]){
        en_passant_square[0] = -1;
        en_passant_square[1] = -1;
    }

    log_move(piece, piece_1, square, piece_2, move_counter, is_en_passant, is_promotion, is_castle);
    is_whites_turn = !is_whites_turn;
    move_counter++;
    update_move_info();
    return true;
}


void Game::move_selected(int square[2]){
    move_piece(selected_piece, square);
    selected_piece[0] = -1;
    selected_piece[1] = -1;
}


void Game::promote(int square[2]){
    board[square[0]][square[1]] = square[1] == 0 ? WHITE_QUEEN  : BLACK_QUEEN;
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


void Game::update_move_info(){
    move_info.clear();
    // spare kings so that they may face threats at last
    int white_king[2] = {-1, -1};
    int black_king[2] = {-1, -1};

    for (int x = 0; x < 8; x++){
        for (int y = 0; y < 8; y++){
            if (board[x][y] == 0){
                move_info.put(x, y, Move());
                continue;
            }
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
    if (white_king[0] != -1) get_valid_moves(white_king);
    if (black_king[0] != -1) get_valid_moves(black_king);
    if (white_king[0] != -1 && black_king[0] != -1) re_check_king_moves(white_king, black_king);

}


void Game::re_check_king_moves(int white_king[2], int black_king[2]){
    // if the kings have any possible moves in common, remove them
    for (size_t i = 0; i < move_info.p_size(white_king); i++){
        for (size_t j = 0; j < move_info.c_size(black_king); j++){
            if (move_info.take(white_king).possible[i][0] == move_info.take(black_king).controlled[j][0] && move_info.take(white_king).possible[i][1] == move_info.take(black_king).controlled[j][1]){
                move_info.take(white_king).possible.erase(move_info.take(white_king).possible.begin() + i);
                i--;
                break;
            }
        }
    }
    for (size_t i = 0; i < move_info.p_size(black_king); i++){
        for (size_t j = 0; j < move_info.c_size(white_king); j++){
            if (move_info.take(black_king).possible[i][0] == move_info.take(white_king).controlled[j][0] && move_info.take(black_king).possible[i][1] == move_info.take(white_king).controlled[j][1]){
                move_info.take(black_king).possible.erase(move_info.take(black_king).possible.begin() + i);
                i--;
                break;
            }
        }
    }
}


void Game::get_valid_moves(int piece[2]){
    char piece_char = board[piece[0]][piece[1]];
    // get if black or white
    bool is_white = is_white_piece(piece_char);
    char normal_piece = tolower(piece_char);
    // for pawns
    int starting_y = is_white ? 6 : 1;
    int direction = is_white ? -1 : 1;  
    int attack_moves[2][2] = {{1, direction}, {-1, direction}};

    Move current_move;
    switch (normal_piece)
    {
    case BLACK_KING: // only the king and pawn moves get affected from white to black // I do not know English
        if (is_white){
            for (int i = 0; i < 8; i++){ // white king movement
                int new_x = piece[0] + king_moves[i][0];
                int new_y = piece[1] + king_moves[i][1];
                if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) continue;
                current_move.c_add(new_x, new_y);
                if (is_white_piece(board[new_x][new_y])) continue;
                bool any_threat = false;
                for (int x = 0; x < 8; x++){
                    if (any_threat) break;
                    for (int y = 0; y < 8; y++){
                        if (any_threat) break;
                        if (board[x][y] == 0) continue;
                        if (is_white_piece(board[x][y])) continue;
                        for (size_t z = 0; z < move_info.c_size(x, y); z++){
                            if (move_info.take(x, y).controlled[z][0] == new_x && move_info.take(x, y).controlled[z][1] == new_y){
                                any_threat = true;
                                break;
                            }
                        }                    
                    }
                }
                if (!any_threat){
                    current_move.p_add(new_x, new_y);
                }
            }
            if (piece[0] == 4 && piece[1] == 7){ // white castle availability check
                if (w_left_castle){ // left side castle
                    bool any_obstacle = false;
                    for (size_t i = 0; i < 3; i++){
                        if (board[w_left_castle_squares[i][0]][w_left_castle_squares[i][1]] != 0){
                            any_obstacle = true;
                            break;
                        }
                    }
                    for (int x = 0; x < 8; x++){
                        if (any_obstacle) break;
                        for (int y = 0; y < 8; y++){
                            if (any_obstacle) break;
                            if (board[x][y] == 0) continue;
                            if (is_white_piece(board[x][y])) continue;
                            if (move_info.take(x, y).c_check(w_left_castle_squares[0]) || 
                                move_info.take(x, y).c_check(w_left_castle_squares[1]) || 
                                move_info.take(x, y).c_check(w_left_castle_squares[2]) ||
                                move_info.take(x, y).c_check(4, 7))
                            {
                                any_obstacle = true;
                                break;
                            }
                        }
                    }
                    if (!any_obstacle){
                        current_move.p_add(0, 7);
                    }
                }
                if (w_right_castle){ // right side castle
                    bool any_obstacle = false;
                    for (size_t i = 0; i < 2; i++){
                        if (board[w_right_castle_squares[i][0]][w_right_castle_squares[i][1]] != 0){
                            any_obstacle = true;
                            break;
                        }
                    }
                    for (int x = 0; x < 8; x++){
                        if (any_obstacle) break;
                        for (int y = 0; y < 8; y++){
                            if (any_obstacle) break;
                            if (board[x][y] == 0) continue;
                            if (is_white_piece(board[x][y])) continue;
                            if (move_info.take(x, y).c_check(w_right_castle_squares[0]) || 
                                move_info.take(x, y).c_check(w_right_castle_squares[1]) || 
                                move_info.take(x, y).c_check(4, 7))
                            {
                                any_obstacle = true;
                                break;
                            }
                        }
                    }
                    if (!any_obstacle){
                        current_move.p_add(7, 7);
                    }
                }
            }
        }
        
        else { // if it's black king
            for (int i = 0; i < 8; i++){ // black king movement
                int new_x = piece[0] + king_moves[i][0];
                int new_y = piece[1] + king_moves[i][1];
                if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) continue;
                current_move.c_add(new_x, new_y);
                if (is_black_piece(board[new_x][new_y])) continue;
                bool any_threat = false;
                for (int x = 0; x < 8; x++){
                    if (any_threat) break;
                    for (int y = 0; y < 8; y++){
                        if (any_threat) break;
                        if (board[x][y] == 0) continue;
                        if (is_black_piece(board[x][y])) continue;
                        for (size_t z = 0; z < move_info.c_size(x, y); z++){
                            if (move_info.take(x, y).controlled[z][0] == new_x && move_info.take(x, y).controlled[z][1] == new_y){
                                any_threat = true;
                                break;
                            }
                        }                    
                    }
                }
                if (!any_threat){
                    current_move.p_add(new_x, new_y);
                }
            }
            if (piece[0] == 4 && piece[1] == 0){ // black castle availability check
                if (b_left_castle){ // left side castle
                    bool any_obstacle = false;
                    for (size_t i = 0; i < 3; i++){
                        if (board[b_left_castle_squares[i][0]][b_left_castle_squares[i][1]] != 0){
                            any_obstacle = true;
                            break;
                        }
                    }
                    for (int x = 0; x < 8; x++){
                        if (any_obstacle) break;
                        for (int y = 0; y < 8; y++){
                            if (any_obstacle) break;
                            if (board[x][y] == 0) continue;
                            if (is_black_piece(board[x][y])) continue;
                            if (move_info.take(x, y).c_check(b_left_castle_squares[0]) || 
                                move_info.take(x, y).c_check(b_left_castle_squares[1]) || 
                                move_info.take(x, y).c_check(b_left_castle_squares[2]) ||
                                move_info.take(x, y).c_check(4, 0))
                            {
                                any_obstacle = true;
                                break;
                            }
                        }
                    }
                    if (!any_obstacle){
                        current_move.p_add(0, 0);
                    }
                }
                if (b_right_castle){ // right side castle
                    bool any_obstacle = false;
                    for (size_t i = 0; i < 2; i++){
                        if (board[b_right_castle_squares[i][0]][b_right_castle_squares[i][1]] != 0){
                            any_obstacle = true;
                            break;
                        }
                    }
                    for (int x = 0; x < 8; x++){
                        if (any_obstacle) break;
                        for (int y = 0; y < 8; y++){
                            if (any_obstacle) break;
                            if (board[x][y] == 0) continue;
                            if (is_black_piece(board[x][y])) continue;
                            if (move_info.take(x, y).c_check(b_right_castle_squares[0]) || 
                                move_info.take(x, y).c_check(b_right_castle_squares[1]) || 
                                move_info.take(x, y).c_check(4, 0))
                            {
                                any_obstacle = true;
                                break;
                            }
                        }
                    }
                    if (!any_obstacle){
                        current_move.p_add(7, 0);
                    }
                }
            }
        }
        break;
    case BLACK_QUEEN:
        for (int i = 0; i < 4; i++){
            int move[2] = {bishop_moves[i][0], bishop_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            while (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7){
                current_move.c_add(new_x, new_y);
                if (board[new_x][new_y] == 0){
                    current_move.p_add(new_x, new_y);
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    current_move.p_add(new_x, new_y);
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
                current_move.c_add(new_x, new_y);
                if (board[new_x][new_y] == 0){
                    current_move.p_add(new_x, new_y);
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    current_move.p_add(new_x, new_y);
                    break;
                }
                else break;
                new_x += move[0];
                new_y += move[1];
            }
        }
        break;
    case BLACK_ROOK:
        for (int i = 0; i < 4; i++){
            int move[2] = {rook_moves[i][0], rook_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            while (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7){
                current_move.c_add(new_x, new_y);
                if (board[new_x][new_y] == 0){
                    current_move.p_add(new_x, new_y);
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    current_move.p_add(new_x, new_y);
                    break;
                }
                else break;
                new_x += move[0];
                new_y += move[1];
            }
        }
        break;
    case BLACK_BISHOP:
        for (int i = 0; i < 4; i++){
            int move[2] = {bishop_moves[i][0], bishop_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            while (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7){
                current_move.c_add(new_x, new_y);
                if (board[new_x][new_y] == 0){
                    current_move.p_add(new_x, new_y);
                }
                else if (is_white_piece(board[new_x][new_y]) != is_white){
                    current_move.p_add(new_x, new_y);
                    break;
                }
                else break;
                new_x += move[0];
                new_y += move[1];
            }
        }
        break;
    case BLACK_KNIGHT:
        for (int i = 0; i < 8; i++){
            int move[2] = {knight_moves[i][0], knight_moves[i][1]};
            int new_x = piece[0] + move[0];
            int new_y = piece[1] + move[1];
            if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) continue;
            current_move.c_add(new_x, new_y);
            if (board[new_x][new_y] == 0 || is_white_piece(board[new_x][new_y]) != is_white){
                current_move.p_add(new_x, new_y);
            }
        }
        break;
    case BLACK_PAWN:
        // by rule, pawns cannot make it out of the board, they would promote
        // so horizontal moves will not be checked if they are out of the board
        if (board[piece[0]][piece[1] + direction] == 0){
            current_move.p_add(piece[0], piece[1] + direction);
            if (piece[1] == starting_y && board[piece[0]][piece[1] + 2 * direction] == 0){
                // check if can move 2 squares
                current_move.p_add(piece[0], piece[1] + direction * 2);
            }
        }
        // check for attacking moves
        for (int i = 0; i < 2; i++){
            int new_x = piece[0] + attack_moves[i][0];
            int new_y = piece[1] + attack_moves[i][1];
            if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) continue;
            current_move.c_add(new_x, new_y);
            if (is_white_piece(board[new_x][new_y]) != is_white && board[new_x][new_y] != 0){
                current_move.p_add(new_x, new_y);
            }
            if (en_passant_square[0] != -1 && en_passant_square[1] == piece[1]){
                if (abs(en_passant_square[1] - new_y) == 1 && en_passant_square[0] == new_x){
                    current_move.p_add(new_x, new_y);
                }
            }
        }
        break;
    default:
        cout << "Invalid piece: " << piece_char << endl;
        break;
    }
    move_info.put(piece, current_move);
}
