/* vim: set et ts=4 sw=4: */

/*
    ChessEngine : A chess engine written in C++

Renderer.h: Render the board using OpenGL


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

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include <GL/glext.h>
#include <GL/glcorearb.h>

enum RenderSpriteType {
    TYPE_PAWN   = 0,
    TYPE_KNIGHT = 1,
    TYPE_BISHOP = 2,
    TYPE_ROOK   = 3,
    TYPE_KING   = 4,
    TYPE_QUEEN  = 5
};

struct RenderSprite 
{
    enum RenderSpriteType sprite_type;
    double x;
    double y;
    double vx;
    double vy;
    int frames;
    bool visible;
};

class Renderer {

    public:
        Renderer();
        ~Renderer();

        float renderScene(int w, int h);

        void resetBoard();

    private:

        void renderSprite(struct RenderSprite* sp, bool white);

        static constexpr int kChessMenTexture       = 0;
        static constexpr int kNumTextures           = 1;

        GLuint m_textures[kNumTextures];

        static constexpr int kPawns     = 0;
        static constexpr int kBishops   = 8;
        static constexpr int kKnights   = 10;
        static constexpr int kRooks     = 12;
        static constexpr int kKing      = 14;
        static constexpr int kQueen     = 15;

        struct RenderSprite m_chessmen_white[16];
        struct RenderSprite m_chessmen_black[16];

        int m_highlighted_square_x = 0;
        int m_highlighted_square_y = 0;

};

