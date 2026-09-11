#include "../types.h"
#include "../psyq/libspu.h"
#include "func_8004A7C0.h"
#include "sound_sequence_constants.h"
#define D_8009B458_IN_DATA
#include "sound.h"

void func_8004B374(s32 arg0, s32 arg1, s32 unused)
{
    u8 *base;
    s32 mask;
    s32 i;
    s32 off;
    s32 *tbl;
    s32 v0;
    s32 t1;
    u8 *p;
    s32 a1;

    (void)unused;
    a1 = arg1;
    base = (u8 *)D_8009B458;
    i = 0;
    mask = 0;
    if (*(s16 *)(base + 0x510) > 0) {
        t1 = arg0 & 0xFF;
        tbl = D_80011434;
        off = 0;
    top:
        p = base + off;
        if (p[0x183] == t1 && p[0x185] == (u8)a1) {
            func_8004A7C0(i);
            mask |= *tbl;
        }
        tbl++;
        do {
            off += 0x28;
            base = (u8 *)D_8009B458;
            i++;
            if (i < *(s16 *)(base + 0x510)) {
                goto top;
            }
        } while (0);
    }

    if (mask != 0) {
        do {
            SpuSetKey(SPU_OFF, mask);
            v0 = SpuGetKeyStatus(mask);
        } while (v0 != SPU_OFF_ENV_ON && v0 != SPU_OFF);
    }

    {
        u8 *q =
            (u8 *)D_8009B458 + ((u8)arg0) * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
        q[4] = 0;
    }
}
