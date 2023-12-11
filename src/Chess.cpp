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

const std::vector<std::pair<int, int>> knightMoves = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};
const std::vector<std::pair<int, int>> pawnCaptures = {{-1, 1}, {1, 1}};
const std::vector<std::pair<int, int>> queenMoves = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
const std::vector<std::pair<int, int>> kingMoves = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
const std::vector<std::pair<int, int>> bishopMoves = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
const std::vector<std::pair<int, int>> rookMoves = {{1, 0}, {0, 1},  {-1, 0}, {0, -1}};

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

    m_board.m_isWhitesTurn = true;

    m_board.m_can_en_passant_file = INVALID_FILE;
    m_board.m_whiteKingHasMoved = false;
    m_board.m_blackKingHasMoved = false;
    m_board.m_whiteARookHasMoved = false;
    m_board.m_whiteHRookHasMoved = false;
    m_board.m_blackARookHasMoved = false;
    m_board.m_blackHRookHasMoved = false;
}

void Chess::getLegalMovesForSquare(int x, int y, uint64_t &moveSquares, bool allowTakeKing)
{
    getLegalMovesForBoardSquare(m_board, x, y, moveSquares, allowTakeKing);
}

void Chess::getLegalMovesForBoardSquare(const ChessBoard& board, int x, int y, uint64_t &moveSquares, bool allowTakeKing)
{
    // Init: zero out legal moves

    moveSquares = 0;

    // First, find the piece which is on this square

    enum PieceTypes piece = getPieceForSquare(board, x, y);

    //std::cout << "Computing moves for " << prettyPiece(piece) << std::endl;

    // Check if it's this colors move

    if (board.m_isWhitesTurn && !(piece & WHITE_PIECES)) return;
    if (!board.m_isWhitesTurn && !(piece & BLACK_PIECES)) return;

    // TODO: Check for check! - cannot allow move if it results in check (or doesn't prevent an existing check)

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

                // Check if blocked by a piece

                if (getPieceForSquare(board, x, y + i*multiplier) != NO_PIECE) break;

                // We can only move 2 squares on first move.

                if ((piece == WHITE_PAWN) && (i == 2) && (y != 1)) break;
                if ((piece == BLACK_PAWN) && (i == 2) && (y != 6)) break;

                // If not, we can move here.

                moveSquares |= 1ULL << (x + (y + i*multiplier)*8);   
            }

            // Simple capturing
       
            for (auto p : pawnCaptures)
            {
                int xx = x + p.first;
                int yy = y + p.second*multiplier;

                if (xx < 0) continue;
                if (xx > 7) continue;
                if ((piece == WHITE_PAWN) && !(getPieceForSquare(board, xx, yy) & BLACK_PIECES)) continue;
                if ((piece == BLACK_PAWN) && !(getPieceForSquare(board, xx, yy) & WHITE_PIECES)) continue;

                if (!allowTakeKing)
                {
                    // Can never take the king
                    if ((piece == WHITE_PAWN) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                    if ((piece == BLACK_PAWN) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;
                }

                moveSquares |= 1ULL << (xx + yy*8);
            }

            // En passant

            for (auto p : pawnCaptures)
            {
                int xx = x + p.first;
                int yy = y + p.second*multiplier;
           
                if (xx < 0) continue;
                if (xx > 7) continue;
            
                if (! ( ((piece == WHITE_PAWN) && (board.m_can_en_passant_file == xx) && (getPieceForSquare(board, xx, yy - multiplier) & BLACK_PIECES)) ||
                        ((piece == BLACK_PAWN) && (board.m_can_en_passant_file == xx) && (getPieceForSquare(board, xx, yy - multiplier) & WHITE_PIECES)) )) continue;

                moveSquares |= 1ULL << (xx + yy*8);
            }

            break;
        }
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:

            for (auto p : knightMoves)
            {

               int xx = x + p.first;
               int yy = y + p.second;

               if ((xx >= 0) && (xx < 8) && (yy >= 0) && (yy < 8))
               {
                    if ((piece == WHITE_KNIGHT) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) continue;
                    if ((piece == BLACK_KNIGHT) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) continue;

                    // Can never take the king
                    if (!allowTakeKing)
                    {
                        if ((piece == WHITE_KNIGHT) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                        if ((piece == BLACK_KNIGHT) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;
                    }

                    moveSquares |= 1ULL << (xx + yy*8);
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

                   if ((xx >= 0) && (xx < 8) && (yy >= 0) && (yy < 8))
                   {
                        if ((piece == WHITE_BISHOP) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;
                        if ((piece == BLACK_BISHOP) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;

                        if (!allowTakeKing)
                        {
                            if ((piece == WHITE_BISHOP) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                            if ((piece == BLACK_BISHOP) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;
                        }

                        moveSquares |= 1ULL << (xx + yy*8);
                  
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

                   if ((xx >= 0) && (xx < 8) && (yy >= 0) && (yy < 8))
                   {
                        if ((piece == WHITE_ROOK) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;
                        if ((piece == BLACK_ROOK) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;

                        if (!allowTakeKing)
                        {
                            if ((piece == WHITE_ROOK) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                            if ((piece == BLACK_ROOK) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;
                        }

                        moveSquares |= 1ULL << (xx + yy*8);
                  
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

                   if ((xx >= 0) && (xx < 8) && (yy >= 0) && (yy < 8))
                   {
                        if ((piece == WHITE_QUEEN) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;
                        if ((piece == BLACK_QUEEN) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;

                        if (!allowTakeKing)
                        {
                            if ((piece == WHITE_QUEEN) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                            if ((piece == BLACK_QUEEN) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;
                        }

                        moveSquares |= 1ULL << (xx + yy*8);
                  
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

               if ((xx >= 0) && (xx < 8) && (yy >= 0) && (yy < 8))
               {
                    if ((piece == WHITE_KING) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) continue;
                    if ((piece == BLACK_KING) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) continue;

                    if (!allowTakeKing)
                    {
                        if ((piece == WHITE_KING) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                        if ((piece == BLACK_KING) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;
                    }

                    moveSquares |= 1ULL << (xx + yy*8);
              
               }

            }

            // Check for castling moves

            if ((piece == WHITE_KING) && (!board.m_whiteKingHasMoved))
            {

                // Check King side castling
                if ((getPieceForSquare(board, F_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, G_FILE, FIRST_RANK) == NO_PIECE) &&
                    !board.m_whiteHRookHasMoved)
                {
                    moveSquares |= 1ULL << (G_FILE);
                }

                // Check Queen side castling
                if ((getPieceForSquare(board, D_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, C_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, B_FILE, FIRST_RANK) == NO_PIECE) &&
                    !board.m_whiteARookHasMoved)
                {
                    moveSquares |= 1ULL << (C_FILE);
                }

            }

            if ((piece == BLACK_KING) && (!board.m_blackKingHasMoved))
            {

                // Check King side castling
                if ((getPieceForSquare(board, F_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, G_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    !board.m_blackHRookHasMoved)
                {
                    moveSquares |= 1ULL << (G_FILE + EIGHTH_RANK*8);
                }

                // Check Queen side castling
                if ((getPieceForSquare(board, D_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, C_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, B_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    !board.m_blackARookHasMoved)
                {
                    moveSquares |= 1ULL << (C_FILE + EIGHTH_RANK*8);
                }

            }
            break;
        default:
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

enum PieceTypes Chess::getPieceForSquare(const ChessBoard& board, int x, int y)
{
    uint64_t sq = 1ULL << (y*8 + x);

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
    uint64_t sq   = 1ULL << (x + y*8);

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

    uint64_t sq   = 1ULL << (x + y*8);

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
    makeMoveForBoard(m_board, x1, y1, x2, y2, ep, castle_kings_side, castle_queens_side);
}


void Chess::makeMoveForBoard(ChessBoard& board, int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side)
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
    if (((start_piece == WHITE_PAWN) && (y1 == 1) && (y2 == 3)) ||
        ((start_piece == BLACK_PAWN) && (y1 == 6) && (y2 == 4)))
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
        if (x1 == 0) board.m_whiteARookHasMoved = true;
        else if (x1 == 7) board.m_whiteHRookHasMoved = true;
    }
    else if (start_piece == BLACK_ROOK)
    {
        if (x1 == 0) board.m_blackARookHasMoved = true;
        else if (x1 == 7) board.m_blackHRookHasMoved = true;
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

    printBoard();

    if (kingIsInCheck(m_board, !board.m_isWhitesTurn)) printf("Check!\n");
    board.m_isWhitesTurn = !board.m_isWhitesTurn;

}

bool Chess::kingIsInCheck(const ChessBoard& board, bool white)
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            uint64_t temp = 0;
            enum PieceTypes type = getPieceForSquare(board, x, y);
            if ((!white && (type & WHITE_PIECES)) ||
                (white && (type & BLACK_PIECES)))
            {
                    getLegalMovesForSquare(x, y, temp, true);
                    if (white && (board.whiteKingsBoard & temp)) return true;
                    if (!white && (board.blackKingsBoard & temp)) return true;
            }
        }
    }

        return false;
}
