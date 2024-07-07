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

#include <MagicBitboards.h>

std::vector<std::pair<int, int>> bishop_moves = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
std::vector<std::pair<int, int>> rook_moves   = {{1, 0}, {-1, 0}, { 0, 1},  {0, -1}};

uint64_t MagicBitboards::get_bishop_occupancy_set_for_square(int sq)
{
        int x1 = sq & 7;
        int y1 = sq >> 3;


        uint64_t bb = 0;

        for (const auto& m : bishop_moves)
        {

            for (int i = 1; i < 8; i++)
            {
                int x2 = x1 + i*m.first;
                int y2 = y1 + i*m.second;
                int x3 = x1 + (i+1)*m.first;
                int y3 = y1 + (i+1)*m.second;

                if (IS_IN_BOARD(x3, y3)) bb 
                        |= COORD_TO_BIT(x2, y2); 
                if (!IS_IN_BOARD(x2, y2)) break;

            }
        }

    return bb;
}

uint64_t MagicBitboards::get_rook_occupancy_set_for_square(int sq)
{
        int x1 = sq & 7;
        int y1 = sq >> 3;


        uint64_t bb = 0;

        for (const auto& m : rook_moves)
        {

            for (int i = 1; i < 8; i++)
            {
                int x2 = x1 + i*m.first;
                int y2 = y1 + i*m.second;
                int x3 = x1 + (i+1)*m.first;
                int y3 = y1 + (i+1)*m.second;

                if (IS_IN_BOARD(x3, y3)) bb 
                        |= COORD_TO_BIT(x2, y2); 
                if (!IS_IN_BOARD(x2, y2)) break;

            }
        }

    return bb;
}

int MagicBitboards::population_count(uint64_t bb)
{
    int sum = 0;
    
    if (bb && !(bb & (bb - 1))) return 1;    // Power of two: 1 bit
    
    while (bb) {
       sum++;
       bb &= bb - 1;
    }

    return sum;
}

bool MagicBitboards::testMagic(Blockers* ch, int sq, bool rook, uint64_t bb, uint64_t magic)
{

    // Count bits in bb

    int count = population_count(bb);

    int used[4096] = {0};

    for (int i = 1; i <= (1<<count); i++)
    {
        // Enumerate occupancy set with binary counter
        
        uint64_t occupied_bb = 0;
        int j = i;

        for (uint64_t b = bb; b != 0; b &= b - 1)
        {
            uint64_t bit = b & -b;

            if (j & 1) occupied_bb |= bit;

            j >>= 1;
        } 
   
        // Multiply by magic
   
        uint64_t mult = occupied_bb * magic; 

        //if (population_count(mult & 0xff00'0000'0000'0000) < 6) return false;

        // Shift by bits in occupancy set

        //mult >>= (64 - count);

        if (rook)
            mult >>= (64 - 12);
        else
            mult >>= (64 - 9);

        if (used[mult] == 0) 
            used[mult] = i;
        else if (used[mult] != i) 
            return false;

        if (rook)
        {
            // Compute rook attack set and store in the table
            uint64_t a = ch->pieceAttacks(PIECE_ROOK, sq, occupied_bb);
            rook_lut[mult][sq] = a;
        }
        else
        {
            // Compute bishop attack set and store in the table
            uint64_t a = ch->pieceAttacks(PIECE_BISHOP, sq, occupied_bb);
            bishop_lut[mult][sq] = a;
        }

    }

    return true;
}

uint64_t MagicBitboards::random_uint64_t() {
  uint64_t u1, u2, u3, u4;
  u1 = (uint64_t)(random()) & 0xFFFF; u2 = (uint64_t)(random()) & 0xFFFF;
  u3 = (uint64_t)(random()) & 0xFFFF; u4 = (uint64_t)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64_t MagicBitboards::random_uint64_t_fewbits() {
  return random_uint64_t() & random_uint64_t() & random_uint64_t();
}


void MagicBitboards::computeTables(Blockers* ch)
{

    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t bb = get_rook_occupancy_set_for_square(sq);

        rook_occupancy[sq] = bb;

        while (true)
        {
            uint64_t magic =  random_uint64_t_fewbits(); //0x100420000431024ULL; //gen64BitMagicNumber();
    
            if (testMagic(ch, sq, true, bb, magic))
            {
                //printf("Magic found for rook on sq %d %lx, bb = %lx\n", sq, magic, bb);
                //return 0;
                rook_magics[sq] = magic;
                break;
            }

        }

    }

    // Compute magics for bishops

    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t bb = get_bishop_occupancy_set_for_square(sq);

        bishop_occupancy[sq] = bb;

        while (true)
        {
            uint64_t magic =  random_uint64_t_fewbits(); //0x100420000431024ULL; //gen64BitMagicNumber();
    
            if (testMagic(ch, sq, false, bb, magic))
            {
                //printf("Magic found for bishop on sq %d %lx\n", sq, magic);
                //return 0;
                bishop_magics[sq] = magic;
                break;
            }

        }

    }

}

