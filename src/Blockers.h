/* vim: set et ts=4 sw=4: */

/*
    ChessEngine : A chess engine written in C++ for SDL2

Blockers.h: Compute blockers and beyond lookup tables

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

#include <Pieces.h>
#include <cstdint>

class Blockers
{

    public:

        Blockers();

        void computeBlockersAndBeyond();

        std::uint64_t m_pieceMoves[6][64];
        std::uint64_t m_arrBlockersAndBeyond[6][64];
        std::uint64_t m_arrBehind[64][64];
        std::uint64_t m_pawnMovesWhite[64];
        std::uint64_t m_pawnMovesBlack[64];
        std::uint64_t m_pawnAttacksWhite[64];
        std::uint64_t m_pawnAttacksBlack[64];

};

