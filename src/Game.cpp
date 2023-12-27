/* vim: set et ts=4 sw=4: */

/*
	ChessEngine : A chess engine written in C++ for SDL2

Game.cpp : game engine

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

#include "Game.h"

Game::Game()    :
    m_chessSem{0},
    m_threadExit{0},
    m_u(&m_r, &m_ch)
{

    // Create our game thread
    m_chessThread = std::thread([](void* a) { Game* g = (Game*)a; g->chessThread(); }, this);
    m_r.resetBoard();

}

void Game::handleLeftKeyDown()
{
    m_u.handleLeftKeyDown();
}

void Game::handleRightKeyDown()
{
    m_u.handleRightKeyDown();
}

void Game::handleUpKeyDown()
{
    m_u.handleUpKeyDown();
}

void Game::handleDownKeyDown()
{
    m_u.handleDownKeyDown();
}
 
void Game::handleAKeyDown()
{
    if (m_u.handleAKeyDown())
    {
        m_u.lock();
    
        m_chessSem.release();
    }
}

void Game::handleBKeyDown()
{
    m_u.handleRightKeyDown();
}

void Game::chessThread()
{
    while (! m_threadExit)
    {
        m_chessSem.acquire();

        while (m_r.animating())
            usleep(10000);

        printf("Computing next move...\n");
        
        int x1,y1,x2,y2;
        bool ep, castle_kings_side, castle_queens_side;
        enum PromotionType promote;

        m_ch.getBestMove(x1, y1, x2, y2, promote);

        m_ch.makeMove(x1, y1, x2, y2, ep, castle_kings_side, castle_queens_side);
        m_r.movePiece(x1, y1, x2, y2, ep, castle_kings_side, castle_queens_side);
         
        while (m_r.animating())
            usleep(10000);
        
        if (promote != NO_PROMOTION)
        {
            switch (promote)
            {
                case PROMOTION_PROMOTE_TO_QUEEN:
                    m_r.promoteToQueen(x2, y2);
                    break;
                case PROMOTION_PROMOTE_TO_ROOK:
                    m_r.promoteToRook(x2, y2);
                    break;
                case PROMOTION_PROMOTE_TO_BISHOP:
                    m_r.promoteToBishop(x2, y2);
                    break;
                case PROMOTION_PROMOTE_TO_KNIGHT:
                    m_r.promoteToKnight(x2, y2);
                    break;    
                default:
                    break;
            }
        }
   
        m_u.unlock(); 
    }
}

float Game::renderScene(int w, int h)
{
    return m_r.renderScene(w, h);
}
