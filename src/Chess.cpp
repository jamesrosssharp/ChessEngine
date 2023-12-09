/* vim: set et ts=4 sw=4: */

/*

    chessengine

Chess.cpp: Functions for dealing with chess boards and computing moves

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

#include "Chess.h"
#include <cstdio>
#include <iostream>

Chess::Chess()
{
    resetBoard();
    printBoard();
}

void Chess::resetBoard()
{
    // Reset bit boards

    m_board.whitePawnsBoard   = 0xff00;
    m_board.whiteKnightsBoard = 0x42;
    m_board.whiteBishopsBoard = 0x24;
    m_board.whiteRooksBoard   = 0x81;
    m_board.whiteKingsBoard   = 0x10;
    m_board.whiteQueensBoard  = 0x8;

    m_board.blackPawnsBoard     = 0x00ff'0000'0000'0000;
    m_board.blackKnightsBoard   = 0x4200'0000'0000'0000;
    m_board.blackBishopsBoard   = 0x2400'0000'0000'0000;
    m_board.blackRooksBoard     = 0x8100'0000'0000'0000;
    m_board.blackKingsBoard     = 0x1000'0000'0000'0000;
    m_board.blackQueensBoard    = 0x0800'0000'0000'0000; 

}

void Chess::getLegalMovesForSquare(int x, int y, bool *moveSquares)
{
    // Init: zero out legal moves

    for (int i = 0; i < 64; i++)
        moveSquares[i] = false;

    // First, find the piece which is on this square

    enum PieceTypes piece = getPieceForSquare(x, y);

    std::cout << "Computing moves for " << prettyPiece(piece) << std::endl;

    // Compute legal moves for this piece

    switch(piece)
    {
        case WHITE_PAWN:

            // White pawn can move in increasing y 1 or two squares.
            // TODO: En passant, capturing

            // Simple moves
            for (int i = 1; i <= 2; i++)
            {

                // Check if blocked by a piece

                if (getPieceForSquare(x, y + i) != NO_PIECE) continue;

                // If not, we can move here.

                moveSquares[x + (y + i)*8] = true;   
            }

            break;

    }

}

void Chess::printBoard()
{
    for (int y = 7; y >= 0; y--)
    {
        for (int x = 0; x <= 7; x++)
        {
            uint64_t sq = 1ULL << (y*8 + x);

            if (m_board.whitePawnsBoard & sq)
                printf("P");
            else if (m_board.whiteKnightsBoard & sq)
                printf("N");
            else if (m_board.whiteBishopsBoard & sq)
                printf("B");
            else if (m_board.whiteRooksBoard & sq)
                printf("R");
            else if (m_board.whiteKingsBoard & sq)
                printf("K");
            else if (m_board.whiteQueensBoard & sq)
                printf("Q");
            else if (m_board.blackPawnsBoard & sq)
                printf("p");
            else if (m_board.blackKnightsBoard & sq)
                printf("n");
            else if (m_board.blackBishopsBoard & sq)
                printf("b");
            else if (m_board.blackRooksBoard & sq)
                printf("r");
            else if (m_board.blackKingsBoard & sq)
                printf("k");
            else if (m_board.blackQueensBoard & sq)
                printf("q");
            else printf(".");
            printf(" "); 
        }
        printf("\n");
    }

}

enum PieceTypes Chess::getPieceForSquare(int x, int y)
{
    uint64_t sq = 1ULL << (y*8 + x);

    if (m_board.whitePawnsBoard & sq)
        return WHITE_PAWN; 
    else if (m_board.whiteKnightsBoard & sq)
        return WHITE_KNIGHT;
    else if (m_board.whiteBishopsBoard & sq)
        return WHITE_BISHOP;
    else if (m_board.whiteRooksBoard & sq)
        return WHITE_ROOK;
    else if (m_board.whiteKingsBoard & sq)
        return WHITE_KING;
    else if (m_board.whiteQueensBoard & sq)
        return WHITE_QUEEN;
    else if (m_board.blackPawnsBoard & sq)
        return BLACK_PAWN;
    else if (m_board.blackKnightsBoard & sq)
        return BLACK_KNIGHT;
    else if (m_board.blackBishopsBoard & sq)
        return BLACK_BISHOP;
    else if (m_board.blackRooksBoard & sq)
        return BLACK_ROOK;
    else if (m_board.blackKingsBoard & sq)
        return BLACK_KING;
    else if (m_board.blackQueensBoard & sq)
        return BLACK_QUEEN;
    return NO_PIECE;
}

std::string Chess::prettyPiece(enum PieceTypes piece)
{
    switch (piece)
    {
        case WHITE_PAWN:
            return "White Pawn";
        case WHITE_KNIGHT:
            return "White Knight";    
        case WHITE_BISHOP:
            return "White Bishop";
        case WHITE_ROOK:
            return "White Rook";
        case WHITE_KING:
            return "White King";
        case WHITE_QUEEN:
            return "White Queen";
        case BLACK_PAWN:
            return "Black Pawn";
        case BLACK_KNIGHT:
            return "Black Knight";    
        case BLACK_BISHOP:
            return "Black Bishop";
        case BLACK_ROOK:
            return "Black Rook";
        case BLACK_KING:
            return "Black King";
        case BLACK_QUEEN:
            return "Black Queen";
        default:
            break;
    }
    return "None";
}
