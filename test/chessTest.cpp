/* vim: set et ts=4 sw=4: */

/*
	slurmemu-ng: Next-Generation Slurm16 emulator

cpuTest.cpp: CPU Tester

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

#include <gtest/gtest.h>

#include <Chess.h>

class ChessTest : public ::testing::Test {
    protected:

        void SetUp() override {
            m_chess = new Chess();
        }

        void TearDown() override {
            delete m_chess;
        }

        Chess *m_chess;
};

TEST_F(ChessTest, TestBlockersAndBeyond)
{
    // Test the lookup tables generated for blockers and beyond

    printf("Arr behind for d4, e5:\n");
    m_chess->printBitBoard(m_chess->getArrBehind(27,36));
    ASSERT_EQ(m_chess->getArrBehind(27,36), (1ULL<<45) | (1ULL<<54) | (1ULL<<63));

    printf("Arr behind for a1, f1:\n");
    m_chess->printBitBoard(m_chess->getArrBehind(0,5));
    ASSERT_EQ(m_chess->getArrBehind(0,5), (1ULL<<6) | (1ULL<<7));

    printf("Arr behind for f1, f4:\n");
    m_chess->printBitBoard(m_chess->getArrBehind(5,37));
    ASSERT_EQ(m_chess->getArrBehind(5,37), (1ULL<<45) | (1ULL<<53) | (1ULL<<61));

    printf("Arr behind for e2, e3:\n");
    m_chess->printBitBoard(m_chess->getArrBehind(12, 20));

    printf("Arr behind for d7, d6:\n");
    m_chess->printBitBoard(m_chess->getArrBehind(51, 43));



    printf("Piece moves for Knight, a3\n");
    m_chess->printBitBoard(m_chess->getPieceMoves(PIECE_KNIGHT, 16));

    printf("Piece moves for Bishop, e5\n");
    m_chess->printBitBoard(m_chess->getPieceMoves(PIECE_BISHOP, 36));

    printf("Piece moves for Rook, h8\n");
    m_chess->printBitBoard(m_chess->getPieceMoves(PIECE_ROOK, 63));

    printf("Piece moves for Queen, d3\n");
    m_chess->printBitBoard(m_chess->getPieceMoves(PIECE_QUEEN, 19));

    printf("Piece moves for King, f7\n");
    m_chess->printBitBoard(m_chess->getPieceMoves(PIECE_KING, 53));

    printf("Blockers and beyond for bishop, f7\n");
    m_chess->printBitBoard(m_chess->getBlockersAndBeyond(PIECE_BISHOP, 53));

    printf("Blockers and beyond for queen, d4\n");
    m_chess->printBitBoard(m_chess->getBlockersAndBeyond(PIECE_QUEEN, 27));

    printf("White Pawn attacks for e3\n");
    m_chess->printBitBoard(m_chess->getWhitePawnAttacks(20));

    printf("Black Pawn attacks for d7\n");
    m_chess->printBitBoard(m_chess->getBlackPawnAttacks(51));

    printf("Rook attacks for e8\n");
    m_chess->printBitBoard(m_chess->getPieceMoves(PIECE_ROOK, 60));
}

TEST_F(ChessTest, perft)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();        
        uint64_t nodes = m_chess->perft(5); 
        printf("Perft: Nodes: %ld\n", nodes);
    auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
    double kNPS = (double)nodes / (usecs.count() / 1'000'000.0) / 1'000.0;    
    printf("kNPS=%1.2f\n", kNPS);
}
