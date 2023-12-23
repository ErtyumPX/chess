#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

using namespace std;


enum Piece {
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


struct MoveLog {
    ushort index;
    ushort x1;
    ushort y1;
    ushort x2;
    ushort y2;
    char piece1;
    char piece2;
};


class Move {
    public:
        vector<vector<int>> possible;
        vector<vector<int>> controlled;

        void p_add(int x, int y){
            possible.push_back({x, y});
        }

        void p_add(int square[2]){
            possible.push_back({square[0], square[1]});
        }

        void c_add(int x, int y){
            controlled.push_back({x, y});
        }

        void c_add(int square[2]){
            controlled.push_back({square[0], square[1]});
        }
};


class MoveInfo{
    public:
        Move all_moves[8][8];

        MoveInfo(){
            for (int x = 0; x < 8; x++){
                for (int y = 0; y < 8; y++){
                    all_moves[x][y] = Move();
                }
            }
        }

        void clear(){
            for (int x = 0; x < 8; x++){
                for (int y = 0; y < 8; y++){
                    all_moves[x][y] = Move();
                }
            }
        }

        Move take(int x, int y){
            return all_moves[x][y];
        }

        Move take(int piece[2]){
            return all_moves[piece[0]][piece[1]];
        }

        void put(int x, int y, Move move){
            all_moves[x][y] = move;
        }

        void put(int piece[2], Move move){
            all_moves[piece[0]][piece[1]] = move;
        }

        int p_size(int x, int y){
            return all_moves[x][y].possible.size();
        }

        int p_size(int piece[2]){
            return all_moves[piece[0]][piece[1]].possible.size();
        }

        int c_size(int x, int y){
            return all_moves[x][y].controlled.size();
        }

        int c_size(int piece[2]){
            return all_moves[piece[0]][piece[1]].controlled.size();
        }
};


class Game{
    private: // private variables
        // window Rendering
        int width;
        int height;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        // event handling
        int current_square = -1;

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
        bool is_dragging = false;
        MoveInfo move_info;
        vector<MoveLog> prev_moves; // array of [x1, y1, x2, y2]
        int move_counter = 0; // move_counter % 2 == 0 -> white's turn
        bool is_whites_turn;

        // game Loop
        bool quit = false;
        bool restart = false;

    public:
        Game(int width, int height);
        ~Game();
        void create_board(string fen = "");
        void render();
        void loop();

    private:
        bool move_piece(int piece[2], int square[2]);
        void move_selected(int square[2]);
        void take_back(short times = 1);
        void go_forward(short times = 1);
        void log_move(int piece[2], int square[2], int index);
        void print_board();
        int mouse_to_square(int mouse_x, int mouse_y);
        void update_move_info();
        void get_valid_moves(int piece[2]);
        void select_piece(int piece[2]);
        void handle_left_mouse();
        void handle_events();
        SDL_Texture* get_texture(int piece);
        int piece_to_int(char piece);
        bool is_white_piece(int piece);
        int** fen_to_board(std::string fen);
};
