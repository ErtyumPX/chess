#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

#include "move_def.hpp"

using namespace std;


class Tester; // unit testing class - friend of Game

class Game{
    private:
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

        // en passant
        int en_passant_square[2] = {-1, -1};

        // castling
        bool w_left_castle = true;
        bool w_right_castle = true;
        bool b_left_castle = true;
        bool b_right_castle = true;

        // game Loop
        bool quit = false;
        bool restart = false;

    public:
        Game();
        ~Game();
        void initialize(int width, int height);
        void create_board(string fen = "");
        void render();
        void loop();
    
    private:
        bool move_piece(int piece[2], int square[2]);
        void move_selected(int square[2]);
        void promote(int sqaure[2]);
        void take_back(short times = 1);
        void go_forward(short times = 1);
        void log_move(int square_1[2], char piece_1, int square_2[2], char piece_2, int index, bool is_en_passant, bool is_promotion, bool is_castle, int en_passant_square[2]);
        void print_board();
        int mouse_to_square(int mouse_x, int mouse_y);
        void update_move_info();
        void re_check_king_moves(int white_king[2], int black_king[2]);
        void get_valid_moves(int piece[2]);
        void select_piece(int piece[2]);
        void handle_left_mouse();
        void handle_events();
        SDL_Texture* get_texture(int piece);
        int piece_to_int(char piece);
        bool is_white_piece(int piece);
        bool is_black_piece(int piece);
        int** fen_to_board(std::string fen);
        string board_to_fen(int** board);
        
    friend class Tester;
};
