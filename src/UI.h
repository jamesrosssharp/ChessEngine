/* vim: set et ts=4 sw=4: */

/*
        Chess Engine

    UI.cpp : User interface functions

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

#include "Renderer.h"

class UI
{
    public:
        
        UI(Renderer* r);

        void handleLeftKeyDown();
        void handleRightKeyDown();
        void handleUpKeyDown();
        void handleDownKeyDown();
        void handleAKeyDown();
        void handleBKeyDown();


    private:

        int     m_highlighted_x;
        int     m_highlighted_y;

        bool    m_is_active;

        Renderer* m_renderer;

        int     m_selected_square_x;
        int     m_selected_square_y;
        bool    m_square_selected;
};

