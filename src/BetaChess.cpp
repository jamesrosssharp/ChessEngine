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

    struct BetaMove moves[256];

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

#define MAKE_MOVE_WHITE(from, to, pieceFrom, pieceTo)   moves[n_moves].sq_from = from;    \
                                    moves[n_moves].sq_to   = to;      \
                                                                        \
                                    moves[n_moves].bitboard_color_from      = BITBOARD_WHITE_PIECES;    \
                                    moves[n_moves].bitboard_color_to        = BITBOARD_WHITE_PIECES;    \
                                    moves[n_moves].bitboard_color_capture   = BITBOARD_BLACK_PIECES;    \
                                                                                                        \
                                    moves[n_moves].from_piece               = pieceFrom;            \
                                    moves[n_moves].to_piece                 = pieceTo;            \
                                                                                                        \
                                    moves[n_moves].capture_piece            = (!!(m_board.blackPawns() & m) * BITBOARD_PAWN)     |     \
                                                                              (!!(m_board.blackKnights() & m) * BITBOARD_KNIGHT) |     \
                                                                              (!!(m_board.blackBishops() & m) * BITBOARD_BISHOP) |     \
                                                                              (!!(m_board.blackRooks() & m) * BITBOARD_ROOK)     |     \
                                                                              (!!(m_board.blackQueens() & m) * BITBOARD_QUEEN)   |     \
                                                                              (!!(m_board.blackKings() & m) * BITBOARD_KING);          \
                                                                                                                                       \
                                    moves[n_moves].flags                    = (moves[n_moves].capture_piece != 0) * IS_CAPTURE;



int BetaChess::_generate_moves_white(struct BetaMove* moves)
{

    int n_moves = 0;
    uint64_t myPieces = *m_board.whitePieces();
    uint64_t theirPieces = *m_board.blackPieces();
    uint64_t allPieces = myPieces | theirPieces;

    // King moves
    for (uint64_t bb = m_board.whiteKings(); bb != 0; bb &= bb - 1)
    {
        uint64_t king   = bb & -bb;
        int kingSq      = bitScanForward(king);
        uint64_t mmoves  = m_blockers.m_pieceMoves[PIECE_KING][kingSq];

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_WHITE(kingSq, toSq, BITBOARD_KING, BITBOARD_KING);

            n_moves++;

        }

    }

    // Pawn moves
    for (uint64_t bb = m_board.whitePawns(); bb != 0; bb &= bb - 1)
    {
    
        uint64_t pawn   = bb & -bb;
        int pawnSq      = bitScanForward(pawn);
        uint64_t mmoves  = m_blockers.m_pawnMovesWhite[pawnSq];

        mmoves &= ~(myPieces | theirPieces);

        uint64_t captures = m_blockers.m_pawnAttacksWhite[pawnSq];

        captures &= theirPieces;

        mmoves |= captures;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_WHITE(pawnSq, toSq, BITBOARD_PAWN, BITBOARD_PAWN);
            
            n_moves++;

        }
    }

    // Knight moves

    for (uint64_t bb = m_board.whiteKnights(); bb != 0; bb &= bb - 1)
    {
        uint64_t knight   = bb & -bb;
        int knightSq      = bitScanForward(knight);
        uint64_t mmoves  = m_blockers.m_pieceMoves[PIECE_KNIGHT][knightSq];

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_WHITE(knightSq, toSq, BITBOARD_KNIGHT, BITBOARD_KNIGHT);

            n_moves++;

        }

    }

    // Bishop moves 

    for (uint64_t bb = m_board.whiteBishops(); bb != 0; bb &= bb - 1)
    {
        uint64_t bishop = bb & -bb;
        int bishopSq = bitScanForward(bishop);
        uint64_t mmoves = m_magicbb->pieceAttacks(PIECE_BISHOP, bishopSq, allPieces);

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_WHITE(bishopSq, toSq, BITBOARD_BISHOP, BITBOARD_BISHOP);

            n_moves++;

        } 
    }

    // Rook moves 

    for (uint64_t bb = m_board.whiteRooks(); bb != 0; bb &= bb - 1)
    {
        uint64_t rook = bb & -bb;
        int rookSq = bitScanForward(rook);
        uint64_t mmoves = m_magicbb->pieceAttacks(PIECE_ROOK, rookSq, allPieces);

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_WHITE(rookSq, toSq, BITBOARD_ROOK, BITBOARD_ROOK);

            n_moves++;

        } 
    }

    // Queen moves

    for (uint64_t bb = m_board.whiteQueens(); bb != 0; bb &= bb - 1)
    {
        uint64_t queen = bb & -bb;
        int queenSq = bitScanForward(queen);
        uint64_t mmoves = m_magicbb->pieceAttacks(PIECE_QUEEN, queenSq, allPieces);

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_WHITE(queenSq, toSq, BITBOARD_QUEEN, BITBOARD_QUEEN);

            n_moves++;

        } 
    }


    return n_moves;
}

#define MAKE_MOVE_BLACK(from, to, pieceFrom, pieceTo)   moves[n_moves].sq_from = from;    \
                                    moves[n_moves].sq_to   = to;      \
                                                                        \
                                    moves[n_moves].bitboard_color_from      = BITBOARD_BLACK_PIECES;    \
                                    moves[n_moves].bitboard_color_to        = BITBOARD_BLACK_PIECES;    \
                                    moves[n_moves].bitboard_color_capture   = BITBOARD_WHITE_PIECES;    \
                                                                                                        \
                                    moves[n_moves].from_piece               = pieceFrom;            \
                                    moves[n_moves].to_piece                 = pieceTo;            \
                                                                                                        \
                                    moves[n_moves].capture_piece            = (!!(m_board.whitePawns() & m) * BITBOARD_PAWN)     |     \
                                                                              (!!(m_board.whiteKnights() & m) * BITBOARD_KNIGHT) |     \
                                                                              (!!(m_board.whiteBishops() & m) * BITBOARD_BISHOP) |     \
                                                                              (!!(m_board.whiteRooks() & m) * BITBOARD_ROOK)     |     \
                                                                              (!!(m_board.whiteQueens() & m) * BITBOARD_QUEEN)   |     \
                                                                              (!!(m_board.whiteKings() & m) * BITBOARD_KING);          \
                                                                                                                                       \
                                    moves[n_moves].flags                    = (moves[n_moves].capture_piece != 0) * IS_CAPTURE;



int BetaChess::_generate_moves_black(struct BetaMove* moves)
{
    int n_moves = 0;
    uint64_t myPieces = *m_board.blackPieces();
    uint64_t theirPieces = *m_board.whitePieces();
    uint64_t allPieces = myPieces | theirPieces;

    // King moves
    for (uint64_t bb = m_board.blackKings(); bb != 0; bb &= bb - 1)
    {
        uint64_t king   = bb & -bb;
        int kingSq      = bitScanForward(king);
        uint64_t mmoves  = m_blockers.m_pieceMoves[PIECE_KING][kingSq];

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_BLACK(kingSq, toSq, BITBOARD_KING, BITBOARD_KING);

            n_moves++;

        }

    }

    // Pawn moves
    for (uint64_t bb = m_board.blackPawns(); bb != 0; bb &= bb - 1)
    {
    
        uint64_t pawn   = bb & -bb;
        int pawnSq      = bitScanForward(pawn);
        uint64_t mmoves  = m_blockers.m_pawnMovesWhite[pawnSq];

        mmoves &= ~(myPieces | theirPieces);

        uint64_t captures = m_blockers.m_pawnAttacksWhite[pawnSq];

        captures &= theirPieces;

        mmoves |= captures;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_BLACK(pawnSq, toSq, BITBOARD_PAWN, BITBOARD_PAWN);
            
            n_moves++;

        }
    }

    // Knight moves

    for (uint64_t bb = m_board.blackKnights(); bb != 0; bb &= bb - 1)
    {
        uint64_t knight   = bb & -bb;
        int knightSq      = bitScanForward(knight);
        uint64_t mmoves  = m_blockers.m_pieceMoves[PIECE_KNIGHT][knightSq];

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_BLACK(knightSq, toSq, BITBOARD_KNIGHT, BITBOARD_KNIGHT);

            n_moves++;

        }

    }

    // Bishop moves 

    for (uint64_t bb = m_board.blackBishops(); bb != 0; bb &= bb - 1)
    {
        uint64_t bishop = bb & -bb;
        int bishopSq = bitScanForward(bishop);
        uint64_t mmoves = m_magicbb->pieceAttacks(PIECE_BISHOP, bishopSq, allPieces);

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_BLACK(bishopSq, toSq, BITBOARD_BISHOP, BITBOARD_BISHOP);

            n_moves++;

        } 
    }

    // Rook moves 

    for (uint64_t bb = m_board.blackRooks(); bb != 0; bb &= bb - 1)
    {
        uint64_t rook = bb & -bb;
        int rookSq = bitScanForward(rook);
        uint64_t mmoves = m_magicbb->pieceAttacks(PIECE_ROOK, rookSq, allPieces);

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_BLACK(rookSq, toSq, BITBOARD_ROOK, BITBOARD_ROOK);

            n_moves++;

        } 
    }

    // Queen moves

    for (uint64_t bb = m_board.blackQueens(); bb != 0; bb &= bb - 1)
    {
        uint64_t queen = bb & -bb;
        int queenSq = bitScanForward(queen);
        uint64_t mmoves = m_magicbb->pieceAttacks(PIECE_QUEEN, queenSq, allPieces);

        mmoves &= ~myPieces;

        for (; mmoves != 0; mmoves &= mmoves - 1 /* clear LS1B */)
        {
            uint64_t m = mmoves & -mmoves; // Isolate LS1B - is this needed?

            int toSq = bitScanForward(m); // Get to square   

            MAKE_MOVE_BLACK(queenSq, toSq, BITBOARD_QUEEN, BITBOARD_QUEEN);

            n_moves++;

        } 
    }

   

    return n_moves;
}

