/* vim: set et ts=4 sw=4: */

/*
    ChessEngine : A chess engine written in C++

Renderer.cpp: Render the board using OpenGL

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

#include "Renderer.h"

#include "chess_piece_texture.h"

#include <string.h>

Renderer::Renderer()
{
    glEnable(GL_TEXTURE_2D);

    glGenTextures(kNumTextures, m_textures);

    glBindTexture(GL_TEXTURE_2D, m_textures[kChessMenTexture]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, chess_sprites.width, chess_sprites.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, chess_sprites.pixel_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    for (int i = 0; i < 16; i++)
    {
        m_chessmen_white[i].visible = false;
        m_chessmen_black[i].visible = false;
    }

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            m_legal_moves[i][j] = false;
}

Renderer::~Renderer()
{


}

float Renderer::renderScene(int w, int h)
{

    glViewport(0, 0, w, h); 

    glClearColor(0.3f, 0.3f, 0.3f, 0.3f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if ((x + y) % 2 == 0)
                continue;

            float x1 = -1 + x/4.0;
            float x2 = -1 + (x+1)/4.0;
            float y1 = -1 + y/4.0;
            float y2 = -1 + (y+1)/4.0;

            glBegin(GL_QUADS);
            glColor3f(0.9, 0.9, 0.9);
            glVertex2f(x1, y1);
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);
            glEnd();

        }    
    }

    // Draw currently highlighted square

    {
        float x1 = m_highlighted_square_x/4.0 - 1.0 ;
        float y1 = m_highlighted_square_y/4.0 - 1.0 ;
        float x2 = (m_highlighted_square_x + 1)/4.0 - 1.0;
        float y2 = (m_highlighted_square_y + 1)/4.0 - 1.0;
       
        float x3 = m_highlighted_square_x/4.0 - 1.0 + 0.01;
        float y3 = m_highlighted_square_y/4.0 - 1.0 + 0.01;
        float x4 = (m_highlighted_square_x + 1)/4.0 - 1.0 - 0.01;
        float y4 = (m_highlighted_square_y + 1)/4.0 - 1.0 - 0.01;
        
        glBegin(GL_QUADS);
        glColor3f(1.0, 0.49, 0.0);
        
        glVertex2f(x1, y1);
        glVertex2f(x3, y3);
        glVertex2f(x3, y4);
        glVertex2f(x1, y2);

        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x4, y3);
        glVertex2f(x3, y3);

        glVertex2f(x4, y3);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x4, y4);

        glVertex2f(x3, y4);
        glVertex2f(x4, y4);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);

        glEnd();

    }
    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
 
    // Draw currently selected square
    
    if (m_square_selected) {
        float x1 = m_selected_square_x/4.0 - 1.0 ;
        float y1 = m_selected_square_y/4.0 - 1.0 ;
        float x2 = (m_selected_square_x + 1)/4.0 - 1.0;
        float y2 = (m_selected_square_y + 1)/4.0 - 1.0;
       
        glBegin(GL_QUADS);
        glColor4f(1.0, 0.49, 0.0, 0.7);
        
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);

        glEnd();
    }

    // Draw legal moves

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {

            if (m_legal_moves[j][i])
            {

                float x1 = i/4.0 - 1.0 ;
                float y1 = j/4.0 - 1.0 ;
                float x2 = (i + 1)/4.0 - 1.0;
                float y2 = (j + 1)/4.0 - 1.0;
               
                glBegin(GL_QUADS);
                glColor4f(0.89, 0.79, 0.29, 0.7);
                
                glVertex2f(x1, y1);
                glVertex2f(x2, y1);
                glVertex2f(x2, y2);
                glVertex2f(x1, y2);

                glEnd();
            }

        }
    }

    // Draw chessmen

    glColor4f(1.0, 1.0, 1.0, 1.0); 
    glEnable(GL_TEXTURE_2D);


    for (int i = 0; i < 16; i++)
    {
        renderSprite(&m_chessmen_white[i], true);
        renderSprite(&m_chessmen_black[i], false);

    }

    glDisable(GL_BLEND);

    return 0.0;
}

void Renderer::resetBoard()
{
    // Set up white pawns
    for (int i = 0; i < 8; i++)
    {
        struct RenderSprite* sp = &m_chessmen_white[kPawns + i];

        sp->visible = true;
        sp->x = -1 + i/4.0;
        sp->y = -0.75;
        sp->sprite_type = TYPE_PAWN;
    }

    // White bishops

    for (int i = 0; i < 2; i++)
    {
        struct RenderSprite* sp = &m_chessmen_white[kBishops + i];

        sp->visible = true;
        sp->x = (i == 0) ? -0.5 : 0.25;
        sp->y = -1.0;
        sp->sprite_type = TYPE_BISHOP;
    }

    // White knights

    for (int i = 0; i < 2; i++)
    {
        struct RenderSprite* sp = &m_chessmen_white[kKnights + i];

        sp->visible = true;
        sp->x = (i == 0) ? -0.75 : 0.5;
        sp->y = -1.0;
        sp->sprite_type = TYPE_KNIGHT;
    }

    // White rooks

    for (int i = 0; i < 2; i++)
    {
        struct RenderSprite* sp = &m_chessmen_white[kRooks + i];

        sp->visible = true;
        sp->x = (i == 0) ? -1.0 : 0.75;
        sp->y = -1.0;
        sp->sprite_type = TYPE_ROOK;
    }

    // White King

    {
        struct RenderSprite* sp = &m_chessmen_white[kKing];

        sp->visible = true;
        sp->x = 0.0;
        sp->y = -1.0;
        sp->sprite_type = TYPE_KING;
    }

    // White queen

    {
        struct RenderSprite* sp = &m_chessmen_white[kQueen];

        sp->visible = true;
        sp->x = -0.25;
        sp->y = -1.0;
        sp->sprite_type = TYPE_QUEEN;
    }

    // Set up black pawns
    for (int i = 0; i < 8; i++)
    {
        struct RenderSprite* sp = &m_chessmen_black[kPawns + i];

        sp->visible = true;
        sp->x = -1 + i/4.0;
        sp->y = 0.5;
        sp->sprite_type = TYPE_PAWN;
    }

    // BLack bishops

    for (int i = 0; i < 2; i++)
    {
        struct RenderSprite* sp = &m_chessmen_black[kBishops + i];

        sp->visible = true;
        sp->x = (i == 0) ? -0.5 : 0.25;
        sp->y = 0.75;
        sp->sprite_type = TYPE_BISHOP;
    }

    // Black knights

    for (int i = 0; i < 2; i++)
    {
        struct RenderSprite* sp = &m_chessmen_black[kKnights + i];

        sp->visible = true;
        sp->x = (i == 0) ? -0.75 : 0.5;
        sp->y = 0.75;
        sp->sprite_type = TYPE_KNIGHT;
    }

    // Black rooks

    for (int i = 0; i < 2; i++)
    {
        struct RenderSprite* sp = &m_chessmen_black[kRooks + i];

        sp->visible = true;
        sp->x = (i == 0) ? -1.0 : 0.75;
        sp->y = 0.75;
        sp->sprite_type = TYPE_ROOK;
    }

    // Black King

    {
        struct RenderSprite* sp = &m_chessmen_black[kKing];

        sp->visible = true;
        sp->x = 0.0;
        sp->y = 0.75;
        sp->sprite_type = TYPE_KING;
    }

    // Black queen

    {
        struct RenderSprite* sp = &m_chessmen_black[kQueen];

        sp->visible = true;
        sp->x = -0.25;
        sp->y = 0.75;
        sp->sprite_type = TYPE_QUEEN;
    }


}

void Renderer::renderSprite(struct RenderSprite* sp, bool white)
{
    double u1, u2;
    double v1, v2;
    double x1, x2;
    double y1, y2;


    if (!sp->visible) return;

    x1 = sp->x;
    x2 = sp->x + 1/4.0;
    y1 = sp->y;
    y2 = sp->y + 1/4.0;

    switch (sp->sprite_type)
    {
        case TYPE_PAWN:
                u1 = 5.0 / 6.0;
                u2 = 1.0;
                break;
        case TYPE_BISHOP:
                u1 = 2.0 / 6.0;
                u2 = 3.0 / 6.0;
                break;
        case TYPE_KNIGHT:
                u1 = 3.0 / 6.0;
                u2 = 4.0 / 6.0;
                break;
        case TYPE_ROOK:
                u1 = 4.0 / 6.0;
                u2 = 5.0 / 6.0;
                break;
        case TYPE_KING:
                u1 = 0.0 / 6.0;
                u2 = 1.0 / 6.0;
                break;
        case TYPE_QUEEN:
                u1 = 1.0 / 6.0;
                u2 = 2.0 / 6.0;
                break;
        default: 
                break;        
    };

    if (white)
    {
        v1 = 0;
        v2 = 1 / 2.0;
    }
    else
    {
        v1 = 1 / 2.0;
        v2 = 2 / 2.0;
    }

    glBegin(GL_QUADS);

        glTexCoord2f(u1, v1);
        glVertex2f(x1, y2);
        glTexCoord2f(u2, v1);
        glVertex2f(x2, y2);
        glTexCoord2f(u2, v2);
        glVertex2f(x2, y1);
        glTexCoord2f(u1, v2);
        glVertex2f(x1, y1);

    glEnd();
}

void Renderer::setHighlightedSquare(int x, int y)
{

    m_highlighted_square_x = x;
    m_highlighted_square_y = y; 

}

void Renderer::setSelectedSquare(bool squareSelected, int x, int y)
{

    m_selected_square_x = x;
    m_selected_square_y = y;
    m_square_selected   = squareSelected;

}

void Renderer::setLegalMoves(bool* legalMoves)
{
    memcpy(&m_legal_moves[0][0], legalMoves, sizeof(m_legal_moves));
}
