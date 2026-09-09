#include "../types.h"
#include "display_object_lifecycle.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "sound.h"
#include "display_object_api.h"
#include "display_object_helpers.h"

void func_800313E8(u8 *p) {
    s32 v;
    s32 t;

    if (func_80042B98((DisplayObjectLifecycle *)p) == 0) {
        *(s32 *)(p + 0x44) = 0;
        *(s16 *)(p + 0x60) = 0;
        *(s32 *)(p + 4) = *(s32 *)(p + 4) & ~GsROTOFF;
        p[0x6C] = p[0x6C] | 0x40;
        SD_SEPlayFull(0xA);
    }

    v = p[0x6C];

    if ((v & 0x40) != 0) {
        t = *(u16 *)(p + 0x60) + 1;
        *(s16 *)(p + 0x60) = t;
        *(s16 *)(p + 0x46) = (s16)t * 512;
        *(s16 *)(p + 0x44) = (s16)t * 512;
        if (*(s16 *)(p + 0x60) >= 8) {
            DisplayObject_ResetVelocity(p);
            *(s16 *)(p + 0x36) = ((*(s16 *)(p + 0x18) - *(s16 *)(p + 0x30)) << 8) / 12;
            p[0x6C] = p[0x6C] & 0xBF;
            *(s16 *)(p + 0x60) = 0xC;
            *(s16 *)(p + 0x38) = ((*(s16 *)(p + 0x1A) - *(s16 *)(p + 0x32)) << 8) / 12;
        }
    } else if ((v & 0x20) != 0) {
        t = *(u16 *)(p + 0x60) - 1;
        *(s16 *)(p + 0x60) = t;
        *(s16 *)(p + 0x46) = (s16)t * 512;
        *(s16 *)(p + 0x44) = (s16)t * 512;
        if (*(s16 *)(p + 0x60) == 0) {
            func_8004036C(p);
        }
    } else {
        DisplayObject_StepPositionXY(p);
        t = *(u16 *)(p + 0x60) - 1;
        *(s16 *)(p + 0x60) = t;
        if (*(s16 *)(p + 0x60) <= 0) {
            *(s32 *)(p + 0x30) = *(s32 *)(p + 0x18);
            *(s16 *)(p + 0x60) = 8;
            p[0x6C] = p[0x6C] | 0x20;
        }
    }
}
