#include "../../types.h"

extern s32 D_8009B0CC;
extern u8 *gFreeDuel_pCursorWidget;
extern s32 rand(void);
extern void FreeDuel_UpdateScreen(void);
extern void FreeDuel_UpdateSparkle(void);

void FreeDuel_Entry(void)
{
    s32 phase;

    rand();
    FreeDuel_UpdateScreen();
    phase = D_8009B0CC & 0x7F;
    if (phase < 0x10) {
        if (phase >= 8) {
            phase = 0xF - phase;
        }
        *(s16 *)(gFreeDuel_pCursorWidget + 0x46) = phase * 48 + 0x1000;
        *(s16 *)(gFreeDuel_pCursorWidget + 0x44) = phase * 48 + 0x1000;
    }
    FreeDuel_UpdateSparkle();
}
