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

const double whitePawnPositionWeights[64] = {
/*a1 -> h1 */ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
/*a2 -> h2 */ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
/*a3 -> h3 */ 0.0, 0.0, 0.1, 0.1, 0.1, 0.1, 0.0, 0.0,
/*a4 -> h4 */ 0.0, 0.0, 0.0, 0.2, 0.2, 0.0, 0.0, 0.0,
/*a5 -> h5 */ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
/*a6 -> h6 */ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
/*a7 -> h7 */ 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
/*a8 -> h8 */ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
};



Chess::Chess()  
{
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

    if (!allowTakeKing && board.m_isWhitesTurn && !(piece & WHITE_PIECES)) return;
    if (!allowTakeKing && !board.m_isWhitesTurn && !(piece & BLACK_PIECES)) return;

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
                int xx = x;
                int yy = y + i*multiplier;

                // Check if blocked by a piece

                if (getPieceForSquare(board, x, yy) != NO_PIECE) break;

                // We can only move 2 squares on first move.

                if ((piece == WHITE_PAWN) && (i == 2) && (y != SECOND_RANK)) break;
                if ((piece == BLACK_PAWN) && (i == 2) && (y != SEVENTH_RANK)) break;

                // Cannot make move if it puts the player in check
                if (!allowTakeKing && movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

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

                if (!allowTakeKing)
                {
                    // Can never take the king
                    if ((piece == WHITE_PAWN) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                    if ((piece == BLACK_PAWN) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;
                }

                // Cannot make move if it puts the player in check
                if (!allowTakeKing && movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

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
                if (!allowTakeKing && movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

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

               if ((xx >= A_FILE) && (xx <= H_FILE) && (yy >= FIRST_RANK) && (yy < EIGHTH_RANK))
               {
                    if ((piece == WHITE_KNIGHT) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) continue;
                    if ((piece == BLACK_KNIGHT) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) continue;

                    // Can never take the king
                    if (!allowTakeKing)
                    {
                        if ((piece == WHITE_KNIGHT) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                        if ((piece == BLACK_KNIGHT) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;
                    }

                    // Cannot make move if it puts the player in check
                    if (!allowTakeKing && movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

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
                        if ((piece == WHITE_BISHOP) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;
                        if ((piece == BLACK_BISHOP) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;

                        if (!allowTakeKing)
                        {
                            if ((piece == WHITE_BISHOP) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                            if ((piece == BLACK_BISHOP) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;
                        }

                        // Cannot make move if it puts the player in check
                        if (allowTakeKing ||  !movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) 
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

                   if ((xx >= A_FILE) && (xx <= H_FILE) && (yy >= FIRST_RANK) && (yy <= EIGHTH_RANK))
                   {
                        if ((piece == WHITE_ROOK) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;
                        if ((piece == BLACK_ROOK) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;

                        if (!allowTakeKing)
                        {
                            if ((piece == WHITE_ROOK) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                            if ((piece == BLACK_ROOK) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;
                        }

                        // Cannot make move if it puts the player in check
                        if (allowTakeKing || !movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) 
                            moveSquares |= COORD_TO_BIT(xx, yy); 
                  
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
                        if ((piece == WHITE_QUEEN) && (getPieceForSquare(board, xx, yy) & WHITE_PIECES)) break;
                        if ((piece == BLACK_QUEEN) && (getPieceForSquare(board, xx, yy) & BLACK_PIECES)) break;

                        if (!allowTakeKing)
                        {
                            if ((piece == WHITE_QUEEN) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) break;
                            if ((piece == BLACK_QUEEN) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) break;
                        }

                        // Cannot make move if it puts the player in check
                        if (allowTakeKing || !movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES))
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

                    if (!allowTakeKing)
                    {
                        if ((piece == WHITE_KING) && (getPieceForSquare(board, xx, yy) == BLACK_KING)) continue;
                        if ((piece == BLACK_KING) && (getPieceForSquare(board, xx, yy) == WHITE_KING)) continue;
                    }

                    // Cannot make move if it puts the player in check
                    if (!allowTakeKing && movePutsPlayerInCheck(board, x, y, xx, yy, piece & WHITE_PIECES)) continue; 

                    moveSquares |= COORD_TO_BIT(xx, yy);
              
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
                    if ((moveSquares & COORD_TO_BIT(F_FILE, FIRST_RANK)) && (allowTakeKing || !(movePutsPlayerInCheck(board, x, y, G_FILE, FIRST_RANK, true))))
                        moveSquares |= COORD_TO_BIT(G_FILE, FIRST_RANK); 
                }

                // Check Queen side castling
                if ((getPieceForSquare(board, D_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, C_FILE, FIRST_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, B_FILE, FIRST_RANK) == NO_PIECE) &&
                    !board.m_whiteARookHasMoved)
                {
                    if ((moveSquares & COORD_TO_BIT(D_FILE, FIRST_RANK)) && (allowTakeKing || !(movePutsPlayerInCheck(board, x, y, C_FILE, FIRST_RANK, true))))
                        moveSquares |= COORD_TO_BIT(C_FILE, FIRST_RANK);
                }

            }

            if ((piece == BLACK_KING) && (!board.m_blackKingHasMoved))
            {

                // Check King side castling
                if ((getPieceForSquare(board, F_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, G_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    !board.m_blackHRookHasMoved)
                {
                    if ((moveSquares & COORD_TO_BIT(F_FILE, EIGHTH_RANK)) && (allowTakeKing || !(movePutsPlayerInCheck(board, x, y, G_FILE, EIGHTH_RANK, false))))
                        moveSquares |= COORD_TO_BIT(G_FILE, EIGHTH_RANK); 
                }

                // Check Queen side castling
                if ((getPieceForSquare(board, D_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, C_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    (getPieceForSquare(board, B_FILE, EIGHTH_RANK) == NO_PIECE) &&
                    !board.m_blackARookHasMoved)
                {
                    if ((moveSquares & COORD_TO_BIT(D_FILE, EIGHTH_RANK)) && (allowTakeKing || !(movePutsPlayerInCheck(board, x, y, C_FILE, EIGHTH_RANK, false))))
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
    makeMoveForBoard(m_board, x1, y1, x2, y2, ep, castle_kings_side, castle_queens_side);
}


void Chess::makeMoveForBoard(ChessBoard& board, int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side, bool print)
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
    if (((start_piece == WHITE_PAWN) && (y1 == SECOND_RANK) && (y2 == THIRD_RANK)) ||
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
    
    if (print)
    {
        printBoard(board);
        if (kingIsInCheck(board, !board.m_isWhitesTurn))
        {
            if (movesForPlayer(board, !board.m_isWhitesTurn) == 0)
                printf("Checkmate!\n");
            else
                printf("Check!\n");
        }
    
        double w, b;

        evalBoard(board, w, b);
        printf("Scores: white = %f  black = %f\n", w, b);
    }

    board.m_isWhitesTurn = !board.m_isWhitesTurn;

}

bool Chess::kingIsInCheck(const ChessBoard& board, bool white)
{
    uint64_t legalMoves = 0;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            uint64_t temp = 0;
            enum PieceTypes type = getPieceForSquare(board, x, y);
            if ((!white && (type & WHITE_PIECES)) ||
                (white && (type & BLACK_PIECES)))
            {
                    getLegalMovesForBoardSquare(board, x, y, temp, true);
                    legalMoves |= temp;
            }
        }
    }

    // We could and should exit early here, but for now, we won't as we want to debug easier...
    //printBitBoard(legalMoves);
    if (white && (board.whiteKingsBoard & legalMoves)) return true;
    if (!white && (board.blackKingsBoard & legalMoves)) return true;

    return false;
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

static double sum_bits_and_multiply(uint64_t bb, double multiplier)
{
    int sum = 0;
    for (int i = 0; i < 64; i ++)
        if (bb & (1ULL << i))
            sum++;
    return sum * multiplier;
}

static double multiply_bits_with_weights(uint64_t bb, const double* weights)
{
    int sum = 0;
    for (int i = 0; i < 64; i ++)
        if (bb & (1ULL << i))
            sum += weights[i];
    return sum;
}

static double multiply_bits_with_weights_reverse(uint64_t bb, const double* weights)
{
    int sum = 0;
    for (int x = 0; x < 8; x ++)
        for (int y = 0; y < 8; y++)
            if (bb & (1ULL << (x + y*8)))
                sum += weights[x + (7-y)*8 ];
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

    white_score += multiply_bits_with_weights(board.whitePawnsBoard, whitePawnPositionWeights);

    black_score += sum_bits_and_multiply(board.blackPawnsBoard, 1.0);
    black_score += sum_bits_and_multiply(board.blackKnightsBoard, 3.0);
    black_score += sum_bits_and_multiply(board.blackBishopsBoard, 3.0);
    black_score += sum_bits_and_multiply(board.blackRooksBoard, 5.0);
    black_score += sum_bits_and_multiply(board.blackQueensBoard, 9.0);

    black_score += multiply_bits_with_weights_reverse(board.blackPawnsBoard, whitePawnPositionWeights);

    // Compute position


    // Compute checkmate

    if (kingIsInCheck(board, true) && (movesForPlayer(board, true) == 0))
    {
        black_score += 10000.0;
    }

    if (kingIsInCheck(board, false) && (movesForPlayer(board, false) == 0))
    {
        white_score += 10000.0;
    }

}

void Chess::getBestMove(int& x1, int& y1, int& x2, int& y2)
{
    uint64_t bb = (m_board.m_isWhitesTurn) ? m_board.allWhitePieces() : m_board.allBlackPieces();

    double my_score = -10000.0;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            uint64_t sq = COORD_TO_BIT(x, y);

            if (bb & sq) 
            {
                uint64_t temp = 0;
                getLegalMovesForBoardSquare(m_board, x, y, temp);
            
                for (int xx = 0; xx < 8; xx++)
                {
        
                    for (int yy = 0; yy < 8; yy++)
                    {
                        // Compute score
           
                        if (temp & COORD_TO_BIT(xx, yy))
                        {

                            ChessBoard clone = m_board;
                            bool ep;
                            bool castle_kings_side;
                            bool castle_queens_side;

                            makeMoveForBoard(clone, x, y, xx, yy, ep, castle_kings_side, castle_queens_side, false);
                       
                            double w = 0.0;
                            double b = 0.0;

                            evalBoard(clone, w, b);

                            if (m_board.m_isWhitesTurn)
                            {
                                if ((w - b) > my_score)
                                {
                                    my_score = w - b;
                                    x1 = x;
                                    x2 = xx;
                                    y1 = y;
                                    y2 = yy;
                                }
                            }
                            else
                            {
                                if ((b - w) > my_score)
                                {
                                    my_score = b - w;
                                    x1 = x;
                                    x2 = xx;
                                    y1 = y;
                                    y2 = yy;
                                }

                            }
                        }
                    }
                }
            }
        }
    }

}
