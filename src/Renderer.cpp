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


}

Renderer::~Renderer()
{


}

float Renderer::renderScene(int w, int h)
{

    glViewport(0, 0, w, h); 

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
 
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1, -1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1, -1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1, 1);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1, 1);

    glEnd();

    return 0.0;
} 
