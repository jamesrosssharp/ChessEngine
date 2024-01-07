/* vim: set et ts=4 sw=4: */

/*
	Chess Engine

main.cpp: Find magics for magic bitboards

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

#include <vector>
#include <cstdint>
#include <../src/Chess.h>
#include <stdlib.h>

std::vector<std::pair<int, int>> bishop_moves = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
 ; 

uint64_t get_bishop_occupancy_set_for_square(int sq)
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

void seedRNG()
{
    srandom(0x666);
}

uint64_t gen64BitMagicNumber()
{
    // TODO: Change this for a better random number generator
    uint64_t r = (random() << 32) | random();
    
    return r;
}

int population_count(uint64_t bb)
{
    int sum = 0;
    
    if (bb && !(bb & (bb - 1))) return 1;    // Power of two: 1 bit
    
    while (bb) {
       sum++;
       bb &= bb - 1;
    }

    return sum;
}

bool testMagic(uint64_t bb, uint64_t magic)
{

    // Count bits in bb

    int count = population_count(bb);

    for (int i = 0; i < (1<<count); i++)
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
   
        //printf("Magic: %lx bb: %lx i: %d occ: %lx\n", magic, bb, i, occupied_bb);

        // Multiply by magic
   
        uint64_t mult = occupied_bb * magic; 

        // Shift by bits in occupancy set

        mult >>= count;

        printf("m: %lx i: %x\n", mult, i);

        // Does resulting bit set match the generated count? If not, return false.

        if (mult != (uint64_t)i) return false;

    }

    return true;
}

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    seedRNG();

    // Compute magics for bishops

    for (int sq = 5; sq < 64; sq++)
    {
        uint64_t bb = get_bishop_occupancy_set_for_square(sq);

        while (true)
        {
            uint64_t magic = gen64BitMagicNumber();
    
            if (testMagic(bb, magic))
            {
                printf("Magic found for bishop on sq %d\n", sq);
                break;
            }

        }

    }


}
