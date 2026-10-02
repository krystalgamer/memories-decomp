#include "../../types.h"
#include "../../psyq/libgte.h"
#include "helpers.h"

void func_80168BE8(void)
{
    s32 row;
    s32 column;
    s32 amplitude;
    s32 phase;
    s32 value;

    D_80169144 += 0x100;
    for (row = 0; row < 5; row++) {
        amplitude = 0x200;
        phase = D_80169144 + (row << 8);
        for (column = 0; column < 9; column++) {
            value = 0;
            if (column == 0) {
                D_80169080[row][column] = 0;
            } else {
                D_80169080[row][column] = value =
                    ((amplitude >> 8) * rcos(phase & 0xFFF)) / 4096;
                if (value < 0) {
                    value <<= 4;
                } else {
                    value <<= 5;
                }
            }
            value += 0x80;
            if (value < 0) {
                value = 0;
            }
            if (value >= 0x100) {
                value = 0xFF;
            }
            D_80169148[row][column] = value | (value << 8) | (value << 16);
            amplitude += 0x60;
            phase -= 0x200;
        }
    }
}
