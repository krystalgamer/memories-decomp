#include "../../types.h"
#include "../../psyq/libgte.h"
#include "helpers.h"

void PalOptions_UpdateWaveTables(void)
{
    s32 row;
    s32 column;
    s32 amplitude;
    s32 phase;
    s32 value;

    gPalOptions_dwWavePhase += 0x100;
    for (row = 0; row < 5; row++) {
        amplitude = 0x200;
        phase = gPalOptions_dwWavePhase + (row << 8);
        for (column = 0; column < 9; column++) {
            value = 0;
            if (column == 0) {
                gPalOptions_aWaveOffsets[row][column] = 0;
            } else {
                gPalOptions_aWaveOffsets[row][column] = value =
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
            gPalOptions_aWaveColors[row][column] = value | (value << 8) | (value << 16);
            amplitude += 0x60;
            phase -= 0x200;
        }
    }
}
