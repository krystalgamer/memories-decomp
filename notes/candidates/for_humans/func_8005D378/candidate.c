#include "../../../../src/types.h"

#define MODEL_ANGLE_FULL_TURN 0x1000
#define MODEL_ANGLE_HALF_TURN 0x801

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

typedef struct {
    u32 target;
    u8 pad4[0x10 - 0x4];
    s16 time;
    u16 duration;
} AnimTrack;

typedef struct {
    u32 field_0;
    u8 matrix[0x18 - 0x4];
    s32 x;
    s32 y;
    s32 z;
    u8 pad24[0x44 - 0x24];
    s16 rx;
    s16 ry;
    s16 rz;
} AnimNode;

typedef struct {
    u8 pad0[0x14];
    u32 slots[1];
} AnimContext;

extern void func_80089060(s16 *, void *);
extern void ScaleMatrix(void *, s32 *);

s32 func_8005D378(AnimContext *ctx)
{
    AnimTrack *track;
    AnimNode *node;
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

    slot = (u32 **)((u8 *)ctx + ((ctx->slots[0] << 2) + 0x14));
    track = (AnimTrack *)slot[0];
    if (track->duration == 0) {
        return 1;
    }

    t = track->time;
    dur = track->duration;
    rest = dur - t;

    node = (AnimNode *)(*(u32 *)((u8 *)ctx + ((track->target >> 24) << 2) + 0x14)
                        + ((track->target & 0xFFFFFF) << 2));

    node->x = (((AnimSample *)slot[1])->x * t + ((AnimSample *)slot[2])->x * rest) / dur;
    node->y = (((AnimSample *)slot[1])->y * t + ((AnimSample *)slot[2])->y * rest) / dur;
    node->z = (((AnimSample *)slot[1])->z * t + ((AnimSample *)slot[2])->z * rest) / dur;

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
            if ((s16)rx - (s16)a >= MODEL_ANGLE_HALF_TURN) {
                w = (s16)rx;
                if (w <= 0) {
                    w = w + MODEL_ANGLE_FULL_TURN;
                } else {
                    w = w - MODEL_ANGLE_FULL_TURN;
                }
                rx = w;
            }
        } else if (d >= MODEL_ANGLE_HALF_TURN) {
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
            if ((s16)ry - (s16)b >= MODEL_ANGLE_HALF_TURN) {
                w = (s16)ry;
                if (w <= 0) {
                    w = w + MODEL_ANGLE_FULL_TURN;
                } else {
                    w = w - MODEL_ANGLE_FULL_TURN;
                }
                ry = w;
            }
        } else if (d >= MODEL_ANGLE_HALF_TURN) {
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
            if ((s16)rz - (s16)c >= MODEL_ANGLE_HALF_TURN) {
                w = (s16)rz;
                if (w <= 0) {
                    w = w + MODEL_ANGLE_FULL_TURN;
                } else {
                    w = w - MODEL_ANGLE_FULL_TURN;
                }
                rz = w;
            }
        } else if (d >= MODEL_ANGLE_HALF_TURN) {
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
    node->rx = ((s16)a * t + (s16)rx * rest2) / dur;
    node->ry = ((s16)b * t + (s16)ry * rest2) / dur;
    node->rz = ((s16)c * t + (s16)rz * rest2) / dur;

    out = &node->rx;
    func_80089060(out, node->matrix);

    scale[0] = (((AnimSample *)slot[1])->sx * t + ((AnimSample *)slot[2])->sx * rest2) / dur;
    scale[1] = (((AnimSample *)slot[1])->sy * t + ((AnimSample *)slot[2])->sy * rest2) / dur;
    scale[2] = (((AnimSample *)slot[1])->sz * t + ((AnimSample *)slot[2])->sz * rest2) / dur;
    ScaleMatrix(node->matrix, scale);

    node->field_0 = 0;
    if (slot[3] == 0) {
        return 0;
    }
    ((u16 *)slot[3])[0] = *(u16 *)&node->x;
    ((u16 *)slot[3])[1] = *(u16 *)&node->y;
    ((u16 *)slot[3])[2] = *(u16 *)&node->z;
    ((u16 *)slot[3])[3] = *(u16 *)&node->rx;
    ((u16 *)slot[3])[4] = *(u16 *)&node->ry;
    ((u16 *)slot[3])[5] = *(u16 *)&node->rz;
    ((u16 *)slot[3])[6] = *(u16 *)&scale[0];
    ((u16 *)slot[3])[7] = *(u16 *)&scale[1];
    ((u16 *)slot[3])[8] = *(u16 *)&scale[2];
    return 0;
}
