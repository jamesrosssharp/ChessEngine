/* vim: set et ts=4 sw=4: */

/*
	Chess Engine

betaChessTest: Test the BetaChess class

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

class BetaChessTest : public ::testing::Test {
    protected:

        void SetUp() override {
            m_chess = new BetaChess();
        }

        void TearDown() override {
            delete m_chess;
        }

        uint64_t RunPerft(int depth) {

            std::chrono::time_point<std::chrono::high_resolution_clock> oldTime = std::chrono::high_resolution_clock::now();        
                uint64_t nodes = m_chess->perft(depth); 
                printf("Perft %d: Nodes: %ld\n", depth, nodes);
            auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime);
            double kNPS = (double)nodes / (usecs.count() / 1'000'000.0) / 1'000.0;    
            printf("kNPS=%1.2f (%1.2f sec)\n", kNPS, usecs.count() / 1'000'000.0);

            return nodes;
        }

        BetaChess *m_chess;
};

TEST_F(BetaChessTest, TestMoveGen)
{
    struct BetaMove moves[512];

    int n_moves = m_chess->generate_moves(moves);

    printf("n moves: %d\n", n_moves);

}

TEST_F(BetaChessTest, TestPerft)
{
    RunPerft(8);
}


