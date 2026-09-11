#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../game/model.h"
#include "../game/model_packet_handlers.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u16 rx;
    u16 ry;
    u16 rz;
    s16 sx;
    s16 sy;
    s16 sz;
} AnimSample;

/*
 * Current best under gcc_2_8_1_g8_split: 396 instructions against 391,
 * with 263 differing positions and 362 target instructions aligned. This is
 * not a near match. Materializing the slot base, re-reading sample/publish
 * pointers per component, loading the six angles together, wrapping through
 * a temporary, and publishing all nine values recover the present structure.
 * Residual: the three angle-wrap blocks still compile with the wrong branch
 * shape and cross-jump behavior, leaving the candidate five instructions long.
 */
s32 func_8005D378(GsARGUNIT_ANIM *ctx)
{
    GsSEQ *track;
    GsCOORDUNIT *node;
    s16 *out;
    u32 **slot;
    s32 t;
    s32 dur;
    s32 rest;
    s32 rest2;
    s32 a;
    s32 b;
    s32 rx;
    s32 ry;
    s32 rz;
    s32 w;
    s32 c;
    s32 scale[3];

    slot = (u32 **)(&ctx->header_size + ctx->header_size);
    track = (GsSEQ *)slot[0];
    if (track->tframe == 0) {
        return 1;
    }

    t = track->rframe;
    dur = track->tframe;
    rest = dur - t;

    node = (GsCOORDUNIT *)(*(u32 *)((u8 *)ctx + ((track->rewrite_idx >> 24) << 2) + 0x14)
                        + ((track->rewrite_idx & 0xFFFFFF) << 2));

    node->matrix.t[0] = (((AnimSample *)slot[1])->x * t + ((AnimSample *)slot[2])->x * rest) / dur;
    node->matrix.t[1] = (((AnimSample *)slot[1])->y * t + ((AnimSample *)slot[2])->y * rest) / dur;
    node->matrix.t[2] = (((AnimSample *)slot[1])->z * t + ((AnimSample *)slot[2])->z * rest) / dur;

    rx = ((AnimSample *)slot[2])->rx;
    ry = ((AnimSample *)slot[2])->ry;
    rz = ((AnimSample *)slot[2])->rz;
    a = ((AnimSample *)slot[1])->rx;
    b = ((AnimSample *)slot[1])->ry;
    c = ((AnimSample *)slot[1])->rz;

    if (dur == 0x10) {
        s32 d;

        d = (s16)a - (s16)rx;
        if (d < 0) {
            if ((s16)rx - (s16)a >= MODEL_ANGLE_WRAP_THRESHOLD) {
                w = (s16)rx;
                if (w <= 0) {
                    w = w + MODEL_ANGLE_FULL_TURN;
                } else {
                    w = w - MODEL_ANGLE_FULL_TURN;
                }
                rx = w;
            }
        } else if (d >= MODEL_ANGLE_WRAP_THRESHOLD) {
            w = (s16)rx;
            if (w <= 0) {
                w = w + MODEL_ANGLE_FULL_TURN;
            } else {
                w = w - MODEL_ANGLE_FULL_TURN;
            }
            rx = w;
        }

        d = (s16)b - (s16)ry;
        if (d < 0) {
            if ((s16)ry - (s16)b >= MODEL_ANGLE_WRAP_THRESHOLD) {
                w = (s16)ry;
                if (w <= 0) {
                    w = w + MODEL_ANGLE_FULL_TURN;
                } else {
                    w = w - MODEL_ANGLE_FULL_TURN;
                }
                ry = w;
            }
        } else if (d >= MODEL_ANGLE_WRAP_THRESHOLD) {
            w = (s16)ry;
            if (w <= 0) {
                w = w + MODEL_ANGLE_FULL_TURN;
            } else {
                w = w - MODEL_ANGLE_FULL_TURN;
            }
            ry = w;
        }

        d = (s16)c - (s16)rz;
        if (d < 0) {
            if ((s16)rz - (s16)c >= MODEL_ANGLE_WRAP_THRESHOLD) {
                w = (s16)rz;
                if (w <= 0) {
                    w = w + MODEL_ANGLE_FULL_TURN;
                } else {
                    w = w - MODEL_ANGLE_FULL_TURN;
                }
                rz = w;
            }
        } else if (d >= MODEL_ANGLE_WRAP_THRESHOLD) {
            w = (s16)rz;
            if (w <= 0) {
                w = w + MODEL_ANGLE_FULL_TURN;
            } else {
                w = w - MODEL_ANGLE_FULL_TURN;
            }
            rz = w;
        }
    }

    rest2 = dur - t;
    node->rot.vx = ((s16)a * t + (s16)rx * rest2) / dur;
    node->rot.vy = ((s16)b * t + (s16)ry * rest2) / dur;
    node->rot.vz = ((s16)c * t + (s16)rz * rest2) / dur;

    out = &node->rot.vx;
    RotMatrixYXZ_gte((SVECTOR *)out, &node->matrix);

    scale[0] = (((AnimSample *)slot[1])->sx * t + ((AnimSample *)slot[2])->sx * rest2) / dur;
    scale[1] = (((AnimSample *)slot[1])->sy * t + ((AnimSample *)slot[2])->sy * rest2) / dur;
    scale[2] = (((AnimSample *)slot[1])->sz * t + ((AnimSample *)slot[2])->sz * rest2) / dur;
    ScaleMatrix(&node->matrix, (VECTOR *)scale);

    node->flg = 0;
    if (slot[3] == 0) {
        return 0;
    }
    ((u16 *)slot[3])[0] = *(u16 *)&node->matrix.t[0];
    ((u16 *)slot[3])[1] = *(u16 *)&node->matrix.t[1];
    ((u16 *)slot[3])[2] = *(u16 *)&node->matrix.t[2];
    ((u16 *)slot[3])[3] = *(u16 *)&node->rot.vx;
    ((u16 *)slot[3])[4] = *(u16 *)&node->rot.vy;
    ((u16 *)slot[3])[5] = *(u16 *)&node->rot.vz;
    ((u16 *)slot[3])[6] = *(u16 *)&scale[0];
    ((u16 *)slot[3])[7] = *(u16 *)&scale[1];
    ((u16 *)slot[3])[8] = *(u16 *)&scale[2];
    return 0;
}
