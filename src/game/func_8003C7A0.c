#include "../types.h"

extern u8 D_8009B37C;
extern s8 D_8009B37D;
extern s8 D_8009B384;
/* Retail performs a fresh absolute load for each input-state test. */
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
/* Keep this byte outside small data so its store retains absolute addressing. */
extern u8 D_8009B408[9];

extern void SD_SetOutputType(s32);
extern void SD_SEPlayFull(s32);
extern void func_8003C568(s32);

void func_8003C7A0(void)
{
    s32 value;

    if (D_8009B384 == 0 && (gInput_wPad1Pressed & 0xA000)) {
        if (gInput_wPad1Pressed & 0x2000) {
            if (D_8009B37D != 0) {
                return;
            }
            value = 1;
            D_8009B408[0] = value;
            D_8009B37D = 1;
        } else {
            if (D_8009B37D == 0) {
                return;
            }
            value = 0;
            D_8009B408[0] = value;
            D_8009B37D = 0;
        }
        SD_SetOutputType(value);
        SD_SEPlayFull(0x2F);
        func_8003C568(D_8009B384);
        return;
    }

    if (D_8009B384 != 0 && (gInput_wPad1Pressed & 0xC0)) {
        SD_SEPlayFull(7);
        D_8009B37C = *(u8 *)&D_8009B384 + 1;
        return;
    }

    if (gInput_wPad1Pressed & 0x20) {
        D_8009B37C = 0;
        SD_SEPlayFull(8);
    }
}
