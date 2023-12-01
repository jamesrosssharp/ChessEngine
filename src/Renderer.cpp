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
}

Renderer::~Renderer()
{


}

float Renderer::renderScene(int w, int h)
{

    glViewport(0, 0, w, h); 

    glClearColor(0.1f, 0.1f, 0.1f, 0.1f );
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

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
 
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
    m_chessmen_white[0].visible = true;
    m_chessmen_white[0].x = -1;
    m_chessmen_white[0].y = -0.75;
    m_chessmen_white[0].sprite_type = TYPE_PAWN;
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
        glVertex2f(x2, y2);
        glTexCoord2f(u2, v1);
        glVertex2f(x1, y2);
        glTexCoord2f(u2, v2);
        glVertex2f(x1, y1);
        glTexCoord2f(u1, v2);
        glVertex2f(x2, y1);

    glEnd();
}