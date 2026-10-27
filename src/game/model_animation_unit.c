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
    ModelAnimParams *interpolation_parameters;
    GsSEQ *sequence;
    GsCOORDUNIT *coordinate_unit;
    ModelKeyframe *output_keyframe;
    s32 remaining_duration;
    s32 total_duration;
    u32 rewrite_work;
    u32 rewrite_section;
    s32 adjusted_target_angle;
    u16 source_rx;
    u16 source_ry;
    u16 source_rz;
    u16 target_rx;
    u16 target_ry;
    u16 target_rz;
    interpolation_parameters = (ModelAnimParams *)(&sp->header_size + sp->header_size);
    sequence = interpolation_parameters->seq;
    if (sequence->tframe == 0) {
        return 1;
    }
    remaining_duration = sequence->rframe;
    total_duration = sequence->tframe;
    rewrite_work = sequence->rewrite_idx;
    rewrite_section = rewrite_work >> 24;
    rewrite_work &= 0xFFFFFF;
    coordinate_unit = (GsCOORDUNIT *)((u32 *)(&sp->header_size)[rewrite_section] + rewrite_work);
    coordinate_unit->matrix.t[0] = (interpolation_parameters->source->x * remaining_duration + interpolation_parameters->target->x * (total_duration - remaining_duration)) / total_duration;
    coordinate_unit->matrix.t[1] = (interpolation_parameters->source->y * remaining_duration + interpolation_parameters->target->y * (total_duration - remaining_duration)) / total_duration;
    coordinate_unit->matrix.t[2] = (interpolation_parameters->source->z * remaining_duration + interpolation_parameters->target->z * (total_duration - remaining_duration)) / total_duration;
    target_rx = interpolation_parameters->target->rx;
    target_ry = interpolation_parameters->target->ry;
    target_rz = interpolation_parameters->target->rz;
    source_rx = interpolation_parameters->source->rx;
    source_ry = interpolation_parameters->source->ry;
    source_rz = interpolation_parameters->source->rz;
    if (total_duration == 16) {
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
        adjusted_target_angle = (s16)target_rx;
        if (adjusted_target_angle > 0) {
            adjusted_target_angle = adjusted_target_angle - MODEL_ANGLE_FULL_TURN;
        } else {
            adjusted_target_angle = adjusted_target_angle + MODEL_ANGLE_FULL_TURN;
        }
        target_rx = adjusted_target_angle;
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
        adjusted_target_angle = (s16)target_ry;
        if (adjusted_target_angle > 0) {
            adjusted_target_angle = adjusted_target_angle - MODEL_ANGLE_FULL_TURN;
        } else {
            adjusted_target_angle = adjusted_target_angle + MODEL_ANGLE_FULL_TURN;
        }
        target_ry = adjusted_target_angle;
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
        adjusted_target_angle = (s16)target_rz;
        if (adjusted_target_angle > 0) {
            adjusted_target_angle = adjusted_target_angle - MODEL_ANGLE_FULL_TURN;
        } else {
            adjusted_target_angle = adjusted_target_angle + MODEL_ANGLE_FULL_TURN;
        }
        target_rz = adjusted_target_angle;
    d2:
        ;
    }
    coordinate_unit->rot.vx = ((s16)source_rx * remaining_duration + (s16)target_rx * (total_duration - remaining_duration)) / total_duration;
    coordinate_unit->rot.vy = ((s16)source_ry * remaining_duration + (s16)target_ry * (total_duration - remaining_duration)) / total_duration;
    coordinate_unit->rot.vz = ((s16)source_rz * remaining_duration + (s16)target_rz * (total_duration - remaining_duration)) / total_duration;
    RotMatrixYXZ_gte(&coordinate_unit->rot, &coordinate_unit->matrix);
    coordinate_unit->flg = 0;
    output_keyframe = interpolation_parameters->out;
    if (output_keyframe != 0) {
        interpolation_parameters->out->x = coordinate_unit->matrix.t[0];
        interpolation_parameters->out->y = coordinate_unit->matrix.t[1];
        interpolation_parameters->out->z = coordinate_unit->matrix.t[2];
        interpolation_parameters->out->rx = coordinate_unit->rot.vx;
        interpolation_parameters->out->ry = coordinate_unit->rot.vy;
        interpolation_parameters->out->rz = coordinate_unit->rot.vz;
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
    s16 *rotation_halfwords;
    ModelAnimationInterpolationSlots *interpolation_slots;
    s32 remaining_duration;
    s32 total_duration;
    s32 translation_elapsed;
    s32 rotation_scale_elapsed;
    s32 source_rx;
    s32 source_ry;
    s32 target_rx;
    s32 target_ry;
    s32 target_rz;
    s32 adjusted_target_angle;
    s32 source_rz;
    s32 scale[3];

    interpolation_slots = (ModelAnimationInterpolationSlots *)(
        &ctx->header_size + ctx->header_size
    );
    track = interpolation_slots->track;
    if (track->tframe == 0) {
        return 1;
    }

    remaining_duration = track->rframe;
    total_duration = track->tframe;
    translation_elapsed = total_duration - remaining_duration;

    {
        u32 rewrite = track->rewrite_idx;
        u32 table_index = rewrite >> 24;
        u32 word_offset = rewrite & 0xFFFFFF;
        node = (GsCOORDUNIT *)(((u32 *)ctx)[table_index + 5]
                            + (word_offset << 2));
    }

    node->matrix.t[0] =
        (interpolation_slots->sample_1->x * remaining_duration + interpolation_slots->sample_2->x * translation_elapsed) / total_duration;
    node->matrix.t[1] =
        (interpolation_slots->sample_1->y * remaining_duration + interpolation_slots->sample_2->y * translation_elapsed) / total_duration;
    node->matrix.t[2] =
        (interpolation_slots->sample_1->z * remaining_duration + interpolation_slots->sample_2->z * translation_elapsed) / total_duration;

    target_rx = interpolation_slots->sample_2->rotation_x;
    target_ry = interpolation_slots->sample_2->rotation_y;
    target_rz = interpolation_slots->sample_2->rotation_z;
    source_rx = interpolation_slots->sample_1->rotation_x;
    source_ry = interpolation_slots->sample_1->rotation_y;
    source_rz = interpolation_slots->sample_1->rotation_z;

    if (total_duration == 0x10) {
        s32 signed_angle_delta;
        signed_angle_delta = (s16)source_rx - (s16)target_rx;
        if (signed_angle_delta < 0) goto negative_x;
        if (signed_angle_delta >= MODEL_ANGLE_WRAP_THRESHOLD) goto wrap_x;
        goto done_x;
negative_x:
        if ((s16)target_rx - (s16)source_rx < MODEL_ANGLE_WRAP_THRESHOLD) goto done_x;
wrap_x:
        adjusted_target_angle = (s16)target_rx;
        if (adjusted_target_angle > 0) {
            adjusted_target_angle -= MODEL_ANGLE_FULL_TURN;
        } else {
            adjusted_target_angle += MODEL_ANGLE_FULL_TURN;
        }
        target_rx = adjusted_target_angle;
done_x:
        ;

        signed_angle_delta = (s16)source_ry - (s16)target_ry;
        if (signed_angle_delta < 0) goto negative_y;
        if (signed_angle_delta >= MODEL_ANGLE_WRAP_THRESHOLD) goto wrap_y;
        goto done_y;
negative_y:
        if ((s16)target_ry - (s16)source_ry < MODEL_ANGLE_WRAP_THRESHOLD) goto done_y;
wrap_y:
        adjusted_target_angle = (s16)target_ry;
        if (adjusted_target_angle > 0) {
            adjusted_target_angle -= MODEL_ANGLE_FULL_TURN;
        } else {
            adjusted_target_angle += MODEL_ANGLE_FULL_TURN;
        }
        target_ry = adjusted_target_angle;
done_y:
        ;

        signed_angle_delta = (s16)source_rz - (s16)target_rz;
        if (signed_angle_delta < 0) goto negative_z;
        if (signed_angle_delta >= MODEL_ANGLE_WRAP_THRESHOLD) goto wrap_z;
        goto done_z;
negative_z:
        if ((s16)target_rz - (s16)source_rz < MODEL_ANGLE_WRAP_THRESHOLD) goto done_z;
wrap_z:
        adjusted_target_angle = (s16)target_rz;
        if (adjusted_target_angle > 0) {
            adjusted_target_angle -= MODEL_ANGLE_FULL_TURN;
        } else {
            adjusted_target_angle += MODEL_ANGLE_FULL_TURN;
        }
        target_rz = adjusted_target_angle;
done_z:
        ;
    }

    node->rot.vx = ((s16)source_rx * remaining_duration + (s16)target_rx * (rotation_scale_elapsed = total_duration - remaining_duration)) / total_duration;
    node->rot.vy = ((s16)source_ry * remaining_duration + (s16)target_ry * rotation_scale_elapsed) / total_duration;
    node->rot.vz = ((s16)source_rz * remaining_duration + (s16)target_rz * rotation_scale_elapsed) / total_duration;

    rotation_halfwords = &node->rot.vx;
    RotMatrixYXZ_gte((SVECTOR *)rotation_halfwords, &node->matrix);

    scale[0] =
        (interpolation_slots->sample_1->scale_x * remaining_duration +
         interpolation_slots->sample_2->scale_x * rotation_scale_elapsed) / total_duration;
    scale[1] =
        (interpolation_slots->sample_1->scale_y * remaining_duration +
         interpolation_slots->sample_2->scale_y * rotation_scale_elapsed) / total_duration;
    scale[2] =
        (interpolation_slots->sample_1->scale_z * remaining_duration +
         interpolation_slots->sample_2->scale_z * rotation_scale_elapsed) / total_duration;
    ScaleMatrix(&node->matrix, (VECTOR *)scale);

    node->flg = 0;
    if (interpolation_slots->output == 0) goto finished;
    interpolation_slots->output[0] = *(u16 *)&node->matrix.t[0];
    interpolation_slots->output[1] = *(u16 *)&node->matrix.t[1];
    interpolation_slots->output[2] = *(u16 *)&node->matrix.t[2];
    interpolation_slots->output[3] = *(u16 *)&node->rot.vx;
    interpolation_slots->output[4] = *(u16 *)&node->rot.vy;
    interpolation_slots->output[5] = *(u16 *)&node->rot.vz;
    interpolation_slots->output[6] = *(u16 *)&scale[0];
    interpolation_slots->output[7] = *(u16 *)&scale[1];
    interpolation_slots->output[8] = *(u16 *)&scale[2];
finished:
    return 0;
}
