#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_packet_handlers.h"
#include "../ygo_types.h"

/* Advances one animation track: interpolates the translation and the three
 * rotation angles between the track's two keyframes over its duration,
 * wrapping angles across the full-turn boundary on 16-tick tracks, rebuilds
 * the matrix through RotMatrixYXZ_gte, and mirrors the six results into the
 * track's optional output record. Returns 1 when the track has no duration.
 *
 * Each value is (source * rframe + target * (tframe - rframe)) / tframe, so
 * it is the source keyframe when rframe equals tframe and the target when
 * rframe reaches zero. The GsCOORDUNIT it rewrites is
 * named by the sequence's rewrite_idx: the top byte selects one of the
 * section pointers that follow header_size, the low 24 bits a word offset
 * into that section. */
int Model_InterpolateTransRot(GsARGUNIT_ANIM *sp) {
    ModelAnimParams *params;
    GsSEQ *seq;
    GsCOORDUNIT *dst;
    ModelKeyframe *out;
    s32 rest;
    s32 total;
    u32 offset;
    u32 section;
    s32 v;
    u16 source_rx;
    u16 source_ry;
    u16 source_rz;
    u16 target_rx;
    u16 target_ry;
    u16 target_rz;
    params = (ModelAnimParams *)(&sp->header_size + sp->header_size);
    seq = params->seq;
    if (seq->tframe == 0) {
        return 1;
    }
    rest = seq->rframe;
    total = seq->tframe;
    offset = seq->rewrite_idx;
    section = offset >> 24;
    offset &= 0xFFFFFF;
    dst = (GsCOORDUNIT *)((u32 *)(&sp->header_size)[section] + offset);
    dst->matrix.t[0] = (params->source->x * rest + params->target->x * (total - rest)) / total;
    dst->matrix.t[1] = (params->source->y * rest + params->target->y * (total - rest)) / total;
    dst->matrix.t[2] = (params->source->z * rest + params->target->z * (total - rest)) / total;
    target_rx = params->target->rx;
    target_ry = params->target->ry;
    target_rz = params->target->rz;
    source_rx = params->source->rx;
    source_ry = params->source->ry;
    source_rz = params->source->rz;
    if (total == 16) {
        if ((s16)source_rx - (s16)target_rx < 0) {
            goto n0;
        }
        if ((s16)source_rx - (s16)target_rx >= MODEL_ANGLE_WRAP_THRESHOLD) {
            goto w0;
        }
        goto d0;
    n0:
        if ((s16)target_rx - (s16)source_rx < MODEL_ANGLE_WRAP_THRESHOLD) {
            goto d0;
        }
    w0:
        v = (s16)target_rx;
        if (v > 0) {
            v = v - MODEL_ANGLE_FULL_TURN;
        } else {
            v = v + MODEL_ANGLE_FULL_TURN;
        }
        target_rx = v;
    d0:
        ;
        if ((s16)source_ry - (s16)target_ry < 0) {
            goto n1;
        }
        if ((s16)source_ry - (s16)target_ry >= MODEL_ANGLE_WRAP_THRESHOLD) {
            goto w1;
        }
        goto d1;
    n1:
        if ((s16)target_ry - (s16)source_ry < MODEL_ANGLE_WRAP_THRESHOLD) {
            goto d1;
        }
    w1:
        v = (s16)target_ry;
        if (v > 0) {
            v = v - MODEL_ANGLE_FULL_TURN;
        } else {
            v = v + MODEL_ANGLE_FULL_TURN;
        }
        target_ry = v;
    d1:
        ;
        if ((s16)source_rz - (s16)target_rz < 0) {
            goto n2;
        }
        if ((s16)source_rz - (s16)target_rz >= MODEL_ANGLE_WRAP_THRESHOLD) {
            goto w2;
        }
        goto d2;
    n2:
        if ((s16)target_rz - (s16)source_rz < MODEL_ANGLE_WRAP_THRESHOLD) {
            goto d2;
        }
    w2:
        v = (s16)target_rz;
        if (v > 0) {
            v = v - MODEL_ANGLE_FULL_TURN;
        } else {
            v = v + MODEL_ANGLE_FULL_TURN;
        }
        target_rz = v;
    d2:
        ;
    }
    dst->rot.vx = ((s16)source_rx * rest + (s16)target_rx * (total - rest)) / total;
    dst->rot.vy = ((s16)source_ry * rest + (s16)target_ry * (total - rest)) / total;
    dst->rot.vz = ((s16)source_rz * rest + (s16)target_rz * (total - rest)) / total;
    RotMatrixYXZ_gte(&dst->rot, &dst->matrix);
    dst->flg = 0;
    out = params->out;
    if (out != 0) {
        params->out->x = dst->matrix.t[0];
        params->out->y = dst->matrix.t[1];
        params->out->z = dst->matrix.t[2];
        params->out->rx = dst->rot.vx;
        params->out->ry = dst->rot.vy;
        params->out->rz = dst->rot.vz;
    }
    return 0;
}

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
s32 Model_InterpolateTransRotScale(GsARGUNIT_ANIM *ctx)
{
    GsSEQ *track;
    GsCOORDUNIT *node;
    s16 *out;
    ModelAnimationInterpolationSlots *slot;
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

    slot = (ModelAnimationInterpolationSlots *)(
        &ctx->header_size + ctx->header_size
    );
    track = slot->track;
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

    node->matrix.t[0] =
        (slot->sample_1->x * t + slot->sample_2->x * rest) / dur;
    node->matrix.t[1] =
        (slot->sample_1->y * t + slot->sample_2->y * rest) / dur;
    node->matrix.t[2] =
        (slot->sample_1->z * t + slot->sample_2->z * rest) / dur;

    rx = slot->sample_2->rotation_x;
    ry = slot->sample_2->rotation_y;
    rz = slot->sample_2->rotation_z;
    a = slot->sample_1->rotation_x;
    b = slot->sample_1->rotation_y;
    c = slot->sample_1->rotation_z;

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

    scale[0] =
        (slot->sample_1->scale_x * t +
         slot->sample_2->scale_x * rest2) / dur;
    scale[1] =
        (slot->sample_1->scale_y * t +
         slot->sample_2->scale_y * rest2) / dur;
    scale[2] =
        (slot->sample_1->scale_z * t +
         slot->sample_2->scale_z * rest2) / dur;
    ScaleMatrix(&node->matrix, (VECTOR *)scale);

    node->flg = 0;
    if (slot->output == 0) goto finished;
    slot->output[0] = *(u16 *)&node->matrix.t[0];
    slot->output[1] = *(u16 *)&node->matrix.t[1];
    slot->output[2] = *(u16 *)&node->matrix.t[2];
    slot->output[3] = *(u16 *)&node->rot.vx;
    slot->output[4] = *(u16 *)&node->rot.vy;
    slot->output[5] = *(u16 *)&node->rot.vz;
    slot->output[6] = *(u16 *)&scale[0];
    slot->output[7] = *(u16 *)&scale[1];
    slot->output[8] = *(u16 *)&scale[2];
finished:
    return 0;
}
