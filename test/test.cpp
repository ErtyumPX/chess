#include "game.hpp"
#include <cassert>

using namespace std;

#define test_fen_1 "k7/8/8/8/8/8/8/7K w - - 0 1"
#define test_fen_2 "5rnk/6pb/7q/8/8/Q7/BP6/KNR5 w - - 0 1"


void test_fen(){
    Game game;
    game.create_board(test_fen_1);
    assert(game.board[0][0] == BLACK_KING);
    assert(game.board[7][7] == WHITE_KING);
    for (int x = 0; x < 8; x++)
    {
        for (int y = 1; y < 8; y++)
        {
            if (x == 0 && y == 0) continue;
            if (x == 7 && y == 7) continue;
            assert(game.board[x][y] == 0);
        }
    }
    

    game.create_board(test_fen_2);
    assert(game.board[0][7] == WHITE_KING);
    assert(game.board[0][6] == WHITE_BISHOP);
    assert(game.board[0][5] == WHITE_QUEEN);
    assert(game.board[1][7] == WHITE_KNIGHT);
    assert(game.board[1][6] == WHITE_PAWN);
    assert(game.board[2][7] == WHITE_ROOK);
    assert(game.board[7][0] == BLACK_KING);
    assert(game.board[7][1] == BLACK_BISHOP);
    assert(game.board[7][2] == BLACK_QUEEN);
    assert(game.board[6][0] == BLACK_KNIGHT);
    assert(game.board[6][1] == BLACK_PAWN);
    assert(game.board[5][0] == BLACK_ROOK);
    ushort pieces = 0;
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (game.board[x][y] != 0) pieces++;
        }
    }
    assert(pieces == 12);
}


int main(){
    test_fen();
    cout << "------------------------------------" << endl;
    cout << "Serious tests here, ehem... all done" << endl;
    return 0;
}
