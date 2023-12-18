#pragma once

#include <iostream>
#include <string>

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
        int possible_moves[8][8][27][2];
        int prev_moves[100][2];

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
        void print_board();
        int mouse_to_square(int mouse_x, int mouse_y);
        void update_possible_moves();
        void get_valid_moves(int piece[2]);
        void select_piece(int piece[2]);
        void handle_left_mouse();
        void handle_events();
        SDL_Texture* get_texture(int piece);
        int piece_to_int(char piece);
        bool is_white_piece(int piece);
        int** fen_to_board(std::string fen);
};
