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

#include <stdint.h>
#include <Blockers.h>
#include <MagicBitboards.h>

enum BitboardPieceIdx
{
    BITBOARD_INVALID1 = 0,
    BITBOARD_INVALID2 = 1,
    BITBOARD_PAWN   = 2,
    BITBOARD_KNIGHT = 3,
    BITBOARD_BISHOP = 4,
    BITBOARD_ROOK   = 5,
    BITBOARD_QUEEN  = 6,
    BITBOARD_KING   = 7
};

enum BitboardColorIdx
{
    BITBOARD_WHITE_PIECES = 0,
    BITBOARD_BLACK_PIECES = 1
};

enum BetaMoveFlags {
    IS_QUIET           = 0,
    IS_CAPTURE         = 1,
    IS_PROMOTE         = 2,
    IS_PROMOTE_CAPTURE = 3,
    IS_CASTLE          = 4,
    IS_EN_PASSENT      = 5
};

struct BetaMove {
    uint32_t sq_from                  : 6;  // 6
    uint32_t sq_to                    : 6;  // 12
    uint32_t bitboard_color_from      : 2;  // 14
    uint32_t bitboard_color_capture   : 2;  // 16
    uint32_t bitboard_color_to        : 2;  // 18
    uint32_t from_piece               : 3;  // 21
    uint32_t capture_piece            : 3;  // 24
    uint32_t to_piece                 : 3;  // 27                                        
    uint32_t flags                    : 5;  // 32
};

enum BoardTurn 
{
    TURN_WHITE = 0,
    TURN_BLACK = 1
};

struct BetaBoard
{

    uint64_t bitboards_color[2];
    uint64_t bitboards_piece[8];

    bool     turn;


    BetaBoard()
    {
        bitboards_color[BITBOARD_WHITE_PIECES] = 0x0000'0000'0000'ffff;
        bitboards_color[BITBOARD_BLACK_PIECES] = 0xffff'0000'0000'0000;
        bitboards_piece[BITBOARD_PAWN]         = 0x00ff'0000'0000'ff00;
        bitboards_piece[BITBOARD_KNIGHT]       = 0x4200'0000'0000'0042;
        bitboards_piece[BITBOARD_BISHOP]       = 0x2400'0000'0000'0024;
        bitboards_piece[BITBOARD_ROOK]         = 0x8100'0000'0000'0081;
        bitboards_piece[BITBOARD_QUEEN]        = 0x1000'0000'0000'0010;
        bitboards_piece[BITBOARD_KING]         = 0x0800'0000'0000'0008;
        turn = TURN_WHITE;
    } 

    uint64_t* whitePieces() { return &bitboards_color[BITBOARD_WHITE_PIECES]; }
    uint64_t* blackPieces() { return &bitboards_color[BITBOARD_BLACK_PIECES]; }
    uint64_t* pawns()       { return &bitboards_piece[BITBOARD_PAWN];         }
    uint64_t* knights()     { return &bitboards_piece[BITBOARD_KNIGHT];       }
    uint64_t* bishops()     { return &bitboards_piece[BITBOARD_BISHOP];       }
    uint64_t* rooks()       { return &bitboards_piece[BITBOARD_ROOK];         }
    uint64_t* kings()       { return &bitboards_piece[BITBOARD_KING];         }
    uint64_t* queens()      { return &bitboards_piece[BITBOARD_QUEEN];        }

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
        uint64_t bbCap  = bbTo * (move.flags & IS_CAPTURE);
 
        bitboards_color[move.bitboard_color_from]      ^= bbFrom;
        bitboards_color[move.bitboard_color_to]        ^= bbTo;
        bitboards_color[move.bitboard_color_capture]   ^= bbCap;
  
        bitboards_piece[move.from_piece]          ^= bbFrom;
        bitboards_piece[move.to_piece]            ^= bbTo;
        bitboards_piece[move.capture_piece]       ^= bbCap;

        turn    ^= TURN_BLACK;
    }

    void makeMove(const struct BetaMove& move)
    {
        _makeMove(move);
        // TODO: Push other meta data, such as can castle, can e.p etc onto stack

    }

    void unmakeMove(const struct BetaMove& move)
    {

        // TODO: Pop other meta data, such as can castle, can e.p etc off stack
        _makeMove(move);

    }

};


class BetaChess {

    public:

        BetaChess() { 
        
            m_blockers.computeBlockersAndBeyond();
            m_magicbb = new MagicBitboards();
            m_magicbb->computeTables(&m_blockers);
        
        }

        ~BetaChess() { delete m_magicbb; }

        uint64_t perft(int depth);


        /**
         *
         *  \return the number of moves generated
         *
         */
        int generate_moves(struct BetaMove* moves)
        {
            // TODO: Get rid of branch
            if (m_board.turn == TURN_WHITE)
                return _generate_moves_white(moves);
            else
                return _generate_moves_black(moves);
        }

    private:
    
        int _generate_moves_white(struct BetaMove* moves);
        int _generate_moves_black(struct BetaMove* moves);

        static int bitScanForward(uint64_t bb)
        {
            return __builtin_ctzll(bb);
        }

    private:

        BetaBoard m_board;
        Blockers  m_blockers;
        MagicBitboards* m_magicbb;
};
