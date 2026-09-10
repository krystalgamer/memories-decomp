#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_packet_handlers.h"

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
int func_8005CEF0(GsARGUNIT_ANIM *sp) {
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
