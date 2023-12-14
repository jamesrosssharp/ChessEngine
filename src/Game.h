/* vim: set et ts=4 sw=4: */

/*
	ChessEngine : A chess engine written in C++ for SDL2

Game.h: Game engine

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

#include <thread>
#include <semaphore>

#include "Renderer.h"
#include "UI.h"
#include "Chess.h"


class Game 
{
    public:

        Game();        

        void handleLeftKeyDown();
        void handleRightKeyDown();
        void handleUpKeyDown();
        void handleDownKeyDown();
        void handleAKeyDown();
        void handleBKeyDown();

        void chessThread(); 

        float renderScene(int w, int h);
    
    private:
        std::thread m_chessThread;
        std::binary_semaphore m_chessSem;
        std::atomic<bool> m_threadExit;

        Renderer m_r;
        Chess    m_ch;
        UI       m_u;

};
