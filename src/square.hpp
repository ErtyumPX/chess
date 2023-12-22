#pragma once

struct Square {
    int x;
    int y;
    Square(int x, int y){
        this->x = x;
        this->y = y;
    }
    Square(int piece[2]){
        this->x = piece[0];
        this->y = piece[1];
    }

    Square operator+(Square s){
        Square square = Square(this->x + s.x, this->y + s.y);
        return square;
    }

    Square operator-(Square s){
        Square square = Square(this->x - s.x, this->y - s.y);
        return square;
    }

    bool operator==(Square s){
        return (this->x == s.x && this->y == s.y);
    }

    bool operator!=(Square s){
        return (this->x != s.x || this->y != s.y);
    }
};