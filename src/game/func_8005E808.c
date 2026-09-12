#include "../types.h"
#include "camera_view.h"
#include "model_copy_slot_u16_values.h"
#include "func_80058DD8.h"
#include "func_8005EBF4.h"
#include "model_transfer_state.h"
#include "model_transfer_flags.h"

/* Timing/state tail of the shared 0x28-byte keyframe record. */
typedef struct {
    u8 pad_00[0x20];
    s16 field_20;
    u16 field_22;
    s16 field_24;
    u8 field_26;
    u8 pad_27;
} ModelKeyframeTimingView;

typedef char ModelKeyframeTimingView_size_must_be_0x28[
    sizeof(ModelKeyframeTimingView) == 0x28 ? 1 : -1
];

void func_8005E808(u8 *p)
{
    ModelKeyframeTimingView *state = (ModelKeyframeTimingView *)p;
    s16 pos[3];
    s16 buf[10][3];
    u8 *e;
    u8 *q;
    u8 *g;
    s32 i;
    s32 j;
    s32 off;
    s32 a;
    s32 k;
    s32 sum;
    u32 d;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 slot;
    u8 *prev;
    u16 v;

    {
        s32 n;
        s32 t;
        n = state->field_20;
        a = __builtin_abs(n);
        t = a;
        if (a <= 0) {
            t = 1;
        }
        k = t * 2;
    }
    func_8005FB30((u8 *)state);
    state->field_24 = 0;
    if (D_8009B074[0x26] != 0) {
        return;
    }
    state->field_26 = 1;
    if (a >= 0x4000) {
        state->field_22 = 0x4000;
    } else {
        state->field_22 = k;
    }
    if (state->field_20 >= 0) {
        return;
    }

    e = (u8 *)state;
    for (i = 0; i < 2; i++, e += 8) {
        q = e + 2;
        g = (u8 *)&D_800F5768[i];
        switch (*(s16 *)(q + 4)) {
        case 0x80:
        case 0x81:
        {
            s32 dx, dy, dz;
            slot = (s16)(*(u16 *)(q + 4) & 0xFF7F);
            if (func_80058DD8(slot) != 1) {
                continue;
            }
            Model_CopySlotU16Values(slot, pos);
            pos[0] = pos[0] + *(u16 *)e;
            pos[1] = pos[1] + *(u16 *)q;
            pos[2] = pos[2] + *(u16 *)(q + 2);
            dx = pos[0] - *(s16 *)g;
            dy = pos[1] - *(s16 *)(g + 2);
            dz = pos[2] - *(s16 *)(g + 4);
            d = k * SquareRoot0(dx * dx + dy * dy + dz * dz) / 1000;
            if (state->field_22 < d) {
                state->field_22 = d;
            }
            continue;
        }
        case 1:
            sum = 0;
            for (j = 0; j < 10; j++) {
                func_8005EBF4((Key *)state, i, j, 0x1E, buf[j]);
            }
            j = 1;
            off = 0;
            for (; j < 10; j++) {
                prev = (u8 *)buf + off;
                dx = *(s16 *)prev - buf[j][0];
                dy = *(s16 *)(prev + 2) - buf[j][1];
                dz = *(s16 *)(prev + 4) - buf[j][2];
                sum += SquareRoot0(dx * dx + dy * dy + dz * dz);
                off += 6;
            }
            d = k * sum / 1000;
            if (state->field_22 < d) {
                state->field_22 = d;
            }
            continue;
        case 4:
        {
            s32 radius_scaled;
            s32 factor;
            v = (u16)D_800F5768[2].vx;
            if (D_800F5768[2].vx < *(s16 *)e) {
                v = *(u16 *)e;
            }
            radius_scaled = (s16)v * 6284 / 1000;
            d = k * radius_scaled;
            factor = __builtin_abs(*(s16 *)q);
            d = (u32)(d * factor) / 4096000;
            if (state->field_22 < d) {
                state->field_22 = d;
            }
            continue;
        }
        default:
            continue;
        }
    }
}
