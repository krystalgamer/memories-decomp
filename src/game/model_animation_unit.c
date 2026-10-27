#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/stdio.h"
#include "model.h"
#include "model_packet_handlers.h"
#include "../unmatched.h"
#include "../psyq/exit.h"
#include "model_handler_state.h"
#include "model_sequence_descriptor.h"
#include "../ygo_types.h"

#define MODEL_ANIMATION_SEQUENCE_SLOT(entry) \
    ((GsSEQ **)&(entry)->animation_sequences[(entry)->animation_sequence_count])
#define MODEL_ANIMATION_WORDS(object) ((u32 *)(object)[1])

void Model_LinkSlotAnimationUnit(s32 *object, ModelSlot *entry)
{
    int scanned_unit_words[2];
    int linked_sequence_count;
    void *primitive_handler;

    if (object[0] != 0x03800000) {
#ifdef VERSION_EUROPE
        printf("unsupported ANIMATION primitive 0x%08lx.\n", object[0]);
#endif
        primitive_handler = (void *)GsU_00000000;
        *(void **)object[1] = primitive_handler;
        return;
    }
    primitive_handler = Model_UpdateAnimationUnit;
    *(void **)object[1] = primitive_handler;
    linked_sequence_count = GsLinkAnim(
        MODEL_ANIMATION_SEQUENCE_SLOT(entry),
        MODEL_ANIMATION_WORDS(object)
    );
    if (GsScanAnim(MODEL_ANIMATION_WORDS(object), 0)) {
        while (GsScanAnim(0, (GsTYPEUNIT *)scanned_unit_words)) {
            *(void **)scanned_unit_words[1] =
                Model_GetAnimationInterpolationHandler(scanned_unit_words[0]);
        }
        entry->animation_sequence_count += linked_sequence_count;
    }
}

void *Model_GetAnimationInterpolationHandler(u32 value)
{
    u32 interpolation_type;

    if ((value & 0xFFFF0000) != 0x03000000) {
        goto default_case;
    }
    interpolation_type = value & 0xFFFF;
    if (interpolation_type == 0x2019) {
        goto case_2019;
    }
    if (interpolation_type == 0x2119) {
        goto case_2119;
    }
    goto default_case;

case_2019:
    return Model_InterpolateTransRot;
case_2119:
    return Model_InterpolateTransRotScale;
default_case:
#ifdef VERSION_EUROPE
    printf("unsupported ANIMATION interpolation 0x%08lx.\n", value);
#endif
    return (void *)GsU_00000000;
}

/*
 * All 1844 bytes match under gcc_2_8_1_g0 and, byte for byte, under this
 * unit's gcc_2_8_1_g8_split.
 * The shared scanner inlines into the four endpoint walks. Direction-local
 * index captures preserve one read; separate duration paths retain their
 * distinct stores; the count precedes the handler table in the spill order.
 *
 * The single-pass endpoint checks preserve GCC 2.8.1's reference weighting.
 * Flattening them exchanges the context and cached-sentinel registers in
 * fifteen words. The GsSEQ cursor starts after one canonical GsSEH header and
 * advances by each record's u32 size. The command byte offset derives from
 * the typed command width. The ModelAnimParams tail keeps its byte-addressed
 * form but derives its base from GsARGUNIT_ANIM::header_size. No register
 * bindings or instruction assembly are used.
 */
static __inline__ u32 scan_command(GsSEQ *track, ModelSeqDescriptor *commands,
                                       u32 index)
{
    s32 step;
    u32 word;
    u32 condition;
    u32 action;

    step = track->speed < 0 ? -1 : 1;
    while ((word = commands[(u16)index].word) & MODEL_SEQ_DESCRIPTOR_CONTROL) {
        if ((word & 0xC0000000) == 0x80000000) {
            condition = (word & 0x7F0000) >> 16;
            if (condition == 0 || track->sid == condition) {
                action = (word & 0x3F800000) >> 23;
                if (condition != 0 || action != 0) {
                    track->sid = action;
                }
                index = word;
                continue;
            }
        } else if ((word & 0xC0000000) == 0xC0000000) {
            condition = (word & 0x7F0000) >> 16;
            action = (word & 0x3F800000) >> 23;
            if (action == 1 && (condition == 0 || track->sid == condition)) {
                return 0xFFFF;
            }
        } else {
            printf(gModelAnim_szAssertFormat, gModelAnim_szSourceFileName, 0x57);
            exit(1);
        }
        index += step;
    }
    return index;
}

u32 *Model_UpdateAnimationUnit(GsARGUNIT_ANIM *ctx)
{
    GsSEQ *cursor;
    GsSEQ *track;
    s32 sequence_count;
    u32 *handler_words;
    ModelSeqDescriptor *commands;
    u32 *parameter_words;
    ModelAnimParams *interpolation_parameters;
    s32 (*handler)(GsARGUNIT_ANIM *);
    s32 sequence_index;
    s32 direction;
    u32 active_frames;
    u32 next_command_index;
    u32 interpolation_type;
    u32 target_duration;
    ModelSeqDescriptor *dispatch_record;
    s32 handler_result;
    u32 *unit_words;

    cursor = (GsSEQ *)((GsSEH *)ctx->primp + 1);
    handler_words = (u32 *)ctx->htop;
    commands = (ModelSeqDescriptor *)ctx->ctop;
    sequence_count = ((u16 *)ctx->primp)[1];
    parameter_words = (u32 *)ctx->ptop;
    for (sequence_index = 0; sequence_index < sequence_count; sequence_index++) {
        active_frames = cursor->aframe;
        track = cursor;
        if ((u16)active_frames != 0) {
            if ((u16)active_frames != 0xFFFF) {
                cursor->aframe = active_frames - 1;
            }
            direction = cursor->speed < 0 ? -1 : 1;
            if (cursor->rframe == 0x7000) {
                direction = 1;
                cursor->rframe = 0;
            } else if (cursor->rframe == 0x6000) {
                direction = -1;
                cursor->rframe = 0;
            }
retry:
            if (track->rframe == 0 || track->tframe == 0) {
                track->traveling = 0;
                if (direction == 1) {
                    u32 old_index;
                    if (commands[track->ti].word & MODEL_SEQ_DESCRIPTOR_CONTROL) {
                        next_command_index = scan_command(track, commands, track->ti + 1);
                        track->ti = next_command_index;
                        do {
                            if ((u16)next_command_index == 0xFFFF) goto stopped;
                        } while (0);
                    }
                    old_index = track->ti;
                    track->ci = old_index;
                    next_command_index = scan_command(track, commands, old_index + direction);
                    track->ti = next_command_index;
                } else {
                    u32 old_index;
                    if (commands[track->ci].word & MODEL_SEQ_DESCRIPTOR_CONTROL) {
                        next_command_index = scan_command(track, commands, track->ci + direction);
                        track->ci = next_command_index;
                        do {
                            if ((u16)next_command_index == 0xFFFF) goto stopped;
                        } while (0);
                    }
                    old_index = track->ci;
                    track->ti = old_index;
                    next_command_index = scan_command(track, commands, old_index + direction);
                    track->ci = next_command_index;
                }
                if ((u16)next_command_index == 0xFFFF) {
stopped:
                    track->aframe = 0;
                    goto next_track;
                }
                if (direction == 1) {
                    target_duration = commands[track->ti].f.tframe << 4;
                    track->tframe = target_duration;
                    track->rframe = target_duration - (u16)track->rframe;
                } else {
                    track->tframe = commands[track->ti].f.tframe << 4;
                }
            }
            dispatch_record = (ModelSeqDescriptor *)(
                track->ti * sizeof(*commands) + (u32)commands);
            interpolation_parameters = (ModelAnimParams *)(
                (u8 *)ctx +
                (u32)&((GsARGUNIT_ANIM *)0)->header_size +
                (ctx->header_size << 2)
            );
            interpolation_type = dispatch_record->f.type & MODEL_SEQ_DESCRIPTOR_TYPE_MASK;
            handler = (s32 (*)(GsARGUNIT_ANIM *))handler_words[interpolation_type + 1];
            interpolation_parameters->seq = cursor;
            interpolation_parameters->source =
                (ModelKeyframe *)(parameter_words + commands[track->ci].f.param_index);
            interpolation_parameters->target =
                (ModelKeyframe *)(parameter_words + commands[track->ti].f.param_index);
            if (track->ii != 0xFFFF) {
                interpolation_parameters->out =
                    (ModelKeyframe *)(parameter_words + commands[track->ii].f.param_index);
            } else {
                interpolation_parameters->out = 0;
            }
            if (track->ci != track->ti) {
                handler_result = handler(ctx);
                if (handler_result == 1) {
                    track->rframe = 0;
                    goto retry;
                }
                if (commands[track->ti].f.tframe == 0) {
                    goto retry;
                }
                track->rframe = (u16)track->rframe - track->speed;
                if (direction == 1 && track->rframe < 0) {
                    track->rframe = 0x7000;
                } else if (direction == -1 && track->rframe >= track->tframe) {
                    track->rframe = 0x6000;
                }
            }
        }
next_track:
        cursor = (GsSEQ *)((u32 *)cursor + cursor->size);
    }
    unit_words = (u32 *)ctx->primp;
    return unit_words + *(u16 *)unit_words;
}

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
