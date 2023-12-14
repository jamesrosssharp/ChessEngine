/* vim: set et ts=4 sw=4: */

/*

   chessengine

Chess.h: Functions for dealing with chess boards and computing moves

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

#include <cstdint>

#include <string>

enum PieceTypes {
    WHITE_PAWN      = 1 << 0,
    WHITE_KNIGHT    = 1 << 1,
    WHITE_BISHOP    = 1 << 2,
    WHITE_ROOK      = 1 << 3,
    WHITE_KING      = 1 << 4,
    WHITE_QUEEN     = 1 << 5,
    BLACK_PAWN      = 1 << 6,
    BLACK_KNIGHT    = 1 << 7,
    BLACK_BISHOP    = 1 << 8,
    BLACK_ROOK      = 1 << 9,
    BLACK_KING      = 1 << 10,
    BLACK_QUEEN     = 1 << 11,
    NO_PIECE        = 1 << 12,
    WHITE_PIECES    = WHITE_PAWN | WHITE_KNIGHT | WHITE_BISHOP | WHITE_ROOK | WHITE_KING | WHITE_QUEEN,
    BLACK_PIECES    = BLACK_PAWN | BLACK_KNIGHT | BLACK_BISHOP | BLACK_ROOK | BLACK_KING | BLACK_QUEEN
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


struct ChessBoard {

    // Store the board as a series of "bit boards"
    uint64_t whitePawnsBoard;
    uint64_t whiteKnightsBoard;
    uint64_t whiteBishopsBoard;
    uint64_t whiteRooksBoard;
    uint64_t whiteQueensBoard;
    uint64_t whiteKingsBoard;

    uint64_t blackPawnsBoard;
    uint64_t blackKnightsBoard;
    uint64_t blackBishopsBoard;
    uint64_t blackRooksBoard;
    uint64_t blackQueensBoard;
    uint64_t blackKingsBoard;

    // Flags 
    bool m_isWhitesTurn;
    int m_can_en_passant_file;
    bool m_whiteKingHasMoved;
    bool m_blackKingHasMoved;
    bool m_whiteARookHasMoved;
    bool m_whiteHRookHasMoved;
    bool m_blackARookHasMoved;
    bool m_blackHRookHasMoved;

    uint64_t allWhitePieces() const
    {
        return  whitePawnsBoard |
                whiteKnightsBoard |
                whiteBishopsBoard |
                whiteRooksBoard |
                whiteQueensBoard |
                whiteKingsBoard;
    }

    uint64_t allBlackPieces() const
    {
        return  blackPawnsBoard |
                blackKnightsBoard |
                blackBishopsBoard |
                blackRooksBoard |
                blackQueensBoard |
                blackKingsBoard;
    }


};

class Chess {

    public:
    
        Chess();

        void resetBoard();

        void getLegalMovesForSquare(int x, int y, uint64_t& moveSquares, bool allowTakeKing = false);
        void getLegalMovesForBoardSquare(const ChessBoard& board, int x, int y, uint64_t& moveSquares, bool allowTakeKing = false);
        void printBoard(const ChessBoard& board);

        void makeMove(int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side);
        void makeMoveForBoard(ChessBoard& board, int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side, bool print = true);

        void getBestMove(int& x1, int& y1, int& x2, int& y2); 

    private:

        void evalBoard(const ChessBoard& board, double& white_score, double& black_score);

        void printBitBoard(uint64_t board);
        bool movePutsPlayerInCheck(const ChessBoard& board, int x1, int y1, int x2, int y2, bool white);
        bool kingIsInCheck(const ChessBoard& board, bool white);
        uint64_t movesForPlayer(const ChessBoard& board, bool white); 

        enum PieceTypes getPieceForSquare(const ChessBoard& board, int x, int y);
        std::string prettyPiece          (enum PieceTypes piece);
        void removePieceFromSquare       (ChessBoard& board, enum PieceTypes type, int x, int y);
        void addPieceToSquare            (ChessBoard& board, enum PieceTypes type, int x, int y);

        ChessBoard m_board;
 
};
