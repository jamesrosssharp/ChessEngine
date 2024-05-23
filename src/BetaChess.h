/* vim: set et ts=4 sw=4: */

/*
	$PROJECT

$FILE: $DESC

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

enum BitboardIdx
{
    BITBOARD_INVALID1 = 0;
    BITBOARD_INVALID2 = 1;
    BITBOARD_WHITE_PIECES = 2;
    BITBOARD_BLACK_PIECES = 3;
    BITBOARD_PAWN   = 4;
    BITBOARD_KNIGHT = 5;
    BITBOARD_BISHOP = 6;
    BITBOARD_ROOK   = 7;
    BITBOARD_QUEEN  = 8;
    BITBOARD_KING   = 9;
};

struct BetaMove {
    uint32_t sq_from : 6;
    uint32_t sq_to   : 6;
    uint32_t bitboard_color_from      : 2;
    uint32_t bitboard_color_capture   : 2;
    uint32_t from_piece               : 4;
    uint32_t capture_piece            : 4;
};

struct BetaBoard
{

    uint64_t bitboards[10];

    BetaBoard()
    {
        bitboards[BITBOARD_WHITE_PIECES] = 0x0000'0000'0000'ffff;
        bitboards[BITBOARD_BLACK_PIECES] = 0xffff'0000'0000'0000;
        bitboards[BITBOARD_PAWN]         = 0x00ff'0000'0000'ff00;
        bitboards[BITBOARD_KNIGHT]       = 0x4200'0000'0000'0042;
        bitboards[BITBOARD_BISHOP]       = 0x2400'0000'0000'0024;
        bitboards[BITBOARD_ROOK]         = 0x8100'0000'0000'0081;
        bitboards[BITBOARD_QUEEN]        = 0x1000'0000'0000'0010;
        bitboards[BITBOARD_KING]         = 0x0800'0000'0000'0008;
    } 

    uint64_t* whitePieces() { return &bitboards[BITBOARD_WHITE_PIECES]; }
    uint64_t* blackPieces() { return &bitboards[BITBOARD_BLACK_PIECES]; }
    uint64_t* pawns()       { return &bitboards[BITBOARD_PAWN];         }
    uint64_t* knights()     { return &bitboards[BITBOARD_KNIGHT];       }
    uint64_t* bishops()     { return &bitboards[BITBOARD_BISHOP];       }
    uint64_t* rooks()       { return &bitboards[BITBOARD_ROOK];         }
    uint64_t* kings()       { return &bitboards[BITBOARD_KING];         }
    uint64_t* queens()      { return &bitboards[BITBOARD_QUEEN];        }

    uint64_t  whitePawns()      { return *pawns()   & *whitePieces(); }
    uint64_t  whiteKnights()    { return *knights() & *whitePieces(); }
    uint64_t  whiteBishops()    { return *bishops() & *whitePieces(); }
    uint64_t  whiteRooks()      { return *rooks()   & *whitePieces(); }
    uint64_t  whiteKings()      { return *kings()   & *whitePieces(); }
    uint64_t  whiteQueens()     { return *queens()  & *whitePieces(); }

    uint64_t  blackPawns()      { return *pawns()   & *blackPieces(); }
    uint64_t  blackKnights()    { return *knights() & *blackPieces(); }
    uint64_t  blackBishops()    { return *bishops() & *blackPieces(); }
    uint64_t  blackRooks()      { return *rooks()   & *blackPieces(); }
    uint64_t  blackKings()      { return *kings()   & *blackPieces(); }
    uint64_t  blackQueens()     { return *queens()  & *blackPieces(); }

    void _makeMove(const struct BetaMove& move)
    {

        uint64_t bbFrom = 1ULL << move.sq_from;
        uint64_t bbTo   = 1ULL << move.sq_to;
        uint64_t bbFromTo = bbFrom ^ bbTo;
 
        bitboards[move.bitboard_color_from]      ^= bbFromTo;
        bitboards[move.bitboard_color_capture]   ^= bbTo;
  
        bitboards[move.from_piece]               ^= bbFromTo;
        bitboards[move.capture_piece]            ^= bbTo;


    }

    void makeMove(const struct BetaMove& move)
    {
        _makeMove(move);
        // TODO: Push other meta data, such as can castle, can e.p etc onto stack

    }

    void unMakeMove(const struct BetaMove& move)
    {

        // TODO: Pop other meta data, such as can castle, can e.p etc off stack
        _makeMove(move);

    }



};

