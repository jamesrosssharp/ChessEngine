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
#include <vector>
#include <functional>

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

enum SimplePieceTypes {
    PIECE_PAWN = 0,
    PIECE_KNIGHT = 1,
    PIECE_BISHOP = 2,
    PIECE_ROOK = 3,
    PIECE_QUEEN = 4,
    PIECE_KING = 5
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
#define IS_IN_BOARD(file, rank) ((file >= A_FILE) && (file <= H_FILE) && (rank >= FIRST_RANK) && (rank <= EIGHTH_RANK))



struct ChessMove {

    ChessMove() :
        x1(INVALID_FILE),
        y1(INVALID_RANK),
        x2(INVALID_FILE),
        y2(INVALID_RANK)
    {

    }

    ChessMove(int _x1, int _y1, int _x2, int _y2) :
        x1(_x1),
        y1(_y1),
        x2(_x2),
        y2(_y2)
    {

    }

    int x1, y1;
    int x2, y2;
};

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

    uint64_t whitePiecesBoard;
    uint64_t blackPiecesBoard;

    uint64_t allPiecesBoard;

    // Flags 
    bool m_isWhitesTurn;
    int m_can_en_passant_file;
    bool m_whiteKingHasMoved;
    bool m_blackKingHasMoved;
    bool m_whiteARookHasMoved;
    bool m_whiteHRookHasMoved;
    bool m_blackARookHasMoved;
    bool m_blackHRookHasMoved;

    uint64_t* pawns[2];
    uint64_t* knights[2];
    uint64_t* bishops[2];
    uint64_t* rooks[2];
    uint64_t* queens[2];
    uint64_t* kings[2]; 

    ChessBoard(const ChessBoard& other)
    {
        whitePawnsBoard = other.whitePawnsBoard;
        blackPawnsBoard = other.blackPawnsBoard;
        whiteKnightsBoard = other.whiteKnightsBoard;
        blackKnightsBoard = other.blackKnightsBoard;
        whiteBishopsBoard = other.whiteBishopsBoard;
        blackBishopsBoard = other.blackBishopsBoard;
        whiteRooksBoard   = other.whiteRooksBoard;
        blackRooksBoard   = other.blackRooksBoard;
        whiteQueensBoard  = other.whiteQueensBoard;
        blackQueensBoard  = other.blackQueensBoard;
        whiteKingsBoard   = other.whiteKingsBoard;
        blackKingsBoard   = other.blackKingsBoard;
        setUpArrays();
        m_isWhitesTurn  = other.m_isWhitesTurn;

    }

    ChessBoard()
    {
        setUpArrays();
    }

    void setUpArrays()
    {
        pawns[1] = &whitePawnsBoard;
        pawns[0] = &blackPawnsBoard;
        knights[1] = &whiteKnightsBoard;
        knights[0] = &blackKnightsBoard;
        bishops[1] = &whiteBishopsBoard;
        bishops[0] = &blackBishopsBoard;
        rooks[1] = &whiteRooksBoard;
        rooks[0] = &blackRooksBoard;
        queens[1] = &whiteQueensBoard;
        queens[0] = &blackQueensBoard;
        kings[1]  = &whiteKingsBoard;
        kings[0]  = &blackKingsBoard;
    }

    uint64_t* myPawns()
    {
        return pawns[(int)m_isWhitesTurn];
    }

    uint64_t* oppPawns()
    {
        return pawns[(int)!m_isWhitesTurn];
    }

    uint64_t* myKnights()
    {
        return knights[(int)m_isWhitesTurn];
    }

    uint64_t* oppKnights()
    {
        return knights[(int)!m_isWhitesTurn];
    }

    uint64_t* myBishops()
    {
        return bishops[(int)m_isWhitesTurn];
    }

    uint64_t* oppBishops()
    {
        return bishops[(int)!m_isWhitesTurn];
    }

    uint64_t* myRooks()
    {
        return rooks[(int)m_isWhitesTurn];
    }

    uint64_t* oppRooks()
    {
        return rooks[(int)!m_isWhitesTurn];
    }

    uint64_t* myQueens()
    {
        return queens[(int)m_isWhitesTurn];
    }

    uint64_t* oppQueens()
    {
        return queens[(int)!m_isWhitesTurn];
    }

    uint64_t* myKings()
    {
        return kings[(int)m_isWhitesTurn];
    }

    uint64_t* oppKings()
    {
        return kings[(int)!m_isWhitesTurn];
    }

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

    void clearOppPieces(uint64_t bb)
    {
        *oppPawns() = *oppPawns() & ~bb;
        *oppKnights() = *oppKnights() & ~bb;
        *oppBishops() = *oppBishops() & ~bb;
        *oppRooks() = *oppRooks() & ~bb;
        *oppQueens() = *oppQueens() & ~bb;
        *oppKings() = *oppKings() & ~bb;
    }

    void nextTurn()
    {
        m_isWhitesTurn = !m_isWhitesTurn;
    }

    std::vector<ChessMove> m_legalMoves;
};



class Chess {

    public:
    
        Chess();

        void resetBoard();

        void getLegalMovesForSquare(int x, int y, uint64_t& moveSquares);
        void getLegalMovesForBoardSquare(const ChessBoard& board, int x, int y, uint64_t& moveSquares);
        void printBoard(const ChessBoard& board);

        void makeMove(int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side);
        void makeMoveForBoard(ChessBoard& board, int x1, int y1, int x2, int y2, bool& ep, bool& castle_kings_side, bool& castle_queens_side, bool print = true, bool recompute_legal = false);

        void getBestMove(int& x1, int& y1, int& x2, int& y2); 

        void getLegalMovesForBoardAsVector(const ChessBoard& board, std::vector<ChessMove>& vec);
        void getLegalMovesForBoardAsVectorSlow(const ChessBoard& board, std::vector<ChessMove>& vec);

        void printBitBoard(uint64_t board);

        std::uint64_t getArrBehind(int sq1, int sq2) { return m_arrBehind[sq1][sq2]; }
        std::uint64_t getPieceMoves(int piece, int sq) { return m_pieceMoves[piece][sq]; }
        std::uint64_t getBlockersAndBeyond(int piece, int sq) { return m_arrBlockersAndBeyond[piece][sq]; }
        std::uint64_t getWhitePawnAttacks(int sq) { return m_pawnAttacksWhite[sq]; }
        std::uint64_t getBlackPawnAttacks(int sq) { return m_pawnAttacksBlack[sq]; }

    protected:

        void evalBoard(const ChessBoard& board, double& white_score, double& black_score);
        void evalBoardFaster(const ChessBoard& board, double& white_score, double& black_score);

        double minimaxAlphaBeta(const ChessBoard& board, bool white, ChessMove& move, bool maximizing, int depth, int& npos, double alpha, double beta);
        double minimaxAlphaBetaFaster(ChessBoard& board, bool white, ChessMove& move, bool maximizing, int depth, int& npos, double alpha, double beta);

        void generateMovesFast(ChessBoard& board, std::function<bool (ChessBoard& b, uint64_t, uint64_t)>);

        bool movePutsPlayerInCheck(const ChessBoard& board, int x1, int y1, int x2, int y2, bool white);
        
        bool kingIsInCheck(const ChessBoard& board, bool white);
        uint64_t movesForPlayer(const ChessBoard& board, bool white); 

        enum PieceTypes getPieceForSquare(const ChessBoard& board, int x, int y);
        std::string prettyPiece          (enum PieceTypes piece);
        void removePieceFromSquare       (ChessBoard& board, enum PieceTypes type, int x, int y);
        void addPieceToSquare            (ChessBoard& board, enum PieceTypes type, int x, int y);

        void computeBlockersAndBeyond();

        static int bitScanForward(uint64_t bb)
        {
            return __builtin_ctzll(bb);
        }

        uint64_t pieceAttacks(enum SimplePieceTypes  piece, int sq, uint64_t occupied)
        {
            // First, get piece moves
            uint64_t moves = m_pieceMoves[piece][sq];

            // Compute blockers
            for (uint64_t bb = occupied & m_arrBlockersAndBeyond[piece][sq]; bb != 0; bb &= bb - 1)
            {
                int sq2 = bitScanForward(bb);
                moves &= ~m_arrBehind[sq][sq2];
            }

            return moves;
        }

        void moveFromBitboards(ChessMove& move, uint64_t from, uint64_t to)
        {
            int fromSq = bitScanForward(from);
            int toSq   = bitScanForward(to);

            move.x1 = fromSq & 7;
            move.y1 = fromSq >> 3;
            move.x2 = toSq & 7;
            move.y2 = toSq >> 3;
                
        } 

        ChessBoard m_board;

        std::uint64_t m_totalCheckTestMicroseconds;
        std::uint64_t m_totalGenerateMoveMicroseconds;
        std::uint64_t m_totalEvaluateMicroseconds;
        std::uint64_t m_totalGenLegalMicroseconds;

        // Lookup tables for "blockers and beyond": https://www.chessprogramming.org/Blockers_and_Beyond
        std::uint64_t m_pieceMoves[6][64];
        std::uint64_t m_arrBlockersAndBeyond[6][64];
        std::uint64_t m_arrBehind[64][64];
        std::uint64_t m_pawnMovesWhite[64];
        std::uint64_t m_pawnMovesBlack[64];
        std::uint64_t m_pawnAttacksWhite[64];
        std::uint64_t m_pawnAttacksBlack[64];

};
