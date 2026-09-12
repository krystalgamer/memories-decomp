#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_packet_handlers.h"
#include "../ygo_types.h"

/*
 * All 1564 bytes at 0x8005D378 match with gcc_2_8_1_g8_split.
 * Explicit negative/positive tests share one adjustment path per angle.
 * Wrapping applies only at duration 16 and beyond a half turn, including
 * the original signed-halfword interpretation and zero-angle direction.
 *
 * Decode the packed rewrite index into its table index and word offset;
 * the base-address table starts five words into the HMD argument block.
 * Capture the remaining duration at its first rotation use, and retain a
 * common publication exit so the final output load and return stay ordered.
 * Translation, rotation, scale and all nine output halfwords are preserved.
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

    {
        u32 rewrite = track->rewrite_idx;
        u32 table_index = rewrite >> 24;
        u32 word_offset = rewrite & 0xFFFFFF;
        node = (GsCOORDUNIT *)(((u32 *)ctx)[table_index + 5]
                            + (word_offset << 2));
    }

    node->matrix.t[0] = (((ModelAnimationSample *)slot[1])->x * t + ((ModelAnimationSample *)slot[2])->x * rest) / dur;
    node->matrix.t[1] = (((ModelAnimationSample *)slot[1])->y * t + ((ModelAnimationSample *)slot[2])->y * rest) / dur;
    node->matrix.t[2] = (((ModelAnimationSample *)slot[1])->z * t + ((ModelAnimationSample *)slot[2])->z * rest) / dur;

    rx = ((ModelAnimationSample *)slot[2])->rotation_x;
    ry = ((ModelAnimationSample *)slot[2])->rotation_y;
    rz = ((ModelAnimationSample *)slot[2])->rotation_z;
    a = ((ModelAnimationSample *)slot[1])->rotation_x;
    b = ((ModelAnimationSample *)slot[1])->rotation_y;
    c = ((ModelAnimationSample *)slot[1])->rotation_z;

    if (dur == 0x10) {
        s32 d;
        d = (s16)a - (s16)rx;
        if (d < 0) goto negative_x;
        if (d >= MODEL_ANGLE_WRAP_THRESHOLD) goto wrap_x;
        goto done_x;
negative_x:
        if ((s16)rx - (s16)a < MODEL_ANGLE_WRAP_THRESHOLD) goto done_x;
wrap_x:
        w = (s16)rx;
        if (w > 0) {
            w -= MODEL_ANGLE_FULL_TURN;
        } else {
            w += MODEL_ANGLE_FULL_TURN;
        }
        rx = w;
done_x:
        ;

        d = (s16)b - (s16)ry;
        if (d < 0) goto negative_y;
        if (d >= MODEL_ANGLE_WRAP_THRESHOLD) goto wrap_y;
        goto done_y;
negative_y:
        if ((s16)ry - (s16)b < MODEL_ANGLE_WRAP_THRESHOLD) goto done_y;
wrap_y:
        w = (s16)ry;
        if (w > 0) {
            w -= MODEL_ANGLE_FULL_TURN;
        } else {
            w += MODEL_ANGLE_FULL_TURN;
        }
        ry = w;
done_y:
        ;

        d = (s16)c - (s16)rz;
        if (d < 0) goto negative_z;
        if (d >= MODEL_ANGLE_WRAP_THRESHOLD) goto wrap_z;
        goto done_z;
negative_z:
        if ((s16)rz - (s16)c < MODEL_ANGLE_WRAP_THRESHOLD) goto done_z;
wrap_z:
        w = (s16)rz;
        if (w > 0) {
            w -= MODEL_ANGLE_FULL_TURN;
        } else {
            w += MODEL_ANGLE_FULL_TURN;
        }
        rz = w;
done_z:
        ;
    }

    node->rot.vx = ((s16)a * t + (s16)rx * (rest2 = dur - t)) / dur;
    node->rot.vy = ((s16)b * t + (s16)ry * rest2) / dur;
    node->rot.vz = ((s16)c * t + (s16)rz * rest2) / dur;

    out = &node->rot.vx;
    RotMatrixYXZ_gte((SVECTOR *)out, &node->matrix);

    scale[0] = (((ModelAnimationSample *)slot[1])->scale_x * t + ((ModelAnimationSample *)slot[2])->scale_x * rest2) / dur;
    scale[1] = (((ModelAnimationSample *)slot[1])->scale_y * t + ((ModelAnimationSample *)slot[2])->scale_y * rest2) / dur;
    scale[2] = (((ModelAnimationSample *)slot[1])->scale_z * t + ((ModelAnimationSample *)slot[2])->scale_z * rest2) / dur;
    ScaleMatrix(&node->matrix, (VECTOR *)scale);

    node->flg = 0;
    if (slot[3] == 0) goto finished;
    ((u16 *)slot[3])[0] = *(u16 *)&node->matrix.t[0];
    ((u16 *)slot[3])[1] = *(u16 *)&node->matrix.t[1];
    ((u16 *)slot[3])[2] = *(u16 *)&node->matrix.t[2];
    ((u16 *)slot[3])[3] = *(u16 *)&node->rot.vx;
    ((u16 *)slot[3])[4] = *(u16 *)&node->rot.vy;
    ((u16 *)slot[3])[5] = *(u16 *)&node->rot.vz;
    ((u16 *)slot[3])[6] = *(u16 *)&scale[0];
    ((u16 *)slot[3])[7] = *(u16 *)&scale[1];
    ((u16 *)slot[3])[8] = *(u16 *)&scale[2];
finished:
    return 0;
}
