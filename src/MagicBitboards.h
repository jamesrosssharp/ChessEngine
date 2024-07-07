/* vim: set et ts=4 sw=4: */

/*
	$PROJECT

$FILE: $DESC

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

#include <Chess.h>
#include <Blockers.h>

class MagicBitboards {

    public:

        void computeTables(Blockers* ch);

        uint64_t pieceAttacks(enum SimplePieceTypes piece, int sq, uint64_t occupied)
        {
            switch (piece)
            {
                case PIECE_BISHOP:
                    return bishopAttacks(sq, occupied);
                case PIECE_ROOK:
                    return rookAttacks(sq, occupied);
                case PIECE_QUEEN:
                    return bishopAttacks(sq, occupied) | rookAttacks(sq, occupied);
            }
            return 0;
        }

        uint64_t rookAttacks(int sq, uint64_t occupied)
        {
            uint64_t b = occupied & rook_occupancy[sq];
            uint64_t bits = (b * rook_magics[sq]) >> 64 - 12;
            return rook_lut[bits][sq];   
        }

        uint64_t bishopAttacks(int sq, uint64_t occupied)
        {
            uint64_t b = occupied & bishop_occupancy[sq];
            uint64_t bits = (b * bishop_magics[sq]) >> 64 - 9;
            return bishop_lut[bits][sq];   
        }


    private:


        uint64_t get_bishop_occupancy_set_for_square(int sq);
        uint64_t get_rook_occupancy_set_for_square(int sq);
        int      population_count(uint64_t bb);
        bool     testMagic(Blockers* ch, int sq, bool rook, uint64_t bb, uint64_t magic);
        uint64_t random_uint64_t();
        uint64_t random_uint64_t_fewbits();


    private:

        uint64_t bishop_magics[64];
        uint64_t rook_magics[64];

        uint64_t bishop_occupancy[64];
        uint64_t rook_occupancy[64];

        uint64_t bishop_lut[512][64];
        uint64_t rook_lut[4096][64];

};


