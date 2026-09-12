#include "../types.h"
#define D_8009B458_IN_DATA
#include "sound.h"
#include "func_80049FB4.h"
#include "../psyq/libspu.h"
#include "sound_sequence_constants.h"
#include "sound_spatialization.h"

void func_8004A518(void)
{
    u8 *base;
    u8 *r1;
    u8 *r3;
    s32 mask;
    s32 i;
    s32 o1;
    s32 off;
    s32 *tbl;
    s32 key;
    s32 k40;
    s32 c72;
    s32 cff;
    s32 one;
    s32 o18;
    u8 b40;
    u8 b7f;
    s32 w7f;

    mask = 0;
    i = mask;
    c72 = 0x72;
    one = 1;
    cff = 0xFF;
    o1 = 0x518;
    base = (u8 *)D_8009B458;
    *(s16 *)(base + 0x512) = 0x7F;
    do {
        r1 = (u8 *)D_8009B458 + o1;
        i++;
        *(s32 *)(r1 + 0) = 0;
        *(s32 *)(r1 + 4) = 0;
        r1[0x26] = 0;
        *(s32 *)(r1 + 0x1C) = 0;
        *(s32 *)(r1 + 8) = 0;
        *(s32 *)(r1 + 0xC) = 0;
        *(s16 *)(r1 + 0x14) = c72;
        *(s16 *)(r1 + 0x16) = c72;
        r1[0x24] = one;
        *(s32 *)(r1 + 0x10) = 0;
        *(s16 *)(r1 + 0x18) = 0;
        r1[0x27] = 0;
        r1[0x28] = cff;
        r1[0x29] = 0;
        r1[0x2B] = 0;
        o1 += SD_SEQUENCE_TRACK_RECORD_SIZE;
    } while (i < SD_SEQUENCE_TRACK_COUNT);

    do {
    base = (u8 *)D_8009B458;
    if (*(s16 *)(base + 0x510) > 0) {
        i = 0;
        k40 = 0x40;
        tbl = D_80011434;
        off = 0x180;
    top2:
            key = *tbl;
            (base + off)[3] = SD_SECONDARY_RECORD_NONE;
            (base + off)[0] = i;
            (base + off)[0xD] = 0;
            (base + off)[0xF] = 0;
            (base + off)[0xC] = 0x40;
            *(s16 *)(base + off + 0x1A) = k40;
            *(s16 *)(base + off + 0x1C) = k40;
            *(s16 *)(base + off + 0x1E) = 0;
            func_8004A764(i);
            SpuSetKey(SPU_OFF, key);
            tbl++;
            off += SD_SECONDARY_OBJECT_SIZE;
            base = (u8 *)D_8009B458;
            i++;
            mask |= key;
        if (i < *(s16 *)(base + 0x510)) goto top2;
    }
    } while (0);

    i = 0;
    b40 = 0x40;
    b7f = 0x7F;
    w7f = 0x7F;
    o18 = i;
    do {
        r3 = (u8 *)D_8009B458 + o18;
        i++;
        r3[1] = b40;
        r3[3] = b7f;
        r3[4] = 0;
        r3[5] = b7f;
        r3[7] = b40;
        *(s32 *)(r3 + 8) = w7f;
        *(s32 *)(r3 + 0xC) = w7f;
        r3[0x10] = 0;
        *(s16 *)(r3 + 0x14) = 0;
        r3[6] = 0;
        o18 += SD_SEQUENCE_CHANNEL_RECORD_SIZE;
    } while (i < SD_SEQUENCE_CHANNEL_COUNT);

    SpuSetKey(SPU_OFF, mask);
}
