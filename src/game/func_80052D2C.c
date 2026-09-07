#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/memory.h"
#include "model.h"

extern u8 D_800F2B20[];
extern u8 D_800F3A10[];
extern u8 D_800F56F0[];
extern u8 D_8009AF98;

extern s32 func_8005F174(void);
extern s32 func_8005F18C(void);
extern void Model_UpdateViewMetrics(s32);

/* Starts a camera move. The transition record at D_800F2B20 is filled with a
 * start point taken from the live camera at D_800F56F0 and an end point taken
 * either from a model slot's +0xDD0 vector, when the slot argument is
 * non-zero, or from the start point itself; +0x01 records which of the two
 * halves - eye at +0x14, target at +0x24 - is slot-driven, and +0x0A holds
 * twice the absolute duration, clamped to 0xFFFE. A duration of zero means
 * "no move": the record's two halves are written straight back into the live
 * camera and Model_UpdateViewMetrics recomputes it. */
void func_80052D2C(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    SVECTOR a;
    SVECTOR b;
    SVECTOR t;
    s32 index;
    s32 state;
    s32 flags;

    memset(&b, 0, 8);
    b.vx = *(u16 *)(D_800F56F0 + 0);
    b.vy = *(u16 *)(D_800F56F0 + 4);
    b.vz = *(u16 *)(D_800F56F0 + 8);
    a = b;
    memset(&t, 0, 8);
    t.vx = *(u16 *)(D_800F56F0 + 0xC);
    t.vy = *(u16 *)(D_800F56F0 + 0x10);
    t.vz = *(u16 *)(D_800F56F0 + 0x14);
    b = t;
    state = func_8005F174();
    if (state == 1) {
        if (func_8005F18C() == state) {
            return;
        }
    }
    if (arg1 != 0 || arg0 <= 0) {
        if (arg1 > 0) {
            index = arg1 - 1;
            arg1 = (s32)(D_800F3A10 + index * MODEL_SLOT_SIZE);
            if (arg3 >= 0) {
                *(s16 *)(D_800F2B20 + 0x12) = index ^ 1;
            } else {
                *(s16 *)(D_800F2B20 + 0x12) = -1;
            }
            *(s16 *)(D_800F2B20 + 0x1A) = index;
        } else {
            u8 *p = D_800F2B20;

            *(s16 *)(p + 0x1A) = -1;
            *(s16 *)(p + 0x12) = -1;
        }
    }
    if (arg2 != 0 || arg0 <= 0) {
        if (arg2 > 0) {
            index = arg2 - 1;
            arg2 = (s32)(D_800F3A10 + index * MODEL_SLOT_SIZE);
            if (arg3 >= 0) {
                *(s16 *)(D_800F2B20 + 0x22) = index ^ 1;
            } else {
                *(s16 *)(D_800F2B20 + 0x22) = -1;
            }
            *(s16 *)(D_800F2B20 + 0x2A) = index;
        } else {
            u8 *p = D_800F2B20;

            *(s16 *)(p + 0x2A) = -1;
            *(s16 *)(p + 0x22) = -1;
        }
        if (arg0 < 0) {
            goto have_flags;
        }
    }
    D_800F2B20[0] = arg0;
have_flags:
    {
        u8 *p = D_800F2B20;

        *(s16 *)(p + 2) = 0x12C;
        *(s16 *)(p + 6) = 8;
        *(s16 *)(p + 4) = 8;
        p[1] = 0;
        if (arg1 != 0) {
            p[1] = 1;
        } else {
            arg1 = (s32)&a;
        }
    }
    if (arg2 != 0) {
        u8 *p = D_800F2B20;

        p[1] = p[1] | 2;
    } else {
        arg2 = (s32)&b;
    }
    {
        u8 *p = D_800F2B20;

        *(u16 *)(p + 0x0C) = a.vx;
        *(u16 *)(p + 0x0E) = a.vy;
        *(u16 *)(p + 0x10) = a.vz;
        *(u16 *)(p + 0x14) = *(u16 *)(arg1 + 0);
        *(u16 *)(p + 0x16) = *(u16 *)(arg1 + 2);
        *(u16 *)(p + 0x18) = *(u16 *)(arg1 + 4);
        *(u16 *)(p + 0x1C) = b.vx;
        *(u16 *)(p + 0x1E) = b.vy;
        *(u16 *)(p + 0x20) = b.vz;
        *(u16 *)(p + 0x24) = *(u16 *)(arg2 + 0);
        *(u16 *)(p + 0x26) = *(u16 *)(arg2 + 2);
        *(u16 *)(p + 0x28) = *(u16 *)(arg2 + 4);
        arg3 = (arg3 < 0 ? -arg3 : arg3) * 2;
        if (arg3 > 0xFFFE) {
            *(u16 *)(p + 0x0A) = 0xFFFF;
        } else {
            *(u16 *)(p + 0x0A) = arg3;
        }
    }
    {
        u8 *p = D_800F2B20;

        flags = p[1];
        *(u16 *)(p + 8) = 0;
        if (flags != 0) {
            D_8009AF98 = 0;
        }
        if (arg3 != 0) {
            return;
        }
        if (flags & 1) {
            *(s32 *)(D_800F56F0 + 0) = *(s16 *)(p + 0x14);
            *(s32 *)(D_800F56F0 + 4) = *(s16 *)(p + 0x16);
            *(s32 *)(D_800F56F0 + 8) = *(s16 *)(p + 0x18);
        }
        if (p[1] & 2) {
            s32 *q = (s32 *)D_800F56F0;

            q[3] = *(s16 *)(p + 0x24);
            q[4] = *(s16 *)(p + 0x26);
            q[5] = *(s16 *)(p + 0x28);
        }
        if (p[1] & 3) {
            Model_UpdateViewMetrics(0);
        }
        p[1] = 0;
    }
}
