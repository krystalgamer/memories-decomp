#define G_SDVALUE_VOLATILE
#include "../types.h"
#include "func_80049010.h"
#include "../psyq/libspu.h"

#include "sound.h"

extern void func_80046294(void);
extern void func_800471D0(s32, s32, s32, s32, s32, s32);

s32 func_80049138(s16 arg0, s32 arg1) {
    SDValue *p = g_SDValue;

    if (*p->music_track != arg0 >> 4) {
        SDValue *q;
        u8 *e;
        s32 off;

        func_80046294();
        func_80049010();
        SpuSetReverbModeType(SPU_REV_MODE_OFF);

        q = g_SDValue;
        q->flags_0040 |= 2;
        off = (arg0 & 0xFF0) >> 1;
        e = (u8 *)(off + (s32)q->field_1560);

        func_800471D0(*(s32 *)(e + 4), (s32)q->music_track, *(u16 *)(e + 2),
                      e[0] << 11, e[1] << 11, 0x20);

        *g_SDValue->music_track = 0xFFFF;
    }

    return 0;
}
