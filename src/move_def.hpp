#pragma once

#include <vector>

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
    unsigned short index;
    unsigned short x1;
    unsigned short y1;
    unsigned short x2;
    unsigned short y2;
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

        bool p_check(int x, int y){
            for (size_t i = 0; i < possible.size(); i++){
                if (possible[i][0] == x && possible[i][1] == y) return true;
            }
            return false;
        }

        bool p_check(int piece[2]){
            for (size_t i = 0; i < possible.size(); i++){
                if (possible[i][0] == piece[0] && possible[i][1] == piece[1]) return true;
            }
            return false;
        }

        void c_add(int x, int y){
            controlled.push_back({x, y});
        }

        void c_add(int square[2]){
            controlled.push_back({square[0], square[1]});
        }

        bool c_check(int x, int y){
            for (size_t i = 0; i < controlled.size(); i++){
                if (controlled[i][0] == x && controlled[i][1] == y) return true;
            }
            return false;
        }

        bool c_check(int piece[2]){
            for (size_t i = 0; i < controlled.size(); i++){
                if (controlled[i][0] == piece[0] && controlled[i][1] == piece[1]) return true;
            }
            return false;
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

        size_t p_size(int x, int y){
            return all_moves[x][y].possible.size();
        }

        size_t p_size(int piece[2]){
            return all_moves[piece[0]][piece[1]].possible.size();
        }

        size_t c_size(int x, int y){
            return all_moves[x][y].controlled.size();
        }

        size_t c_size(int piece[2]){
            return all_moves[piece[0]][piece[1]].controlled.size();
        }
};
