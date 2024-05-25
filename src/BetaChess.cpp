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

#include <BetaChess.h>

uint64_t BetaChess::perft(int depth)
{

    if (depth == 0) return 1ULL;

    struct BetaMove moves[512];

    int n = generate_moves(moves);
    int nodes = 0;

    for (int i = 0; i < n; i++)
    {
        m_board.makeMove(moves[i]);

            nodes += perft(depth - 1);

        m_board.unmakeMove(moves[i]);

    }

    return nodes;
}

int BetaChess::_generate_moves_white(struct BetaMove* moves)
{

    int n_moves = 0;
    uint64_t myPieces = *m_board.whitePieces();

    // Pawn moves
    for (uint64_t bb = m_board.whiteKings(); bb != 0; bb &= bb - 1)
    {
        uint64_t king   = bb & -bb;
        int kingSq      = bitScanForward(king);
        uint64_t mmoves  = m_blockers.m_pieceMoves[PIECE_KING][kingSq];

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B

            int toSq = bitScanForward(m); // Get to square   

            moves[n_moves].sq_from = kingSq;
            moves[n_moves].sq_to   = toSq;

            moves[n_moves].bitboard_color_from      = BITBOARD_WHITE_PIECES;   
            moves[n_moves].bitboard_color_to        = BITBOARD_WHITE_PIECES;     
            moves[n_moves].bitboard_color_capture   = BITBOARD_BLACK_PIECES;

            moves[n_moves].from_piece               = BITBOARD_KING;                
            moves[n_moves].to_piece                 = BITBOARD_KING;                 
            
            moves[n_moves].capture_piece            = (!!(m_board.blackPawns() & m) * BITBOARD_PAWN)     |     
                                                      (!!(m_board.blackKnights() & m) * BITBOARD_KNIGHT) |     
                                                      (!!(m_board.blackBishops() & m) * BITBOARD_BISHOP) |     
                                                      (!!(m_board.blackRooks() & m) * BITBOARD_ROOK)     |     
                                                      (!!(m_board.blackQueens() & m) * BITBOARD_QUEEN)   |     
                                                      (!!(m_board.blackKings() & m) * BITBOARD_KING);     

            moves[n_moves].flags                    = (moves[n_moves].capture_piece != 0) * IS_CAPTURE;      

            n_moves++;

        }

    }

    return n_moves;
}

int BetaChess::_generate_moves_black(struct BetaMove* moves)
{


    return 0;
}

