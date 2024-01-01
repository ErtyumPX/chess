#include "game.hpp"

int king_moves[8][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
int knight_moves[8][2] = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, -2}, {-1, 2}, {-2, -1}, {-2, 1}};
int bishop_moves[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int rook_moves[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};


void Game::log_move(int piece[2], int square[2], int index){
    if (index < (int)prev_moves.size()){
        prev_moves.erase(prev_moves.begin() + index, prev_moves.end());
    }
    MoveLog log;
    log.x1 = piece[0];
    log.y1 = piece[1];
    log.piece1 = board[piece[0]][piece[1]];
    log.x2 = square[0];
    log.y2 = square[1];
    log.piece2 = board[square[0]][square[1]];
    log.index = index;
    prev_moves.push_back(log);
}


bool Game::move_piece(int piece[2], int square[2]){
    bool is_move_valid = false;
    for (int i = 0; i < move_info.p_size(piece); i++){
        if (move_info.take(piece).possible[i][0] == square[0] && move_info.take(piece).possible[i][1] == square[1]){
            is_move_valid = true;
            break;
        }
    }
    if (!is_move_valid) return false;
    log_move(piece, square, move_counter); // it is important to log before applying the change

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

    if (tolower(board[piece[0]][piece[1]]) == BLACK_PAWN){ // if it's a pawn
        if (abs(piece[0] - square[0]) == 1 && board[square[0]][square[1]] == 0){ 
            // if it's an en-passant move
            SDL_assert(en_passant_square[0] == square[0]);
            SDL_assert(en_passant_square[1] == piece[1]);
            board[square[0]][piece[1]] = 0;
        }
        if(abs(piece[1] - square[1]) == 2){
            // if the pawn made a double move, hold its square
            en_passant_square[0] = square[0];
            en_passant_square[1] = square[1];
        }
        else{
            // if not holding any en-passant square for the next turn, just remove it
            en_passant_square[0] = -1;
            en_passant_square[1] = -1;   
        }
        int starting_y = is_white_piece(board[piece[0]][piece[1]]) ? 6 : 1;
        if (abs(starting_y - square[1]) == 6){ // promoting
            board[piece[0]][piece[1]] = 0;
            promote(square);
        }
        else{ // if not promoting, then continue normally
            board[square[0]][square[1]] = board[piece[0]][piece[1]];
            board[piece[0]][piece[1]] = 0;
        }
    }
    else
    {
        board[square[0]][square[1]] = board[piece[0]][piece[1]];
        board[piece[0]][piece[1]] = 0;
        en_passant_square[0] = -1;
        en_passant_square[1] = -1;   
    }
    
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
        board[log.x1][log.y1] = log.piece1;
        board[log.x2][log.y2] = log.piece2;
        is_whites_turn = !is_whites_turn;
    }
    update_move_info();
}


void Game::go_forward(short times){
    for (int i = 0; i < times; i++){
        if (move_counter >= (int)prev_moves.size()) break;
        move_counter++;
        MoveLog log = prev_moves[move_counter - 1];
        board[log.x1][log.y1] = 0;
        board[log.x2][log.y2] = log.piece1;
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
    for (int i = 0; i < move_info.p_size(white_king); i++){
        for (int j = 0; j < move_info.c_size(black_king); j++){
            if (move_info.take(white_king).possible[i][0] == move_info.take(black_king).controlled[j][0] && move_info.take(white_king).possible[i][1] == move_info.take(black_king).controlled[j][1]){
                move_info.take(white_king).possible.erase(move_info.take(white_king).possible.begin() + i);
                i--;
                break;
            }
        }
    }
    for (int i = 0; i < move_info.p_size(black_king); i++){
        for (int j = 0; j < move_info.c_size(white_king); j++){
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
            for (int i = 0; i < 8; i++){
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
                        for (int z = 0; z < move_info.c_size(x, y); z++){
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
        }
        else { // if it's black king

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
