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
//#include <iostream>
#include <vector>
#include <chrono>

const std::vector<std::pair<int, int>> knightMoves = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};
const std::vector<std::pair<int, int>> pawnCaptures = {{-1, 1}, {1, 1}};
const std::vector<std::pair<int, int>> queenMoves = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
const std::vector<std::pair<int, int>> kingMoves = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
const std::vector<std::pair<int, int>> bishopMoves = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
const std::vector<std::pair<int, int>> rookMoves = {{1, 0}, {0, 1},  {-1, 0}, {0, -1}};

const double pawnPositionWeights[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
    50,  50, 50, 50, 50, 50, 50, 50,
    10,  10, 20, 30, 30, 20, 10, 10,
     5,  5,  10, 25, 25, 10,  5,  5,
     0,  0,  0,  20, 20,  0,  0,  0,
     5, -5, -10,  0,  0,-10, -5,  5,
     5, 10, 10, -20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0 
};

const double knightPositionWeights[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

const double bishopsPositionWeights[64] = {
-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20,
};

const double rooksPositionWeights[64] = {
 0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0
};

const double queenPositionWeights[64] = {
-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
  0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20
};

const double kingPositionWeights[64] = {
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-20,-30,-30,-40,-40,-30,-30,-20,
-10,-20,-20,-20,-20,-20,-20,-10,
 20, 20,  0,  0,  0,  0, 20, 20,
 20, 30, 10,  0,  0, 10, 30, 20
};

#define INFINITY 1e10

Chess::Chess()  :
    m_totalCheckTestMicroseconds(0),
    m_totalGenerateMoveMicroseconds(0),
    m_totalEvaluateMicroseconds(0),
    m_totalGenLegalMicroseconds(0)
{
    computeBlockersAndBeyond();
    resetBoard();
    printBoard(m_board);

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

    m_board.m_isWhitesTurn = true;

    m_board.m_can_en_passant_file = INVALID_FILE;
    m_board.m_whiteKingHasMoved = false;
    m_board.m_blackKingHasMoved = false;
    m_board.m_whiteARookHasMoved = false;
    m_board.m_whiteHRookHasMoved = false;
    m_board.m_blackARookHasMoved = false;
    m_board.m_blackHRookHasMoved = false;
}

void Chess::getLegalMovesForSquare(int x, int y, uint64_t &moveSquares)
{
    getLegalMovesForBoardSquare(m_board, x, y, moveSquares);
}

void Chess::getLegalMovesForBoardSquare(const ChessBoard& board, int x, int y, uint64_t &moveSquares)
{
    // Init: zero out legal moves

    moveSquares = 0;

    // First, find the piece which is on this square

    enum PieceTypes piece = getPieceForSquare(board, x, y);

    // Check if it's this colors move

    if (board.m_isWhitesTurn && !(piece & WHITE_PIECES)) return;
    if (!board.m_isWhitesTurn && !(piece & BLACK_PIECES)) return;

    // Compute legal moves for this piece

    switch(piece)
    {
        case WHITE_PAWN:
        case BLACK_PAWN:
        {
            int multiplier = (piece == WHITE_PAWN) ? 1 : -1;

            // White pawn can move in increasing y 1 or two squares.
            // TODO: En passant, capturing

            // Simple moves
            for (int i = 1; i <= 2; i++)
            {
                int xx = x;
                int yy = y + i*multiplier;

                // Check if blocked by a piece

                if (getPieceForSquare(board, x, yy) != NO_PIECE) break;

                // We can only move 2 squares on first move.

                if ((piece == WHITE_PAWN) && (i == 2) && (y != SECOND_RANK)) break;
                if ((piece == BLACK_PAWN) && (i == 2) && (y != SEVENTH_RANK)) break;

                // Cannot make move if it puts the player in check
                if (movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

                // If not, we can move here.

                moveSquares |= COORD_TO_BIT(xx, yy); 
            }

            // Simple capturing
       
            for (auto p : pawnCaptures)
            {
                int xx = x + p.first;
                int yy = y + p.second*multiplier;

                if (xx < A_FILE) continue;
                if (xx > H_FILE) continue;
                if ((piece == WHITE_PAWN) && !(getPieceForSquare(board, xx, yy) & BLACK_PIECES)) continue;
                if ((piece == BLACK_PAWN) && !(getPieceForSquare(board, xx, yy) & WHITE_PIECES)) continue;

                    // Can never take the king
                if ((piece == WHITE_PAWN) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                if ((piece == BLACK_PAWN) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;

                // Cannot make move if it puts the player in check
                if (movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

                moveSquares |= COORD_TO_BIT(xx, yy);
            }

            // En passant

            for (auto p : pawnCaptures)
            {
                int xx = x + p.first;
                int yy = y + p.second*multiplier;
           
                if (xx < A_FILE) continue;
                if (xx > H_FILE) continue;
            
                if (! ( ((piece == WHITE_PAWN) && (board.m_can_en_passant_file == xx) && (getPieceForSquare(board, xx, yy - multiplier) & BLACK_PIECES)) ||
                        ((piece == BLACK_PAWN) && (board.m_can_en_passant_file == xx) && (getPieceForSquare(board, xx, yy - multiplier) & WHITE_PIECES)) )) continue;

                // Cannot make move if it puts the player in check
                if (movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

                moveSquares |= COORD_TO_BIT(xx, yy); 
            }

            break;
        }
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:

            for (auto p : knightMoves)
            {

               int xx = x + p.first;
               int yy = y + p.second;

               if ((xx >= A_FILE) && (xx <= H_FILE) && (yy >= FIRST_RANK) && (yy <= EIGHTH_RANK))
               {
                    if ((piece == WHITE_KNIGHT) && (COORD_TO_BIT(xx, yy) & board.allWhitePieces())) continue;
                    if ((piece == BLACK_KNIGHT) && (COORD_TO_BIT(xx, yy) & board.allBlackPieces())) continue;

                    // Can never take the king
                    if ((piece == WHITE_KNIGHT) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                    if ((piece == BLACK_KNIGHT) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;

                    // Cannot make move if it puts the player in check
                    if (movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

                    moveSquares |= COORD_TO_BIT(xx, yy); 
               }

            }

            break;
        case WHITE_BISHOP:
        case BLACK_BISHOP:

            for (auto p : bishopMoves)
            {
                for (int k = 1; k < 8; k++)
                {
                   int xx = x + p.first * k;
                   int yy = y + p.second * k;

                   if ((xx >= A_FILE) && (xx <= H_FILE) && (yy >= FIRST_RANK) && (yy <= EIGHTH_RANK))
                   {
                        if ((piece == WHITE_BISHOP) && (COORD_TO_BIT(xx, yy) & board.allWhitePieces())) break;
                        if ((piece == BLACK_BISHOP) && (COORD_TO_BIT(xx, yy) & board.allBlackPieces())) break;
                     
                        if ((piece == WHITE_BISHOP) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                        if ((piece == BLACK_BISHOP) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;

                        // Cannot make move if it puts the player in check
                        if (!movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) 
                            moveSquares |= COORD_TO_BIT(xx, yy); 
                  
                        if ((piece == WHITE_BISHOP) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;
                        if ((piece == BLACK_BISHOP) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;

                   }

               }
            }

            break;
        case WHITE_ROOK:
        case BLACK_ROOK:

            for (auto p : rookMoves)
            {
                for (int k = 1; k < 8; k++)
                {
                   int xx = x + p.first * k;
                   int yy = y + p.second * k;

                   uint64_t sq = COORD_TO_BIT(xx, yy); 

                   if ((xx >= A_FILE) && (xx <= H_FILE) && (yy >= FIRST_RANK) && (yy <= EIGHTH_RANK))
                   {
                        if ((piece == WHITE_ROOK) && (sq & board.allWhitePieces())) break;
                        if ((piece == BLACK_ROOK) && (sq & board.allBlackPieces())) break;
                     
                        if ((piece == WHITE_ROOK) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                        if ((piece == BLACK_ROOK) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;

                        // Cannot make move if it puts the player in check
                        if (!movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) 
                            moveSquares |= sq; 
                  
                        if ((piece == WHITE_ROOK) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;
                        if ((piece == BLACK_ROOK) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;

                   }

               }
            }

            break;
        case WHITE_QUEEN:
        case BLACK_QUEEN:

            for (auto p : queenMoves)
            {
                for (int k = 1; k < 8; k++)
                {
                   int xx = x + p.first * k;
                   int yy = y + p.second * k;

                   if ((xx >= A_FILE) && (xx <= H_FILE) && (yy >= FIRST_RANK) && (yy <= EIGHTH_RANK))
                   {
                        if ((piece == WHITE_QUEEN) && (COORD_TO_BIT(xx, yy) & board.allWhitePieces())) break;
                        if ((piece == BLACK_QUEEN) && (COORD_TO_BIT(xx, yy) & board.allBlackPieces())) break;
                                    
                        if ((piece == WHITE_QUEEN) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                        if ((piece == BLACK_QUEEN) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;

                        // Cannot make move if it puts the player in check
                        if (!movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES))
                            moveSquares |= COORD_TO_BIT(xx, yy); 
                  
                        if ((piece == WHITE_QUEEN) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;
                        if ((piece == BLACK_QUEEN) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;

                   }

               }
            }

            break;
        case WHITE_KING:
        case BLACK_KING:

            for (auto p : kingMoves)
            {
               int xx = x + p.first;
               int yy = y + p.second;

               if ((xx >= A_FILE) && (xx <= H_FILE) && (yy >= FIRST_RANK) && (yy <= EIGHTH_RANK))
               {
                    if ((piece == WHITE_KING) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) continue;
                    if ((piece == BLACK_KING) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) continue;

                    if ((piece == WHITE_KING) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                    if ((piece == BLACK_KING) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;

                    // Cannot make move if it puts the player in check
                    if (movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

                    moveSquares |= COORD_TO_BIT(xx, yy);
              
               }

            }

            // Check for castling moves

            if ((piece == WHITE_KING) && (!board.m_whiteKingHasMoved) && !kingIsInCheck(board, true))
            {

                // Check King side castling
                if ((getPieceForSquare(board, F_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, G_FILE, FIRST_RANK) == NO_PIECE) &&
                    !board.m_whiteHRookHasMoved)
                {
                    if ((moveSquares & COORD_TO_BIT(F_FILE, FIRST_RANK)) && (!(movePutsPlayerInCheck(board, x, y, G_FILE, FIRST_RANK, true))))
                        moveSquares |= COORD_TO_BIT(G_FILE, FIRST_RANK); 
                }

                // Check Queen side castling
                if ((getPieceForSquare(board, D_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, C_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, B_FILE, FIRST_RANK) == NO_PIECE) &&
                    !board.m_whiteARookHasMoved)
                {
                    if ((moveSquares & COORD_TO_BIT(D_FILE, FIRST_RANK)) && (!(movePutsPlayerInCheck(board, x, y, C_FILE, FIRST_RANK, true))))
                        moveSquares |= COORD_TO_BIT(C_FILE, FIRST_RANK);
                }

            }

            if ((piece == BLACK_KING) && (!board.m_blackKingHasMoved) && !kingIsInCheck(board, false))
            {

                // Check King side castling
                if ((getPieceForSquare(board, F_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, G_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    !board.m_blackHRookHasMoved)
                {
                    if ((moveSquares & COORD_TO_BIT(F_FILE, EIGHTH_RANK)) && (!(movePutsPlayerInCheck(board, x, y, G_FILE, EIGHTH_RANK, false))))
                        moveSquares |= COORD_TO_BIT(G_FILE, EIGHTH_RANK); 
                }

                // Check Queen side castling
                if ((getPieceForSquare(board, D_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, C_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, B_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    !board.m_blackARookHasMoved)
                {
                    if ((moveSquares & COORD_TO_BIT(D_FILE, EIGHTH_RANK)) && (!(movePutsPlayerInCheck(board, x, y, C_FILE, EIGHTH_RANK, false))))
                        moveSquares |= COORD_TO_BIT(C_FILE, EIGHTH_RANK); 
                }

            }
            break;
        default:
           break; 
    }

}

void Chess::printBitBoard(uint64_t board)
{
    for (int y = 7; y >= 0; y--)
    {
        for (int x = 0; x <= 7; x++)
        {
            uint64_t sq = COORD_TO_BIT(x, y);

            if (board & sq) printf("* ");
            else printf(". "); 
        }
        printf("\n");
    }
}    

void Chess::printBoard(const ChessBoard& board)
{
    for (int y = 7; y >= 0; y--)
    {
        for (int x = 0; x <= 7; x++)
        {
            uint64_t sq = COORD_TO_BIT(x, y);

            if (board.whitePawnsBoard & sq)
                printf("P");
            else if (board.whiteKnightsBoard & sq)
                printf("N");
            else if (board.whiteBishopsBoard & sq)
                printf("B");
            else if (board.whiteRooksBoard & sq)
                printf("R");
            else if (board.whiteKingsBoard & sq)
                printf("K");
            else if (board.whiteQueensBoard & sq)
                printf("Q");
            else if (board.blackPawnsBoard & sq)
                printf("p");
            else if (board.blackKnightsBoard & sq)
                printf("n");
            else if (board.blackBishopsBoard & sq)
                printf("b");
            else if (board.blackRooksBoard & sq)
                printf("r");
            else if (board.blackKingsBoard & sq)
                printf("k");
            else if (board.blackQueensBoard & sq)
                printf("q");
            else printf(".");
            printf(" "); 
        }
        printf("\n");
    }

}

enum PieceTypes Chess::getPieceForSquare(const ChessBoard& board, int x, int y)
{
    uint64_t sq = COORD_TO_BIT(x, y);

    if (board.whitePawnsBoard & sq)
        return WHITE_PAWN; 
    else if (board.whiteKnightsBoard & sq)
        return WHITE_KNIGHT;
    else if (board.whiteBishopsBoard & sq)
        return WHITE_BISHOP;
    else if (board.whiteRooksBoard & sq)
        return WHITE_ROOK;
    else if (board.whiteKingsBoard & sq)
        return WHITE_KING;
    else if (board.whiteQueensBoard & sq)
        return WHITE_QUEEN;
    else if (board.blackPawnsBoard & sq)
        return BLACK_PAWN;
    else if (board.blackKnightsBoard & sq)
        return BLACK_KNIGHT;
    else if (board.blackBishopsBoard & sq)
        return BLACK_BISHOP;
    else if (board.blackRooksBoard & sq)
        return BLACK_ROOK;
    else if (board.blackKingsBoard & sq)
        return BLACK_KING;
    else if (board.blackQueensBoard & sq)
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

void Chess::removePieceFromSquare(ChessBoard& board, enum PieceTypes type, int x, int y)
{
    uint64_t sq   = COORD_TO_BIT(x, y);

    switch (type)
    {
        case WHITE_PAWN:
            board.whitePawnsBoard &= ~sq;
            break;
        case BLACK_PAWN:
            board.blackPawnsBoard &= ~sq;            
            break;
        case WHITE_KNIGHT:
            board.whiteKnightsBoard &= ~sq;
            break;
        case BLACK_KNIGHT:
            board.blackKnightsBoard &= ~sq;
            break;
        case WHITE_BISHOP:
            board.whiteBishopsBoard &= ~sq;
            break;
        case BLACK_BISHOP:
            board.blackBishopsBoard &= ~sq;
            break;
        case WHITE_ROOK:
            board.whiteRooksBoard &= ~sq;
            break;
        case BLACK_ROOK:
            board.blackRooksBoard &= ~sq;
            break;
        case WHITE_KING:
            board.whiteKingsBoard &= ~sq;
            break;
        case BLACK_KING:
            board.blackKingsBoard &= ~sq;
            break;
        case WHITE_QUEEN:
            board.whiteQueensBoard &= ~sq;
            break;
        case BLACK_QUEEN:
            board.blackQueensBoard &= ~sq;
            break;
        default: ; 
    }

}
        
void Chess::addPieceToSquare(ChessBoard& board, enum PieceTypes type, int x, int y)
{

    uint64_t sq   = COORD_TO_BIT(x, y);

    switch (type)
    {
        case WHITE_PAWN:
            board.whitePawnsBoard |= sq;
            break;
        case BLACK_PAWN:
            board.blackPawnsBoard |= sq;            
            break;
        case WHITE_KNIGHT:
            board.whiteKnightsBoard |= sq;
            break;
        case BLACK_KNIGHT:
            board.blackKnightsBoard |= sq;
            break;
        case WHITE_BISHOP:
            board.whiteBishopsBoard |= sq;
            break;
        case BLACK_BISHOP:
            board.blackBishopsBoard |= sq;
            break;
        case WHITE_ROOK:
            board.whiteRooksBoard |= sq;
            break;
        case BLACK_ROOK:
            board.blackRooksBoard |= sq;
            break;
        case WHITE_KING:
            board.whiteKingsBoard |= sq;
            break;
        case BLACK_KING:
            board.blackKingsBoard |= sq;
            break;
        case WHITE_QUEEN:
            board.whiteQueensBoard |= sq;
            break;
        case BLACK_QUEEN:
            board.blackQueensBoard |= sq;
            break;
        default: ; 
    }
}

void Chess::makeMove(int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side)
{
    makeMoveForBoard(m_board, x1, y1, x2, y2, ep, castle_kings_side, castle_queens_side, true, true);
}


void Chess::makeMoveForBoard(ChessBoard& board, int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side, bool print, bool recompute_legal)
{

    ep = false;
    castle_kings_side = false;
    castle_queens_side = false;

    enum PieceTypes start_piece = getPieceForSquare(board, x1, y1);
    enum PieceTypes end_piece   = getPieceForSquare(board, x2, y2);

    if (end_piece != NO_PIECE)
    {
        // Capture... remove piece from board
        removePieceFromSquare(board, end_piece, x2, y2);
    }

    // If this is an en passant pawn capture, need to remove captured en passant pawn
    int yy = (start_piece == WHITE_PAWN) ? y2 - 1 : y2 + 1;

    if (
        ((start_piece == WHITE_PAWN) && (x2 != x1) && (board.m_can_en_passant_file == x2) && (getPieceForSquare(board, x2, yy) == BLACK_PAWN)) ||
        ((start_piece == BLACK_PAWN) && (x2 != x1) && (board.m_can_en_passant_file == x2) && (getPieceForSquare(board, x2, yy) == WHITE_PAWN)))
    {
        end_piece = getPieceForSquare(board, x2, yy);
        removePieceFromSquare(board, end_piece, x2, yy); 
        ep = true;
    }

    // If this move is a first move of a pawn, update "can_enpassant_file"
    if (((start_piece == WHITE_PAWN) && (y1 == SECOND_RANK) && (y2 == FOURTH_RANK)) ||
        ((start_piece == BLACK_PAWN) && (y1 == SEVENTH_RANK) && (y2 == FIFTH_RANK)))
    {
        board.m_can_en_passant_file = x1;
    }
    else
        board.m_can_en_passant_file = INVALID_FILE;

    // If this piece is a king, flag that the king has moved
    if (start_piece == WHITE_KING)
        board.m_whiteKingHasMoved = true;
    else if (start_piece == BLACK_KING)
        board.m_blackKingHasMoved = true;

    // Check if rooks have moved (disable castle)
    if (start_piece == WHITE_ROOK)
    {
        if (x1 == A_FILE) board.m_whiteARookHasMoved = true;
        else if (x1 == H_FILE) board.m_whiteHRookHasMoved = true;
    }
    else if (start_piece == BLACK_ROOK)
    {
        if (x1 == A_FILE) board.m_blackARookHasMoved = true;
        else if (x1 == H_FILE) board.m_blackHRookHasMoved = true;
    } 

    // Check for castling

    if ((start_piece == WHITE_KING) && (abs(x2 - x1) > 1))
    {

        if (x2 == G_FILE)
        {
            // Castle King side
            removePieceFromSquare(board, WHITE_ROOK, H_FILE, FIRST_RANK);
            addPieceToSquare(board, WHITE_ROOK, F_FILE, FIRST_RANK);
            board.m_whiteHRookHasMoved = true;
            castle_kings_side = true;
        }
        else if (x2 == C_FILE)
        {
            // Castle Queen side
            removePieceFromSquare(board, WHITE_ROOK, A_FILE, FIRST_RANK);
            addPieceToSquare(board, WHITE_ROOK, D_FILE, FIRST_RANK);
            board.m_whiteARookHasMoved = true;
            castle_queens_side = true;
        }

    }
    else if ((start_piece == BLACK_KING) && (abs(x2 - x1) > 1))
    {

        if (x2 == G_FILE)
        {
            // Castle King side
            removePieceFromSquare(board, BLACK_ROOK, H_FILE, EIGHTH_RANK);
            addPieceToSquare(board, BLACK_ROOK, F_FILE, EIGHTH_RANK);
            board.m_blackHRookHasMoved = true;
            castle_kings_side = true;
        }
        else if (x2 == C_FILE)
        {
            // Castle Queen side
            removePieceFromSquare(board, BLACK_ROOK, A_FILE, EIGHTH_RANK);
            addPieceToSquare(board, BLACK_ROOK, D_FILE, EIGHTH_RANK);
            board.m_blackARookHasMoved = true;
            castle_queens_side = true;
        }

    }

    removePieceFromSquare(board, start_piece, x1, y1);
    addPieceToSquare(board, start_piece, x2, y2);
    
    board.m_isWhitesTurn = !board.m_isWhitesTurn;

    // Compute legal moves for board
    if (recompute_legal)
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();
            
        board.m_legalMoves.clear();
        getLegalMovesForBoardAsVector(board, board.m_legalMoves);

        auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
        m_totalGenLegalMicroseconds += usecs.count();

   }

    if (print)
    {
        printBoard(board);
        if (kingIsInCheck(board, board.m_isWhitesTurn))
        {
            if (movesForPlayer(board, board.m_isWhitesTurn) == 0)
                printf("Checkmate!\n");
            else
                printf("Check!\n");
        }
    
        double w, b;

        evalBoardFaster(board, w, b);
        printf("Scores: white = %f  black = %f\n", w, b);
    }


}

bool Chess::kingIsInCheck(const ChessBoard& board, bool white)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();
   
    // Check for check from pawns

    if (white)
    {
        uint64_t bb = board.blackPawnsBoard;

        while (bb)
        {
            uint64_t bb2 = bb & -bb;
            uint64_t attacks = m_pawnAttacksBlack[bitScanForward(bb2)];
            if (board.whiteKingsBoard & attacks) goto check; 
            bb &= bb - 1;
        }
    }
    else
    {
        uint64_t bb = board.whitePawnsBoard;

        while (bb)
        {
            uint64_t bb2 = bb & -bb;
            uint64_t attacks = m_pawnAttacksWhite[bitScanForward(bb2)];
            if (board.blackKingsBoard & attacks) goto check; 
            bb &= bb - 1;
        }
    }

    // Check for check from knights

    {
    
        uint64_t bb = white ? board.blackKnightsBoard : board.whiteKnightsBoard;

        while(bb)
        {
            uint64_t bb2 = bb & -bb; // Isolate LS1B
            uint64_t moves = pieceAttacks(PIECE_KNIGHT, bitScanForward(bb2), board.allWhitePieces() | board.allBlackPieces());
            if (moves & (white ? board.whiteKingsBoard : board.blackKingsBoard)) goto check;
            bb &= bb - 1;
        }

    }

    // Check for check from bishops

    {
    
        uint64_t bb = white ? board.blackBishopsBoard : board.whiteBishopsBoard;

        while(bb)
        {
            uint64_t bb2 = bb & -bb; // Isolate LS1B
            uint64_t moves = pieceAttacks(PIECE_BISHOP, bitScanForward(bb2), board.allWhitePieces() | board.allBlackPieces());
            if (moves & (white ? board.whiteKingsBoard : board.blackKingsBoard)) goto check;
            bb &= bb - 1;
        }

    }

    // Check for check from rooks
   
     {
    
        uint64_t bb = white ? board.blackRooksBoard : board.whiteRooksBoard;

        while(bb)
        {
            uint64_t bb2 = bb & -bb; // Isolate LS1B
            uint64_t moves = pieceAttacks(PIECE_ROOK, bitScanForward(bb2), board.allWhitePieces() | board.allBlackPieces());
            if (moves & (white ? board.whiteKingsBoard : board.blackKingsBoard)) goto check;
            bb &= bb - 1;
        }

    }

    // Check for check from queens

     {
    
        uint64_t bb = white ? board.blackQueensBoard : board.whiteQueensBoard;

        while(bb)
        {
            uint64_t bb2 = bb & -bb; // Isolate LS1B
            uint64_t moves = pieceAttacks(PIECE_QUEEN, bitScanForward(bb2), board.allWhitePieces() | board.allBlackPieces());
            if (moves & (white ? board.whiteKingsBoard : board.blackKingsBoard)) goto check;
            bb &= bb - 1;
        }

    }

    // Check for possible check from Kings... so that Kings can't get near each other

    {
        uint64_t bb = white ? board.blackKingsBoard : board.whiteKingsBoard;

        while(bb)
        {
            uint64_t bb2 = bb & -bb; // Isolate LS1B
            uint64_t moves = pieceAttacks(PIECE_KING, bitScanForward(bb2), board.allWhitePieces() | board.allBlackPieces());
            if (moves & (white ? board.whiteKingsBoard : board.blackKingsBoard)) goto check;
            bb &= bb - 1;
        }

    }
   
    {
        auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
        m_totalCheckTestMicroseconds += usecs.count();
    }
    return false;
check:
    { 
        auto usecs2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
        m_totalCheckTestMicroseconds += usecs2.count();
    }
    return true;
}

bool Chess::movePutsPlayerInCheck(const ChessBoard& board, int x1, int y1, int x2, int y2, bool white)
{
    ChessBoard boardCopy = board;

    bool ep;
    bool castle_kings_side;
    bool castle_queens_side;

    makeMoveForBoard(boardCopy, x1, y1, x2, y2, ep, castle_kings_side, castle_queens_side, false);

    bool check = kingIsInCheck(boardCopy, white);

    return check;

}

uint64_t Chess::movesForPlayer(const ChessBoard& board, bool white)
{
    uint64_t bb = (white) ? board.allWhitePieces() : board.allBlackPieces();
    uint64_t legalMoves = 0;

    ChessBoard b = board;
    b.m_isWhitesTurn = white;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            uint64_t sq = COORD_TO_BIT(x, y);

            if (bb & sq) 
            {
                uint64_t temp = 0;
                getLegalMovesForBoardSquare(b, x, y, temp);
                legalMoves |= temp;
            }
        }
    }
    return legalMoves;
}

double Chess::sum_bits_and_multiply(uint64_t bb, double multiplier)
{
    int sum = 0;
    
    if (bb && !(bb & (bb - 1))) return multiplier;    // Power of two: 1 bit
    
    while (bb) {
       sum++;
       bb &= bb - 1;
    }

    return sum * multiplier;
}

double Chess::multiply_bits_with_weights(uint64_t bb, const double* weights)
{
    double sum = 0;
    
    for ( ; bb != 0; bb &= bb - 1)
    {
        int sq = bitScanForward(bb);
        sum += weights[sq];
    } 
    
    return sum;
}

double Chess::multiply_bits_with_weights_reverse(uint64_t bb, const double* weights)
{
    double sum = 0;
   
    for ( ; bb != 0; bb &= bb - 1)
    {
        int sq = bitScanForward(bb);
        sq = (sq&7) + (7-(sq>>3))*8;
        sum += weights[sq];
    } 

    return sum;
}

void Chess::evalBoard(const ChessBoard& board, double& white_score, double& black_score)
{

    white_score = 0.0;
    black_score = 0.0;

    // Compute material
    white_score += sum_bits_and_multiply(board.whitePawnsBoard, 1.0);
    white_score += sum_bits_and_multiply(board.whiteKnightsBoard, 3.0);
    white_score += sum_bits_and_multiply(board.whiteBishopsBoard, 3.0);
    white_score += sum_bits_and_multiply(board.whiteRooksBoard, 5.0);
    white_score += sum_bits_and_multiply(board.whiteQueensBoard, 9.0);


    black_score += sum_bits_and_multiply(board.blackPawnsBoard, 1.0);
    black_score += sum_bits_and_multiply(board.blackKnightsBoard, 3.0);
    black_score += sum_bits_and_multiply(board.blackBishopsBoard, 3.0);
    black_score += sum_bits_and_multiply(board.blackRooksBoard, 5.0);
    black_score += sum_bits_and_multiply(board.blackQueensBoard, 9.0);

    // Compute positional heuristics

    white_score += 0.01*multiply_bits_with_weights_reverse(board.whitePawnsBoard,   pawnPositionWeights);
    white_score += 0.01*multiply_bits_with_weights_reverse(board.whiteKnightsBoard, knightPositionWeights);
    white_score += 0.01*multiply_bits_with_weights_reverse(board.whiteBishopsBoard, bishopsPositionWeights);
    white_score += 0.01*multiply_bits_with_weights_reverse(board.whiteRooksBoard,   rooksPositionWeights);
    white_score += 0.01*multiply_bits_with_weights_reverse(board.whiteQueensBoard,   queenPositionWeights);
    white_score += 0.01*multiply_bits_with_weights_reverse(board.whiteKingsBoard,   kingPositionWeights);

    black_score += 0.01*multiply_bits_with_weights(board.blackPawnsBoard, pawnPositionWeights);
    black_score += 0.01*multiply_bits_with_weights(board.blackKnightsBoard, knightPositionWeights);
    black_score += 0.01*multiply_bits_with_weights(board.blackBishopsBoard, bishopsPositionWeights);
    black_score += 0.01*multiply_bits_with_weights(board.blackRooksBoard, rooksPositionWeights);
    black_score += 0.01*multiply_bits_with_weights(board.blackQueensBoard, queenPositionWeights);
    black_score += 0.01*multiply_bits_with_weights(board.blackKingsBoard,  kingPositionWeights);

    if (board.m_isWhitesTurn)
    {
        int movesWhite = board.m_legalMoves.size();
        //white_score += 0.01*sum_bits(movesWhite);

        // Compute checkmate

        if (kingIsInCheck(board, true) && (movesWhite == 0))
        {
            black_score += 900.0;
        }
    }
    else 
    {
        int movesBlack = board.m_legalMoves.size();
        //white_score += 0.01*sum_bits(movesBlack);

        // Compute checkmate

        if (kingIsInCheck(board, false) && (movesBlack == 0))
        {
            white_score += 900.0;
        }
    }

}

void Chess::getLegalMovesForBoardAsVectorSlow(const ChessBoard& board, std::vector<ChessMove>& vec)
{
    uint64_t bb = (board.m_isWhitesTurn) ? board.allWhitePieces() : board.allBlackPieces();

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            uint64_t sq = COORD_TO_BIT(x, y);

            if (bb & sq) 
            {
                uint64_t temp = 0;
                getLegalMovesForBoardSquare(board, x, y, temp);
            
                for (int xx = 0; xx < 8; xx++)
                {
        
                    for (int yy = 0; yy < 8; yy++)
                    {
                        // Compute score
           
                        if (temp & COORD_TO_BIT(xx, yy))
                        {
                            vec.emplace_back(x, y, xx, yy);
                        } 
                   }
                }
            }
        }
    }
}

void Chess::getLegalMovesForBoardAsVector(const ChessBoard& board, std::vector<ChessMove>& vec)
{
    uint64_t bb = (board.m_isWhitesTurn) ? board.allWhitePieces() : board.allBlackPieces();

    while (bb)
    {
        int idx = 63 - __builtin_clzl(bb);
        int x = idx & 7;
        int y = idx >> 3;

        uint64_t temp = 0;
        getLegalMovesForBoardSquare(board, x, y, temp);
        
        while (temp)
        {
            int idx2 = 63 - __builtin_clzl(temp);
            int xx = idx2 & 7;
            int yy = idx2 >> 3;

            vec.emplace_back(x, y, xx, yy);

            temp &= ~(1ULL << idx2); 
        }

        bb &= ~(1ULL << idx);
    }

}



double Chess::minimaxAlphaBeta(const ChessBoard& board, bool white, ChessMove& move, bool maximizing, int depth, uint64_t& npos, double alpha, double beta)
{

    npos++;

    if ((depth == 0) || (board.m_legalMoves.size() == 0))
    {
        double whiteScore = 0.0, blackScore = 0.0;
     
        std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();
            evalBoard(board, whiteScore, blackScore); 
        auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
        m_totalEvaluateMicroseconds += usecs.count();


        if (white)
        {
            return whiteScore - blackScore;
        }
        else
        {
            return blackScore - whiteScore; 
        }
    }

    if (maximizing)
    {
        double score = -INFINITY;

        for (const auto & m : board.m_legalMoves)
        {
            ChessBoard b = board;
            bool ep, castle_kings_side, castle_queens_side;
            ChessMove mm;
            std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();
            
            makeMoveForBoard(b, m.x1, m.y1, m.x2, m.y2, ep, castle_kings_side, castle_queens_side, false, true);

            auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
            m_totalGenerateMoveMicroseconds += usecs.count();

            double newscore = minimaxAlphaBeta(b, white, mm, false, depth - 1, npos, alpha, beta); 
            if (newscore > score)
            {
                score = newscore;
                move = m;
            }
            alpha = std::max(alpha, newscore);
            if (newscore >= beta)
                break;
        }
        return score;
    }
    else
    {
        double score = INFINITY;
        for (const auto & m : board.m_legalMoves)
        {
            ChessBoard b = board;
            bool ep, castle_kings_side, castle_queens_side;
            ChessMove mm;

            std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();
            
            makeMoveForBoard(b, m.x1, m.y1, m.x2, m.y2, ep, castle_kings_side, castle_queens_side, false, true);
            
            auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
            m_totalGenerateMoveMicroseconds += usecs.count();

            double newscore = minimaxAlphaBeta(b, white, mm, true, depth - 1, npos, alpha, beta); 
            if (newscore < score)
            {
                score = newscore;
                move = m;
            }
            beta = std::min(beta, newscore);
            if (newscore <= alpha)
                break;
        }
        return score;
    }

    return 0.0;
}



void Chess::getBestMove(int& x1, int& y1, int& x2, int& y2)
{

    ChessMove m;

    uint64_t npos = 0;

    std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();
 
    double maxScore = minimaxAlphaBetaFaster(m_board, m_board.m_isWhitesTurn, m, true, 6, npos, -INFINITY, INFINITY);

    auto msecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
    
    printf("Number of positions: %ld (%1.3f secs) = %1.3f KNps\n", npos, msecs.count() / 1'000'000.0, npos / 1000.0 / (msecs.count() / 1'000'000.0));
    printf("check test: %1.3f eval: %1.3f gen: %1.3f gen2: %1.3f \n", m_totalCheckTestMicroseconds / 1'000'000.0, 
                m_totalEvaluateMicroseconds / 1'000'000.0, m_totalGenerateMoveMicroseconds / 1'000'000.0, 
                m_totalGenLegalMicroseconds / 1'000'000.0); 

    m_totalCheckTestMicroseconds    = 0;
    m_totalGenerateMoveMicroseconds = 0;
    m_totalEvaluateMicroseconds     = 0;
    m_totalGenLegalMicroseconds     = 0;

    printf("Max score: %f\n", maxScore);
    printf("Best move is: %d,%d->%d,%d\n", m.x1, m.y1, m.x2, m.y2);

    x1 = m.x1;
    x2 = m.x2;
    y1 = m.y1;
    y2 = m.y2;

}

void Chess::computeBlockersAndBeyond()
{
/*        std::uint64_t m_pieceMoves[6][64];
        std::uint64_t m_arrBlockersAndBeyond[6][64];
        std::uint64_t m_arrBehind[64][64];
*/

    // Compute piece moves and occupancy mask

    // Pawns - set to zero
    for (int sq = 0; sq < 64; sq++)
    {
        m_pieceMoves[PIECE_PAWN][sq] = 0;
        m_arrBlockersAndBeyond[PIECE_PAWN][sq] = 0;
    }

    // Now compute specialised pawn arrays

    for (int sq = 0; sq < 64; sq++)
    {
        int x1 = sq & 7;
        int y1 = sq >> 3;

        m_pawnMovesWhite[sq] = 0;
        m_pawnAttacksWhite[sq] = 0;

        if (y1 == EIGHTH_RANK) break;

        m_pawnMovesWhite[sq] |= COORD_TO_BIT(x1, y1 + 1);

        if (IS_IN_BOARD(x1 - 1, y1 + 1))
            m_pawnAttacksWhite[sq] |= COORD_TO_BIT(x1 - 1, y1 + 1);

        if (IS_IN_BOARD(x1 + 1, y1 + 1))
            m_pawnAttacksWhite[sq] |= COORD_TO_BIT(x1 + 1, y1 + 1);

        if (y1 != SECOND_RANK) continue;

        m_pawnMovesWhite[sq] |= COORD_TO_BIT(x1, y1 + 2);

    }

    for (int sq = 63; sq >= 0; sq--)
    {
        int x1 = sq & 7;
        int y1 = sq >> 3;

        m_pawnMovesBlack[sq] = 0;
        m_pawnAttacksBlack[sq] = 0;

        if (y1 == FIRST_RANK) break;

        m_pawnMovesBlack[sq] |= COORD_TO_BIT(x1, y1 - 1);

        if (IS_IN_BOARD(x1 - 1, y1 - 1))
            m_pawnAttacksBlack[sq] |= COORD_TO_BIT(x1 - 1, y1 - 1);

        if (IS_IN_BOARD(x1 + 1, y1 - 1))
            m_pawnAttacksBlack[sq] |= COORD_TO_BIT(x1 + 1, y1 - 1);

        if (y1 != SEVENTH_RANK) continue;

        m_pawnMovesBlack[sq] |= COORD_TO_BIT(x1, y1 - 2);

    }
 

    //std::uint64_t m_pawnMovesBlack[64];
    //std::uint64_t m_pawnAttacksBlack[64];


    // Knights

    for (int sq = 0; sq < 64; sq++)
    {
        int x1 = sq & 7;
        int y1 = sq >> 3;

        m_pieceMoves[PIECE_KNIGHT][sq] = 0;
        m_arrBlockersAndBeyond[PIECE_KNIGHT][sq] = 0;
        
        for (const auto& m : knightMoves)
        {

            int x2 = x1 + m.first;
            int y2 = y1 + m.second;
            
            if (!IS_IN_BOARD(x2, y2)) continue;

            m_pieceMoves[PIECE_KNIGHT][sq] |= COORD_TO_BIT(x2, y2);
        }

    }

    // Bishops

    for (int sq = 0; sq < 64; sq++)
    {
        int x1 = sq & 7;
        int y1 = sq >> 3;

        m_pieceMoves[PIECE_BISHOP][sq] = 0;
        m_arrBlockersAndBeyond[PIECE_BISHOP][sq] = 0;

        for (const auto& m : bishopMoves)
        {

            for (int i = 1; i < 8; i++)
            {
                int x2 = x1 + i*m.first;
                int y2 = y1 + i*m.second;
                int x3 = x1 + (i+1)*m.first;
                int y3 = y1 + (i+1)*m.second;

                if (IS_IN_BOARD(x3, y3)) m_arrBlockersAndBeyond[PIECE_BISHOP][sq] 
                        |= COORD_TO_BIT(x2, y2); 
                if (!IS_IN_BOARD(x2, y2)) break;

                m_pieceMoves[PIECE_BISHOP][sq] |= COORD_TO_BIT(x2, y2);
            }
        }

    }

    // Rooks

    for (int sq = 0; sq < 64; sq++)
    {
        int x1 = sq & 7;
        int y1 = sq >> 3;

        m_pieceMoves[PIECE_ROOK][sq] = 0;
        m_arrBlockersAndBeyond[PIECE_ROOK][sq] = 0;

        for (const auto& m : rookMoves)
        {

            for (int i = 1; i < 8; i++)
            {
                int x2 = x1 + i*m.first;
                int y2 = y1 + i*m.second;
                int x3 = x1 + (i+1)*m.first;
                int y3 = y1 + (i+1)*m.second;

                if (IS_IN_BOARD(x3, y3)) m_arrBlockersAndBeyond[PIECE_ROOK][sq] 
                        |= COORD_TO_BIT(x2, y2); 
 
                if (!IS_IN_BOARD(x2, y2)) break;

                m_pieceMoves[PIECE_ROOK][sq] |= COORD_TO_BIT(x2, y2);
            }
        }

    }

  // Queens

    for (int sq = 0; sq < 64; sq++)
    {
        int x1 = sq & 7;
        int y1 = sq >> 3;

        m_pieceMoves[PIECE_QUEEN][sq] = 0;
        m_arrBlockersAndBeyond[PIECE_QUEEN][sq] = 0;

        for (const auto& m : queenMoves)
        {

            for (int i = 1; i < 8; i++)
            {
                int x2 = x1 + i*m.first;
                int y2 = y1 + i*m.second;
                int x3 = x1 + (i+1)*m.first;
                int y3 = y1 + (i+1)*m.second;

                if (IS_IN_BOARD(x3, y3)) m_arrBlockersAndBeyond[PIECE_QUEEN][sq] 
                        |= COORD_TO_BIT(x2, y2); 
 
                if (!IS_IN_BOARD(x2, y2)) break;

                m_pieceMoves[PIECE_QUEEN][sq] |= COORD_TO_BIT(x2, y2);
            }
        }

    }

    // Kings
    for (int sq = 0; sq < 64; sq++)
    {
        int x1 = sq & 7;
        int y1 = sq >> 3;

        m_pieceMoves[PIECE_KING][sq] = 0;
        m_arrBlockersAndBeyond[PIECE_KING][sq] = 0;
        
        for (const auto& m : kingMoves)
        {

            int x2 = x1 + m.first;
            int y2 = y1 + m.second;
            
            if (!IS_IN_BOARD(x2, y2)) continue;

            m_pieceMoves[PIECE_KING][sq] |= COORD_TO_BIT(x2, y2);
        }

    }

    // Compute behind table

    for (int sq1 = 0; sq1 < 64; sq1++)
    {
        int x1 = sq1 & 7;
        int y1 = sq1 >> 3;

        for (int sq2 = 0; sq2 < 64; sq2++)
        {

            int x2 = sq2 & 7;
            int y2 = sq2 >> 3;

            // We are standing on sq1. Fire a ray toward sq2, if we can. compute squares behind sq2.
            
            int dx = x2 - x1;
            int dy = y2 - y1;

            if ((dx > 0) && (dy > 0) && (dx == dy))
            {
                // Gradient = (1, 1)
                dx = 1;
                dy = 1;
            }
            else if ((dx < 0) && (dy > 0) && (-dx == dy))
            {
                // Gradient = (-1, 1)
                dx = -1;
                dy = 1;
            }
            else if ((dx < 0) && (dy < 0) && (dx == dy))
            {
                // Gradient = (-1, -1)
                dx = -1;
                dy = -1;
            }
            else if ((dx > 0) && (dy < 0) && (dx == -dy))
            {
                // Gradient = (1, -1)
                dx = 1;
                dy = -1;
            }
            else if (dx == 0)
            {
                dy = (dy > 0) ? 1 : -1;
            }
            else if (dy == 0)
            {
                dx = (dx > 0) ? 1 : -1;
            }
            else
            {
                dx = 0;
                dy = 0;
            }

            m_arrBehind[sq1][sq2] = 0;
            
            if (dx || dy)
            {
                do {
                    x2 += dx;
                    y2 += dy;

                    if (IS_IN_BOARD(x2, y2))
                        m_arrBehind[sq1][sq2] |= COORD_TO_BIT(x2, y2);
                } while (IS_IN_BOARD(x2, y2));
            }
        }
    }

}

double Chess::minimaxAlphaBetaFaster(ChessBoard& board, bool white, ChessMove& move, bool maximizing, int depth, uint64_t& npos, double alpha, double beta)
{
    bool isRoot = npos == 0;

    if (depth == 0)
    {
        double whiteScore = 0.0;
        double blackScore = 0.0;

        evalBoardFaster(board, whiteScore, blackScore);

        npos ++;

        if (white)
        {
            return whiteScore - blackScore;
        }
        else 
        {
            return blackScore - whiteScore;
        }
    }

    if (maximizing)
    {
        bool betaCutoff = false;

        generateMovesFast(board,
                [&] (ChessBoard& b, uint64_t from, uint64_t to) 
                { 
                    ChessMove mm;            
    
                    double newscore = minimaxAlphaBetaFaster(b, white, mm, false, depth - 1, npos, alpha, beta); 
                    
                    if (newscore >= beta)
                    {    
                        betaCutoff = true;
                        return true; 
                    }
                    if (newscore > alpha)
                    {
                        alpha = newscore;

                        if (isRoot)
                            moveFromBitboards(move, from, to); 
                    }

                    return false;
                });

        if (betaCutoff) 
        {
            return beta;
        }
        return alpha;
    }
    else
    {
        bool alphaCutoff = false;

        generateMovesFast(board, 
                [&] (ChessBoard& b, uint64_t from, uint64_t to) 
                { 
                    ChessMove mm;            
    
                    double newscore = minimaxAlphaBetaFaster(b, white, mm, true, depth - 1, npos, alpha, beta); 
                    if (newscore <= alpha)
                    {
                        alphaCutoff = true;
                        return true;
                    }
                    if (newscore < beta)
                    {
                        beta = newscore;
                        if (isRoot) // root
                            moveFromBitboards(move, from, to); 
                    }
                    
                    return false; 
                });
        if (alphaCutoff) 
        {
            return alpha;
        }
        return beta;
    }

    return 0.0;
}

void Chess::generateMovesFast(ChessBoard& board, std::function<bool (ChessBoard& b, uint64_t from_bb, uint64_t to_bb)> func)
{

    uint64_t myPieces  = 0;
    uint64_t oppPieces = 0;
    uint64_t allPieces = 0;
    uint64_t *myPawnAttacks;
    uint64_t *myPawnMoves;
    uint64_t enPassentSq;
    uint64_t kingMoveSquares = 0;
        
    if (board.m_isWhitesTurn)
    {
        myPieces     = board.allWhitePieces();
        oppPieces    = board.allBlackPieces();
        allPieces    = board.allWhitePieces() | board.allBlackPieces();
        myPawnMoves     = m_pawnMovesWhite;
        myPawnAttacks   = m_pawnAttacksWhite;
        if (board.m_can_en_passant_file != INVALID_FILE)
            enPassentSq     = COORD_TO_BIT(board.m_can_en_passant_file, SIXTH_RANK);
        else
            enPassentSq     = 0;
    }
    else
    {
        myPieces     = board.allBlackPieces();
        oppPieces    = board.allWhitePieces();
        allPieces    = board.allWhitePieces() | board.allBlackPieces();
        myPawnMoves     = m_pawnMovesBlack;
        myPawnAttacks   = m_pawnAttacksBlack;

        if (board.m_can_en_passant_file != INVALID_FILE)
            enPassentSq     = COORD_TO_BIT(board.m_can_en_passant_file, THIRD_RANK);
        else
            enPassentSq     = 0;
 
    }

    // Pawn moves
    for (uint64_t bb = *board.myPawns(); bb != 0; bb &= bb - 1)
    {
        uint64_t pawn = bb & -bb;
        int pawnSq = bitScanForward(pawn);

        uint64_t m = myPawnMoves[pawnSq];
        for (uint64_t mb = m & allPieces; mb != 0; mb &= (mb - 1))
        {
            int sq = bitScanForward(mb);
            m &= ~m_arrBehind[pawnSq][sq];
        }
      
        m &= ~allPieces;

        for (; m != 0; m &= (m-1))
        {
            uint64_t mm = m & -m;
            ChessBoard newb(board);

            int newPawnSq = bitScanForward(mm);

            if (abs(newPawnSq - pawnSq) > 8) newb.m_can_en_passant_file = pawn & 7;
            else newb.m_can_en_passant_file = INVALID_FILE;

            *newb.myPawns() = (*newb.myPawns() & ~pawn) | mm;
            newb.nextTurn();

            if (func(newb, pawn, mm)) goto done;

        }

        // Pawn captures
        m = myPawnAttacks[pawnSq];

        m &= (oppPieces | enPassentSq);

        for (; m != 0; m &= (m-1))
        {
            uint64_t mm = m & -m;

            ChessBoard newb(board);

            *newb.myPawns() = (*newb.myPawns() & ~pawn) | mm;
            newb.clearOppPieces(mm);
             
            newb.nextTurn();

            if (func(newb, pawn, mm)) goto done;
        }


    }    


    // Knight moves

    for (uint64_t bb = *board.myKnights(); bb != 0; bb &= bb - 1)
    {
        uint64_t knight = bb & -bb;
        int knightSq = bitScanForward(knight);
        uint64_t moves = m_pieceMoves[PIECE_KNIGHT][knightSq];

        moves &= ~myPieces;

        for (; moves != 0; moves &= moves - 1)
        {
            uint64_t mm = moves & -moves;

            ChessBoard newb(board);

            *newb.myKnights() = (*newb.myKnights() & ~knight) | mm;
            newb.clearOppPieces(mm);
             
            newb.nextTurn();

            if (func(newb, knight, mm)) goto done;

        }

    }
    
    // Bishop moves

    for (uint64_t bb = *board.myBishops(); bb != 0; bb &= bb - 1)
    {
        uint64_t bishop = bb & -bb;
        int bishopSq = bitScanForward(bishop);
        uint64_t moves = pieceAttacks(PIECE_BISHOP, bishopSq, allPieces);

        moves &= ~myPieces;

        for (; moves != 0; moves &= moves - 1)
        {
            uint64_t mm = moves & -moves;

            ChessBoard newb(board);

            *newb.myBishops() = (*newb.myBishops() & ~bishop) | mm;
            newb.clearOppPieces(mm);
             
            newb.nextTurn();

            if (func(newb, bishop, mm)) goto done;

        }

    }

    // Rook moves

    for (uint64_t bb = *board.myRooks(); bb != 0; bb &= bb - 1)
    {
        uint64_t rook = bb & -bb;
        int rookSq = bitScanForward(rook);
        uint64_t moves = pieceAttacks(PIECE_ROOK, rookSq, allPieces);

        moves &= ~myPieces;

        for (; moves != 0; moves &= moves - 1)
        {
            uint64_t mm = moves & -moves;

            ChessBoard newb(board);

            *newb.myRooks() = (*newb.myRooks() & ~rook) | mm;
            newb.clearOppPieces(mm);
            if ((rookSq & 7) == A_FILE) *newb.myARookHasMoved() = true;
            else if ((rookSq & 7) == H_FILE) *newb.myHRookHasMoved() = true;
             
            newb.nextTurn();


            if (func(newb, rook, mm)) goto done;

        }

    }

    // Queen moves

    for (uint64_t bb = *board.myQueens(); bb != 0; bb &= bb - 1)
    {
        uint64_t queen = bb & -bb;
        int queenSq = bitScanForward(queen);
        uint64_t moves = pieceAttacks(PIECE_QUEEN, queenSq, allPieces);

        moves &= ~myPieces;

        for (; moves != 0; moves &= moves - 1)
        {
            uint64_t mm = moves & -moves;

            ChessBoard newb(board);

            *newb.myQueens() = (*newb.myQueens() & ~queen) | mm;
            newb.clearOppPieces(mm);
             
            newb.nextTurn();

            if (func(newb, queen, mm)) goto done;

        }

    }

   // King moves

    for (uint64_t bb = *board.myKings(); bb != 0; bb &= bb - 1)
    {
        uint64_t king = bb & -bb;
        int kingSq = bitScanForward(king);
        uint64_t moves = m_pieceMoves[PIECE_KING][kingSq];

        moves &= ~myPieces;

        for (; moves != 0; moves &= moves - 1)
        {
            uint64_t mm = moves & -moves;

            kingMoveSquares |= mm;

            ChessBoard newb(board);

            *newb.myKings() = (*newb.myKings() & ~king) | mm;
            newb.clearOppPieces(mm);
            *newb.myKingHasMoved() = true;
            newb.nextTurn();

            if (func(newb, king, mm)) goto done;

        }

    }

    // Castling

    if ((board.m_isWhitesTurn) && (!board.m_whiteKingHasMoved) && !kingIsInCheck(board, true))
    {

        // Check King side castling
        if ((getPieceForSquare(board, F_FILE, FIRST_RANK) == NO_PIECE) &&
            (getPieceForSquare(board, G_FILE, FIRST_RANK) == NO_PIECE) &&
            !board.m_whiteHRookHasMoved)
        {
            if (!movePutsPlayerInCheck(board, E_FILE, FIRST_RANK, F_FILE, FIRST_RANK, true) && !movePutsPlayerInCheck(board, E_FILE, FIRST_RANK, G_FILE, FIRST_RANK, true))
                kingMoveSquares |= COORD_TO_BIT(G_FILE, FIRST_RANK); 
        }

        // Check Queen side castling
        if ((getPieceForSquare(board, D_FILE, FIRST_RANK) == NO_PIECE) &&
            (getPieceForSquare(board, C_FILE, FIRST_RANK) == NO_PIECE) &&
            (getPieceForSquare(board, B_FILE, FIRST_RANK) == NO_PIECE) &&
            !board.m_whiteARookHasMoved)
        {
            if (!movePutsPlayerInCheck(board, E_FILE, FIRST_RANK, D_FILE, FIRST_RANK, true) && !movePutsPlayerInCheck(board, E_FILE, FIRST_RANK, C_FILE, FIRST_RANK, true))
                kingMoveSquares |= COORD_TO_BIT(C_FILE, FIRST_RANK);
        }

    }
    else if ((!board.m_isWhitesTurn) && (!board.m_blackKingHasMoved) && !kingIsInCheck(board, false))
    {

        // Check King side castling
        if ((getPieceForSquare(board, F_FILE, EIGHTH_RANK) == NO_PIECE) &&
            (getPieceForSquare(board, G_FILE, EIGHTH_RANK) == NO_PIECE) &&
            !board.m_blackHRookHasMoved)
        {
            if (!movePutsPlayerInCheck(board, E_FILE, EIGHTH_RANK, G_FILE, EIGHTH_RANK, false) && !movePutsPlayerInCheck(board, E_FILE, EIGHTH_RANK, G_FILE, EIGHTH_RANK, false))
                kingMoveSquares |= COORD_TO_BIT(G_FILE, EIGHTH_RANK); 
        }

        // Check Queen side castling
        if ((getPieceForSquare(board, D_FILE, EIGHTH_RANK) == NO_PIECE) &&
            (getPieceForSquare(board, C_FILE, EIGHTH_RANK) == NO_PIECE) &&
            (getPieceForSquare(board, B_FILE, EIGHTH_RANK) == NO_PIECE) &&
            !board.m_blackARookHasMoved)
        {
            if (!movePutsPlayerInCheck(board, E_FILE, EIGHTH_RANK, D_FILE, EIGHTH_RANK, false) && !movePutsPlayerInCheck(board, E_FILE, EIGHTH_RANK, C_FILE, EIGHTH_RANK, false))
                kingMoveSquares |= COORD_TO_BIT(C_FILE, EIGHTH_RANK); 
        }

    }

    if (kingMoveSquares)
    {
        for (uint64_t bb = *board.myKings(); bb != 0; bb &= bb - 1)
        {
            uint64_t king = bb & -bb;

            for (; kingMoveSquares != 0; kingMoveSquares &= kingMoveSquares - 1)
            {
                uint64_t mm = kingMoveSquares & -kingMoveSquares;

                ChessBoard newb(board);

                *newb.myKings() = (*newb.myKings() & ~king) | mm;
                if (mm == COORD_TO_BIT(G_FILE, FIRST_RANK)) *newb.myRooks() = (*newb.myRooks() & ~COORD_TO_BIT(H_FILE, FIRST_RANK)) | COORD_TO_BIT(F_FILE, FIRST_RANK);
                else if (mm == COORD_TO_BIT(C_FILE, FIRST_RANK)) *newb.myRooks() = (*newb.myRooks() & ~COORD_TO_BIT(A_FILE, FIRST_RANK)) | COORD_TO_BIT(D_FILE, FIRST_RANK);
                else if (mm == COORD_TO_BIT(G_FILE, EIGHTH_RANK)) *newb.myRooks() = (*newb.myRooks() & ~COORD_TO_BIT(H_FILE, EIGHTH_RANK)) | COORD_TO_BIT(F_FILE, EIGHTH_RANK);
                else if (mm == COORD_TO_BIT(C_FILE, EIGHTH_RANK)) *newb.myRooks() = (*newb.myRooks() & ~COORD_TO_BIT(A_FILE, EIGHTH_RANK)) | COORD_TO_BIT(D_FILE, EIGHTH_RANK);


                *newb.myKingHasMoved() = true;
                newb.nextTurn();

                if (func(newb, king, mm)) goto done;

            }
        }
    }

done:
    return;
}

void Chess::evalBoardFaster(const ChessBoard& board, double& white_score, double& black_score)
{

    white_score = 0.0;
    black_score = 0.0;

    // Compute material
    white_score += sum_bits_and_multiply(board.whitePawnsBoard, 1.0);
    white_score += sum_bits_and_multiply(board.whiteKnightsBoard, 3.0);
    white_score += sum_bits_and_multiply(board.whiteBishopsBoard, 3.0);
    white_score += sum_bits_and_multiply(board.whiteRooksBoard, 5.0);
    white_score += sum_bits_and_multiply(board.whiteQueensBoard, 9.0);
    white_score += sum_bits_and_multiply(board.whiteKingsBoard, 900.0);


    black_score += sum_bits_and_multiply(board.blackPawnsBoard, 1.0);
    black_score += sum_bits_and_multiply(board.blackKnightsBoard, 3.0);
    black_score += sum_bits_and_multiply(board.blackBishopsBoard, 3.0);
    black_score += sum_bits_and_multiply(board.blackRooksBoard, 5.0);
    black_score += sum_bits_and_multiply(board.blackQueensBoard, 9.0);
    black_score += sum_bits_and_multiply(board.blackKingsBoard, 900.0);

    // Compute positional score

    constexpr double pos_mult = 0.01;

    white_score += pos_mult*multiply_bits_with_weights_reverse(board.whitePawnsBoard,   pawnPositionWeights);
    white_score += pos_mult*multiply_bits_with_weights_reverse(board.whiteKnightsBoard, knightPositionWeights);
    white_score += pos_mult*multiply_bits_with_weights_reverse(board.whiteBishopsBoard, bishopsPositionWeights);
    white_score += pos_mult*multiply_bits_with_weights_reverse(board.whiteRooksBoard,   rooksPositionWeights);
    white_score += pos_mult*multiply_bits_with_weights_reverse(board.whiteQueensBoard,   queenPositionWeights);
    white_score += pos_mult*multiply_bits_with_weights_reverse(board.whiteKingsBoard,   kingPositionWeights);

    black_score += pos_mult*multiply_bits_with_weights(board.blackPawnsBoard, pawnPositionWeights);
    black_score += pos_mult*multiply_bits_with_weights(board.blackKnightsBoard, knightPositionWeights);
    black_score += pos_mult*multiply_bits_with_weights(board.blackBishopsBoard, bishopsPositionWeights);
    black_score += pos_mult*multiply_bits_with_weights(board.blackRooksBoard, rooksPositionWeights);
    black_score += pos_mult*multiply_bits_with_weights(board.blackQueensBoard, queenPositionWeights);
    black_score += pos_mult*multiply_bits_with_weights(board.blackKingsBoard,  kingPositionWeights);

}



