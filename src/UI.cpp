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

#include "UI.h"

#include <cstdio>

UI::UI(Renderer* r, Chess* ch)    :
    m_highlighted_x(0),
    m_highlighted_y(0),
    m_renderer(r),
    m_selected_square_x(0),
    m_selected_square_y(0),
    m_square_selected(false),
    m_ch(ch)
{
    
}

void UI::handleLeftKeyDown()
{
    if (m_renderer->animating()) return;
    if (m_renderer->animating()) return;

    if (m_highlighted_x > 0)
        m_highlighted_x --;
    m_renderer->setHighlightedSquare(m_highlighted_x, m_highlighted_y);
}

void UI::handleRightKeyDown()
{
    if (m_renderer->animating()) return;
    if (m_renderer->animating()) return;
    if (m_highlighted_x < 7)
        m_highlighted_x ++;
    m_renderer->setHighlightedSquare(m_highlighted_x, m_highlighted_y);
}

void UI::handleUpKeyDown()
{
    if (m_renderer->animating()) return;
    if (m_highlighted_y < 7)
        m_highlighted_y ++;
    m_renderer->setHighlightedSquare(m_highlighted_x, m_highlighted_y);
}

void UI::handleDownKeyDown()
{
    if (m_renderer->animating()) return;
    if (m_highlighted_y > 0)
        m_highlighted_y --;
    m_renderer->setHighlightedSquare(m_highlighted_x, m_highlighted_y);
}

bool UI::handleAKeyDown()
{
    bool ret = false;

    if (m_renderer->animating() || m_locked) return false;
    if (m_square_selected)
    {
        // If user already selected this square, deselect it 
        if ((m_selected_square_x == m_highlighted_x) && (m_selected_square_y == m_highlighted_y))
        {
            m_square_selected = false;
            clearLegalMoves();
        }
        else
        {
            printf("Move from %d,%d to %d,%d\n", m_selected_square_x, m_selected_square_y, m_highlighted_x, m_highlighted_y);
        
            // Check if move if legal

            if (!(m_legal_moves & (1ULL << (m_highlighted_y*8 + m_highlighted_x))))
            { 
                // Not legal - queue "Illegal move" to speech synth
                printf("Illegal move!\n");
            }
            else 
            {
                // Legal - make move in renderer, and pass to engine to get response
                bool ep = false;
                bool castle_kings_side = false; 
                bool castle_queens_side = false;

                m_ch->makeMove(m_selected_square_x, m_selected_square_y, m_highlighted_x, m_highlighted_y, ep, castle_kings_side, castle_queens_side);
                m_renderer->movePiece(m_selected_square_x, m_selected_square_y, m_highlighted_x, m_highlighted_y, ep, castle_kings_side, castle_queens_side);
                clearLegalMoves();
                m_square_selected = false;
                ret = true;
            }    
        }
    }
    else
    {
        m_square_selected = true;
        m_selected_square_x = m_highlighted_x;
        m_selected_square_y = m_highlighted_y;
        m_ch->getLegalMovesForSquare(m_highlighted_x, m_highlighted_y, m_legal_moves);
    } 

    m_renderer->setSelectedSquare(m_square_selected, m_selected_square_x, m_selected_square_y);
    m_renderer->setLegalMoves(m_legal_moves); 

    return ret;

}

void UI::handleBKeyDown()
{

}

void UI::clearLegalMoves()
{
    m_legal_moves = 0;        
}
