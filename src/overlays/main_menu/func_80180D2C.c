#include "../../types.h"

extern u8 *gMain_apMenuEntries[];
extern u8 D_80184596;
extern u8 D_80184599;

void func_80180D2C(s32 mode)
{
    s32 i;
    s32 offset;

    for (i = 0; i < 0xB; i++) {
        if (i & 1) {
            offset = 0x1E0;
        } else {
            offset = -0xA0;
        }
        if (gMain_apMenuEntries[i] != 0) {
            if (mode != 0) {
                *(s16 *)(gMain_apMenuEntries[i] + 0x36) = 0xA0;
                *(s16 *)(gMain_apMenuEntries[i] + 0x38) = offset;
            } else {
                *(s16 *)(gMain_apMenuEntries[i] + 0x36) = offset;
                *(s16 *)(gMain_apMenuEntries[i] + 0x38) = 0xA0;
            }
            *(s16 *)(gMain_apMenuEntries[i] + 0x30) = *(u16 *)(gMain_apMenuEntries[i] + 0x36);
            *(s16 *)(gMain_apMenuEntries[i] + 0x60) = 0x10;
        }
    }
    D_80184596 = mode;
    D_80184599 = 1;
}
