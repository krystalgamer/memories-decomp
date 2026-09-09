#include "../types.h"
#include "model_transfer_flags.h"
#include "model_update_view_metrics.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "../psyq/memory.h"
#include "model.h"

extern u8 D_800F3A10[];
extern u8 D_8009AF98;

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
    b.vx = *(u16 *)&D_800F56F0.vpx;
    b.vy = *(u16 *)&D_800F56F0.vpy;
    b.vz = *(u16 *)&D_800F56F0.vpz;
    a = b;
    memset(&t, 0, 8);
    t.vx = *(u16 *)&D_800F56F0.vrx;
    t.vy = *(u16 *)&D_800F56F0.vry;
    t.vz = *(u16 *)&D_800F56F0.vrz;
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
                D_800F2B20.eye.pair_slot = index ^ 1;
            } else {
                D_800F2B20.eye.pair_slot = -1;
            }
            D_800F2B20.eye.slot = index;
        } else {
            ModelCameraMove *p = &D_800F2B20;

            p->eye.slot = -1;
            p->eye.pair_slot = -1;
        }
    }
    if (arg2 != 0 || arg0 <= 0) {
        if (arg2 > 0) {
            index = arg2 - 1;
            arg2 = (s32)(D_800F3A10 + index * MODEL_SLOT_SIZE);
            if (arg3 >= 0) {
                D_800F2B20.target.pair_slot = index ^ 1;
            } else {
                D_800F2B20.target.pair_slot = -1;
            }
            D_800F2B20.target.slot = index;
        } else {
            ModelCameraMove *p = &D_800F2B20;

            p->target.slot = -1;
            p->target.pair_slot = -1;
        }
        if (arg0 < 0) {
            goto have_flags;
        }
    }
    D_800F2B20.mode = arg0;
have_flags:
    {
        ModelCameraMove *p = &D_800F2B20;

        p->field_02 = 0x12C;
        p->field_06 = 8;
        p->field_04 = 8;
        p->flags = 0;
        if (arg1 != 0) {
            p->flags = 1;
        } else {
            arg1 = (s32)&a;
        }
    }
    if (arg2 != 0) {
        ModelCameraMove *p = &D_800F2B20;

        p->flags = p->flags | 2;
    } else {
        arg2 = (s32)&b;
    }
    {
        ModelCameraMove *p = &D_800F2B20;

        p->eye.start_x = a.vx;
        p->eye.start_y = a.vy;
        p->eye.start_z = a.vz;
        p->eye.end_x = *(u16 *)(arg1 + 0);
        p->eye.end_y = *(u16 *)(arg1 + 2);
        p->eye.end_z = *(u16 *)(arg1 + 4);
        p->target.start_x = b.vx;
        p->target.start_y = b.vy;
        p->target.start_z = b.vz;
        p->target.end_x = *(u16 *)(arg2 + 0);
        p->target.end_y = *(u16 *)(arg2 + 2);
        p->target.end_z = *(u16 *)(arg2 + 4);
        arg3 = (arg3 < 0 ? -arg3 : arg3) * 2;
        if (arg3 > 0xFFFE) {
            p->duration = 0xFFFF;
        } else {
            p->duration = arg3;
        }
    }
    {
        ModelCameraMove *p = &D_800F2B20;

        flags = p->flags;
        p->elapsed = 0;
        if (flags != 0) {
            D_8009AF98 = 0;
        }
        if (arg3 != 0) {
            return;
        }
        /* These three reads stay in byte-address form on purpose. Written as
         * `p->eye.end_x` the struct type lets gcc prove they cannot alias the
         * `u8 []` at D_800F56F0, so it hoists the first load into the delay
         * slot of the store above and the function comes out one instruction
         * short of retail. The cast keeps the dependency the original had.
         * Typing D_800F56F0 is the follow-up that would let this read as a
         * member again. */
        if (flags & 1) {
            u8 *raw = (u8 *)p;

            D_800F56F0.vpx = *(s16 *)(raw + 0x14);
            D_800F56F0.vpy = *(s16 *)(raw + 0x16);
            D_800F56F0.vpz = *(s16 *)(raw + 0x18);
        }
        if (p->flags & 2) {
            s32 *q = (s32 *)&D_800F56F0;
            u8 *raw = (u8 *)p;

            q[3] = *(s16 *)(raw + 0x24);
            q[4] = *(s16 *)(raw + 0x26);
            q[5] = *(s16 *)(raw + 0x28);
        }
        if (p->flags & 3) {
            Model_UpdateViewMetrics(0);
        }
        p->flags = 0;
    }
}
