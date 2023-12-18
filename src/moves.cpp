#include "game.hpp"

int king_moves[8][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
int knight_moves[8][2] = {{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, -2}, {-1, 2}, {-2, -1}, {-2, 1}};
int bishop_moves[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
int rook_moves[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

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
    is_whites_turn = !is_whites_turn;
    update_possible_moves();
    return true;
}

void Game::move_selected(int square[2]){
    move_piece(selected_piece, square);
    selected_piece[0] = -1;
    selected_piece[1] = -1;
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

    cout << white_king[0] << " " << white_king[1] << endl;
    cout << black_king[0] << " " << black_king[1] << endl;
    cout << "Hihi" << endl;
    // calculate king moves at last
    get_valid_moves(white_king);
    get_valid_moves(black_king);
}

void Game::get_valid_moves(int piece[2]){
    for (int z = 0; z < 27; z++){
        possible_moves[piece[0]][piece[1]][z][0] = -1;
        possible_moves[piece[0]][piece[1]][z][1] = -1;
    }
    int cursor;
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
            for (int i = 0; i < 8; i++){
                int new_x = piece[0] + king_moves[i][0];
                int new_y = piece[1] + king_moves[i][1];
                if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) continue;
                if (is_white_piece(board[new_x][new_y])) continue;
                bool any_threat = false;
                for (int x = 0; x < 8; x++){
                    if (any_threat) break;
                    for (int y = 0; y < 8; y++){
                        if (any_threat) break;
                        if (board[x][y] == 0) continue;
                        if (is_white_piece(board[x][y])) continue;
                        for (int z = 0; z < 27; z++){
                            if (possible_moves[x][y][z][0] == -1) break;
                            if (possible_moves[x][y][z][0] == new_x && possible_moves[x][y][z][1] == new_y){
                                any_threat = true;
                                break;
                            }
                        }                    
                    }
                }
                if (!any_threat){
                    possible_moves[piece[0]][piece[1]][cursor][0] = new_x;
                    possible_moves[piece[0]][piece[1]][cursor][1] = new_y;
                    cursor++;
                }
                
            }
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
