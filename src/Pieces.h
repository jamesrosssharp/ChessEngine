/* vim: set et ts=4 sw=4: */

/*
    ChessEngine : A chess engine written in C++ for SDL2

Pieces.h: Some piece types enums

License: MIT License

Copyright 2023 J.R.Sharp

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

enum SimplePieceTypes {
    PIECE_PAWN = 0,
    PIECE_KNIGHT = 1,
    PIECE_BISHOP = 2,
    PIECE_ROOK = 3,
    PIECE_QUEEN = 4,
    PIECE_KING = 5
};

enum Files {
    A_FILE = 0,
    B_FILE = 1,
    C_FILE = 2,
    D_FILE = 3,
    E_FILE = 4,
    F_FILE = 5,
    G_FILE = 6,
    H_FILE = 7,
    INVALID_FILE = -1
};

enum Ranks {
    FIRST_RANK   = 0,
    SECOND_RANK  = 1,
    THIRD_RANK   = 2,
    FOURTH_RANK  = 3,
    FIFTH_RANK   = 4,
    SIXTH_RANK   = 5,
    SEVENTH_RANK = 6,
    EIGHTH_RANK   = 7,
    INVALID_RANK = -1
};

#define COORD_TO_BIT(file, rank) (1ULL << ((file) + (rank)*8))
#define IS_IN_BOARD(file, rank) ((file >= A_FILE) && (file <= H_FILE) && (rank >= FIRST_RANK) && (rank <= EIGHTH_RANK))

