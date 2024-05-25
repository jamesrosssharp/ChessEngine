/* vim: set et ts=4 sw=4: */

/*
    ChessEngine : A chess engine written in C++ for SDL2

Blockers.cpp: Compute blockers and beyond lookup tables

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

#include <Blockers.h>
#include <vector>

static const std::vector<std::pair<int, int>> knightMoves  =     {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};
static const std::vector<std::pair<int, int>> pawnCaptures =     {{-1, 1}, {1, 1}};
static const std::vector<std::pair<int, int>> queenMoves   =     {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
static const std::vector<std::pair<int, int>> kingMoves    =     {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
static const std::vector<std::pair<int, int>> bishopMoves  =     {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
static const std::vector<std::pair<int, int>> rookMoves    =     {{1, 0}, {0, 1},  {-1, 0}, {0, -1}};

Blockers::Blockers()
{
    computeBlockersAndBeyond();
}

void Blockers::computeBlockersAndBeyond()
{
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
