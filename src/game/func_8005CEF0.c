#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_packet_handlers.h"

/* Advances one animation track: interpolates the translation (+0x18) and
 * the three rotation angles (+0x44) between the track's two keyframes over
 * its duration, wrapping angles across the full-turn boundary on 16-tick
 * tracks, rebuilds the matrix through RotMatrixYXZ_gte, and mirrors the six
 * results into the track's optional output record. Returns 1 when the
 * track has no duration. */
u8 *func_8005CEF0(u8 **arg0) {
    u8 *s1;
    u8 *fr;
    GsCOORDUNIT *dst;
    u8 *out;
    s32 t;
    s32 dur;
    u32 w;
    u32 n;
    s32 v;
    u16 a0;
    u16 a1;
    u16 a2;
    u16 b0;
    u16 b1;
    u16 b2;
    s1 = ((u8 *)arg0) + (*(s32 *)(((u8 *)arg0) + 0x14) * 4 + 0x14);
    fr = *(u8 **)s1;
    if (*(u16 *)(fr + 0x12) == 0) {
        return (u8 *)1;
    }
    t = *(s16 *)(fr + 0x10);
    dur = *(u16 *)(fr + 0x12);
    w = *(s32 *)fr;
    n = w >> 24;
    w &= 0xFFFFFF;
    dst = (GsCOORDUNIT *)(*(u8 **)(((u8 *)arg0) + n * 4 + 0x14) + w * 4);
    dst->matrix.t[0] = (*(s16 *)((*(u8 **)(s1 + 4)) + 0) * t + *(s16 *)((*(u8 **)(s1 + 8)) + 0) * (dur - t)) / dur;
    dst->matrix.t[1] = (*(s16 *)((*(u8 **)(s1 + 4)) + 2) * t + *(s16 *)((*(u8 **)(s1 + 8)) + 2) * (dur - t)) / dur;
    dst->matrix.t[2] = (*(s16 *)((*(u8 **)(s1 + 4)) + 4) * t + *(s16 *)((*(u8 **)(s1 + 8)) + 4) * (dur - t)) / dur;
    b0 = *(u16 *)((*(u8 **)(s1 + 8)) + 6);
    b1 = *(u16 *)((*(u8 **)(s1 + 8)) + 8);
    b2 = *(u16 *)((*(u8 **)(s1 + 8)) + 0xA);
    a0 = *(u16 *)((*(u8 **)(s1 + 4)) + 6);
    a1 = *(u16 *)((*(u8 **)(s1 + 4)) + 8);
    a2 = *(u16 *)((*(u8 **)(s1 + 4)) + 0xA);
    if (dur == 16) {
        if ((s16)a0 - (s16)b0 < 0) {
            goto n0;
        }
        if ((s16)a0 - (s16)b0 >= MODEL_ANGLE_WRAP_THRESHOLD) {
            goto w0;
        }
        goto d0;
    n0:
        if ((s16)b0 - (s16)a0 < MODEL_ANGLE_WRAP_THRESHOLD) {
            goto d0;
        }
    w0:
        v = (s16)b0;
        if (v > 0) {
            v = v - MODEL_ANGLE_FULL_TURN;
        } else {
            v = v + MODEL_ANGLE_FULL_TURN;
        }
        b0 = v;
    d0:
        ;
        if ((s16)a1 - (s16)b1 < 0) {
            goto n1;
        }
        if ((s16)a1 - (s16)b1 >= MODEL_ANGLE_WRAP_THRESHOLD) {
            goto w1;
        }
        goto d1;
    n1:
        if ((s16)b1 - (s16)a1 < MODEL_ANGLE_WRAP_THRESHOLD) {
            goto d1;
        }
    w1:
        v = (s16)b1;
        if (v > 0) {
            v = v - MODEL_ANGLE_FULL_TURN;
        } else {
            v = v + MODEL_ANGLE_FULL_TURN;
        }
        b1 = v;
    d1:
        ;
        if ((s16)a2 - (s16)b2 < 0) {
            goto n2;
        }
        if ((s16)a2 - (s16)b2 >= MODEL_ANGLE_WRAP_THRESHOLD) {
            goto w2;
        }
        goto d2;
    n2:
        if ((s16)b2 - (s16)a2 < MODEL_ANGLE_WRAP_THRESHOLD) {
            goto d2;
        }
    w2:
        v = (s16)b2;
        if (v > 0) {
            v = v - MODEL_ANGLE_FULL_TURN;
        } else {
            v = v + MODEL_ANGLE_FULL_TURN;
        }
        b2 = v;
    d2:
        ;
    }
    dst->rot.vx = ((s16)a0 * t + (s16)b0 * (dur - t)) / dur;
    dst->rot.vy = ((s16)a1 * t + (s16)b1 * (dur - t)) / dur;
    dst->rot.vz = ((s16)a2 * t + (s16)b2 * (dur - t)) / dur;
    RotMatrixYXZ_gte(&dst->rot, &dst->matrix);
    dst->flg = 0;
    out = *(u8 **)(s1 + 0xC);
    if (out != 0) {
        *(u16 *)(*(u8 **)(s1 + 0xC) + 0x0) = *(u16 *)&dst->matrix.t[0];
        *(u16 *)(*(u8 **)(s1 + 0xC) + 0x2) = *(u16 *)&dst->matrix.t[1];
        *(u16 *)(*(u8 **)(s1 + 0xC) + 0x4) = *(u16 *)&dst->matrix.t[2];
        *(u16 *)(*(u8 **)(s1 + 0xC) + 0x6) = *(u16 *)&dst->rot.vx;
        *(u16 *)(*(u8 **)(s1 + 0xC) + 0x8) = *(u16 *)&dst->rot.vy;
        *(u16 *)(*(u8 **)(s1 + 0xC) + 0xA) = *(u16 *)&dst->rot.vz;
    }
    return (u8 *)0;
}
