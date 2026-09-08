#include "../types.h"
#include "../psyq/libspu.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"

extern u8 *g_SDValue;

void func_80047480(void) {
    u8 *p;
    u8 *q;
    u8 *r;
    s32 i;
    s32 j;
    s32 k;

    g_SDValue[0x434] = 0;
    g_SDValue[0x435] = 0;

    for (i = 0; i < 4; i++) {
        (g_SDValue + i)[0x40C] = 0;
        (g_SDValue + i)[0x410] = 0;
        *(s16 *)&(g_SDValue + (i + i))[0x404] = 0;
        *(s16 *)&(g_SDValue + (i + i))[0x414] = 0;
        *(s16 *)&(g_SDValue + (i + i))[0x41C] = 0;
        (g_SDValue + i)[0x428] = 0;
        *(s16 *)&(g_SDValue + (i + i))[0x42C] = 0;
    }

    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

    p = g_SDValue;
    *(s16 *)(p + 0x394) = 0x3FFF;
    *(s16 *)(p + 0x396) = 0x3FFF;
    *(s16 *)(p + 0x398) = 0x1000;
    *(s16 *)(p + 0x39C) = 0x3C00;
    *(s32 *)(p + 0x3A8) = 1;
    *(s32 *)(p + 0x3AC) = 1;
    *(s32 *)(p + 0x3B0) = 3;
    *(s32 *)(p + 0x448) = 0x801E2800;
    *(s32 *)(p + 0x388) = 0xFFFF;
    *(s16 *)(p + 0x390) = 0;
    *(s16 *)(p + 0x392) = 0;
    *(s16 *)(p + 0x3B4) = 0;
    *(s16 *)(p + 0x3B6) = 0;
    *(s16 *)(p + 0x3B8) = 0;
    *(s16 *)(p + 0x3BA) = 0;
    *(s16 *)(p + 0x3BC) = 0;
    *(s32 *)(p + 0x43C) = 0x801E4000;
    *(s32 *)(p + 0x438) = 0x1010;
    *(s16 *)(p + 0x440) = 0;
    *(u16 *)(p + 0x442) = 0xFFFF;
    *(s32 *)(p + 0x444) =
        ((*(u16 *)p * 2 + 0xF) & 0xFFF0) + 0x801E4000;

    i = 0;
    if (*(u16 *)p != 0) {
        r = p;
        do {
            *(u16 *)(*(u8 **)(r + 0x43C) + i * 2) = SD_PENDING_ENTRY_NONE;
            i++;
        } while (i < *(u16 *)r);
    }

    i = 0;
    q = g_SDValue;
    for (; i < SD_VOICE_LOOKUP_BANK_COUNT; i++) {
        j = 0;
        k = i << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        for (; j < SD_VOICE_LOOKUP_BANK_ENTRY_COUNT; j++) {
            *(u16 *)(q + k + SD_VOICE_LOOKUP_BYTE_OFFSET) = SD_PENDING_ENTRY_NONE;
            k += 2;
        }
    }
}
