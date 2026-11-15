#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_sequence_constants.h"
#include "sound_secondary_voice.h"

s32 SD_PitchFromNote(s32 note_high, s32 note_low, s32 base, s32 offset)
{
    s16 fine;
    s16 col;
    s16 diff;
    s16 down;
    s32 note;

    fine = note_low + offset;
    note = note_high + fine / 128;
    col = fine % 128;
    diff = note - base;
    if (diff >= 0) {
        return (u16)(gSD_awNotePitchTable[diff % 12][col] << (diff / 12));
    }
    down = -diff;
    return (u16)(gSD_awNotePitchTable[(12 - (s16)(down % 12)) % 12][col] >>
                 ((down + 11) / 12));
}

void SD_SpatializeSecondaryObject(SDSecondaryObject *object,
                                 SDSecondaryRecord *channel)
{
    s32 pan;
    s32 level;
    s32 left;
    s32 right;

    if (gSD_pSecondaryState->pan_center_override != 0) {
        pan = SD_SECONDARY_PAN_CENTER;
    } else {
        pan = gSD_pSecondaryState->transfer.vab_pan + object->program_pan +
              object->tone_pan + channel->pan -
              SD_SECONDARY_PAN_SUM_BIAS;
    }
    if (pan < 0) {
        pan = 0;
    }
    if (pan >= SD_SECONDARY_PAN_LIMIT) {
        pan = SD_SECONDARY_PAN_MAX;
    }
    object->pan = pan;

    level = gSD_pSecondaryState->transfer.vab_master_volume *
            (u16)gSD_pSecondaryState->sequence_level;
    level = (level * channel->expression * channel->volume) >> 14;
    level = (level * object->program_volume * object->tone_volume) >> 14;

    if (pan >= SD_SECONDARY_PAN_CENTER) {
        if (pan == SD_SECONDARY_PAN_CENTER) {
            left = right = level;
        } else {
            right = level;
            left = ((SD_SECONDARY_PAN_CENTER -
                     (pan & SD_SECONDARY_PAN_HALF_MASK)) * (right << 1)) >> 7;
        }
    } else {
        left = level;
        right = (pan * (left << 1)) >> 7;
    }

    left = (left * gSD_pSecondaryState->spatial_level_left) >> 7;
    right = (right * gSD_pSecondaryState->spatial_level_right) >> 7;
    object->level_left = (left * (object->velocity & 0x7F)) >> 7;
    object->level_right = (right * (object->velocity & 0x7F)) >> 7;
}

/* Scales one secondary object's stereo level by the two channel masters at
 * +0x514/+0x516 and submits it as a volume-only SpuSetVoiceAttr request
 * through the shared block at gSD_pSecondaryState + 0x4C0. The mask is
 * VOLL|VOLR|VOLMODEL|VOLMODER; volmode is cleared so both channels stay in
 * direct mode, and the 7.7 products are shifted back down by 7.
 *
 * The voice-mask table is const, as in sound_voice_setup.c. A load from an
 * unchanging table does not depend on the stores through `s`, so the scheduler
 * is free to place it after the right-channel master is read: the table
 * address then occupies $v0 across that master's life, which puts the master
 * in $a1 and the first product in $a3, and keeps the mask value in $a0 next to
 * its store. With the plain declaration every source shape left the master in
 * $v0. The state-pointer arm keeps its address out of small data.
 */
void SD_SetVoiceVolume(s32 voice, s32 left, s32 right) {
    SDSecondaryState *s;

    s = gSD_pSecondaryState;
    s->voice_attr.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER;
    s->voice_attr.voice = gSD_adwVoiceMasks[voice];
    s->voice_attr.volume.left = (left * s->master_level_left) >> 7;
    s->voice_attr.volume.right = (right * s->master_level_right) >> 7;
    s->voice_attr.volmode.left = SPU_VOICE_DIRECT;
    s->voice_attr.volmode.right = SPU_VOICE_DIRECT;
    SpuSetVoiceAttr(&s->voice_attr);
}

void SD_UpdateSecondaryObjectVolumes(void)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    s32 i;

    for (i = 0; i < state->object_count; i++) {
        if (state->objects[i].channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
            SD_SpatializeSecondaryObject(
                &state->objects[i],
                &state->channels[state->objects[i].channel_index]);
            SD_SetVoiceVolume(i, gSD_pSecondaryState->objects[i].level_left,
                              gSD_pSecondaryState->objects[i].level_right);
        }
        state = gSD_pSecondaryState;
    }
}

s32 SD_CalcPitchBend(SDSecondaryObject *entry, s32 value)
{
    u8 check = value;
    int result;

    if (check < SD_SEQUENCE_PITCH_BEND_CENTER) {
        if (entry->pitch_bend_negative_scale == 0)
            return 0;
        result = -((entry->pitch_bend_negative_scale << 1) *
                   (SD_SEQUENCE_PITCH_BEND_CENTER - (u8)value));
    } else {
        if (check == SD_SEQUENCE_PITCH_BEND_CENTER)
            return 0;
        if (entry->pitch_bend_positive_scale == 0)
            return 0;
        result = (entry->pitch_bend_positive_scale << 1) *
                 ((u8)value - SD_SEQUENCE_PITCH_BEND_POSITIVE_BIAS);
    }
    return (short)result;
}

/* Re-derives a voice's raw pitch when its channel's pitch-bend MSB changes
 * (or when forced). Caches the bend value in the object, adds SD_CalcPitchBend's
 * adjustment to the note in 7.7 units, fills the voice_attr request block
 * in the secondary state and submits it through SpuSetVoiceAttr. */

void SD_UpdateSecondaryObjectPitch(SDSecondaryObject *p, s32 force)
{
    SDSecondaryRecord *e;
    s32 v;
    s32 x;
    s32 y;

    e = &gSD_pSecondaryState->channels[p->channel_index];
    if (e->pitch_bend_msb == p->cached_pitch_bend && force == 0) {
        return;
    }
    p->cached_pitch_bend = e->pitch_bend_msb;
    v = SD_CalcPitchBend(p, e->pitch_bend_msb);
    v = v + (p->note << 7);
    x = (s16)v >> 7;
    y = v & 0x7F;
    gSD_pSecondaryState->voice_attr.mask = SPU_VOICE_PITCH;
    gSD_pSecondaryState->voice_attr.voice = gSD_adwVoiceMasks[p->voice_index];
    v = SD_PitchFromNote(x, y, p->center_note, p->center_fine);
    gSD_pSecondaryState->voice_attr.pitch = v;
    SpuSetVoiceAttr(&gSD_pSecondaryState->voice_attr);
}

/* Resets the secondary sequencer: every track reader goes back to its
 * ended/default-tempo state, every live object is detached from its channel
 * and keyed off with a fresh envelope, and every channel record returns to
 * centre pan and full volume/expression. */
void SD_ResetSecondaryPlayback(void) {
    SDSecondaryState *base;
    SDSequenceTrack *track;
    SDSecondaryRecord *channel;
    s32 mask;
    s32 i;
    s32 key;
    SDSecondaryObject *obj;
    s32 c72;
    s32 cff;
    s32 one;
    u8 b40;
    u8 b7f;
    s32 w7f;

    mask = 0;
    i = mask;
    c72 = 0x72;
    one = 1;
    cff = 0xFF;
    base = gSD_pSecondaryState;
    base->sequence_level = 0x7F;
    do {
        track = &gSD_pSecondaryState->tracks[i];
        i++;
        track->pos = 0;
        track->pos_saved = 0;
        track->loop_count = 0;
        track->delta_remaining = 0;
        track->chunk_length = 0;
        track->chunk_end = 0;
        track->tempo_accumulator = c72;
        track->tempo_step = c72;
        track->ended = one;
        track->chunk_start = 0;
        track->delta_scale_remainder = 0;
        track->field_0027 = 0;
        track->running_status_held = cff;
        track->running_status = 0;
        track->field_002B = 0;
    } while (i < SD_SEQUENCE_TRACK_COUNT);

    for (i = 0; i < gSD_pSecondaryState->object_count; i++) {
        obj = &gSD_pSecondaryState->objects[i];
        key = gSD_adwVoiceMasks[i];
        obj->channel_index = SD_SECONDARY_RECORD_NONE;
        obj->voice_index = i;
        obj->lifecycle_stage = 0;
        obj->key_held = 0;
        obj->pan = 0x40;
        obj->cached_pitch_bend = 0x40;
        obj->key_on_pitch_bend = 0x40;
        obj->reuse_countdown = 0;
        SD_ResetVoiceEnvelope(i);
        SpuSetKey(SPU_OFF, key);
        mask |= key;
    }

    i = 0;
    b40 = 0x40;
    b7f = 0x7F;
    w7f = 0x7F;
    do {
        channel = &gSD_pSecondaryState->channels[i];
        i++;
        channel->pan = b40;
        channel->volume = b7f;
        channel->velocity = 0;
        channel->expression = b7f;
        channel->pitch_bend_msb = b40;
        channel->field_0008 = w7f;
        channel->field_000C = w7f;
        channel->reverb_mode = 0;
        channel->field_0014 = 0;
        channel->voice_usage = 0;
    } while (i < SD_SEQUENCE_CHANNEL_COUNT);

    SpuSetKey(SPU_OFF, mask);
}

void SD_ResetSecondaryPlaybackThunk(void)
{
    SD_ResetSecondaryPlayback();
}

/* The voice-attribute block lives in the 0x40-byte hole at +0x4C0, which is
   exactly sizeof(SpuVoiceAttr) and pins a_mode/adsr1/adsr2 to the offsets the
   target stores through. The table and state-pointer declaration arms keep
   their addresses out of small data. */

void SD_SetVoiceEnvelopeFromTone(s32 index, SDSecondaryObject *object)
{
    SDSecondaryState *p = gSD_pSecondaryState;

    p->voice_attr.mask =
        SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2;
    p->voice_attr.voice = gSD_adwVoiceMasks[index];
    p->voice_attr.adsr1 = object->adsr1;
    p->voice_attr.adsr2 = object->adsr2;
    p->voice_attr.a_mode = object->a_mode;
    SpuSetVoiceAttr(&p->voice_attr);
}

void SD_ResetVoiceEnvelope(s32 index)
{
    SDSecondaryState *p = gSD_pSecondaryState;
    p->voice_attr.voice = gSD_adwVoiceMasks[index];
    p->voice_attr.mask =
        SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2;
    p->voice_attr.adsr1 = 0;
    p->voice_attr.adsr2 = 0;
    p->voice_attr.a_mode = SPU_VOICE_EXPIncN;
    SpuSetVoiceAttr(&p->voice_attr);
}

void SD_KeyOffSecondaryObject(s32 index)
{
    SDSecondaryObject *entry = &gSD_pSecondaryState->objects[index];
    int *item;
    int result;
    if (entry->channel_index >= SD_SEQUENCE_CHANNEL_COUNT)
        return;
    item = &gSD_adwVoiceMasks[index];
    do {
        SpuSetKey(SPU_OFF, *item);
        result = SpuGetKeyStatus(*item);
        if (result == SPU_OFF_ENV_ON)
            break;
    } while (result != SPU_OFF);
    entry->key_held = 0;
}

s32 SD_FindLowestPrioritySecondaryObject(s32 value)
{
    s32 result = -1;
    SDSecondaryState *state = gSD_pSecondaryState;
    s32 best = 0xFFFF;
    s32 i = 0;

    if (state->object_count > i) {
        value = (u8)value;
        do {
            u16 candidate = state->objects[i].reuse_countdown;

            if ((u16)best >= candidate &&
                state->objects[i].channel_index == value &&
                state->objects[i].lifecycle_stage != 0) {
                best = candidate;
                result = i;
            }
            state = gSD_pSecondaryState;
            i++;
        } while (i < state->object_count);
    }
    best = result;
    return best;
}

s32 SD_PrepareSecondaryObjectReuse(s32 index, s32 value)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    SDSecondaryObject *object = &state->objects[index];

    if (object->channel_index != SD_SECONDARY_RECORD_NONE) {
        SDSecondaryRecord *record = &state->channels[object->channel_index];

        if ((record->voice_usage & 0xF) != 0) {
            record->voice_usage = record->voice_usage - 1;
        }
    }
    return index;
}

s32 SD_SelectSecondaryObject(s32 value, s32 variant)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    s32 i;
    s32 result = -1;
    u32 best;

    /* Single-pass nesting preserves the retail allocation under GCC 2.8.1. */
    do {
        do {
            do {
                i = 0;
                if (state->object_count > 0) {
                    s32 none = SD_SECONDARY_RECORD_NONE;
                    s32 count = state->object_count;
                    do {
                        if (state->objects[i].channel_index == none) {
                            return i;
                        }
                        i++;
                    } while (i < count);
                }

                state = gSD_pSecondaryState;
                i = 0;
                if (state->object_count > 0) {
                    s32 count = state->object_count;
                    do {
                        if (state->objects[i].lifecycle_stage == 0) {
                            return SD_PrepareSecondaryObjectReuse(i, (u8)value);
                        }
                        i++;
                    } while (i < count);
                }

                state = gSD_pSecondaryState;
                i = 0;
                if (state->object_count > 0) {
                    do {
                        u8 owner = state->objects[i].channel_index;
                        if (owner == (u8)value &&
                            state->objects[i].key_note == (u8)variant) {
                            return SD_PrepareSecondaryObjectReuse(i, owner);
                        }
                        state = gSD_pSecondaryState;
                        i++;
                    } while (i < state->object_count);
                }

                state = gSD_pSecondaryState;
                i = 0;
                if (state->object_count > 0) {
                    s32 count = state->object_count;
                    do {
                        if (state->objects[i].lifecycle_stage == 0) {
                            return SD_PrepareSecondaryObjectReuse(i, (u8)value);
                        }
                        i++;
                    } while (i < count);
                }
            } while (0);
        } while (0);
    } while (0);

    state = gSD_pSecondaryState;
    best = 0xFFFF;
    i = 0;
    if (state->object_count > 0) {
        s32 count = state->object_count;
        do {
            u16 candidate = state->objects[i].reuse_countdown;
            if (candidate < best) {
                best = candidate;
                result = i;
            }
            i++;
        } while (i < count);
    }
    if (best >= 0xFFFF) {
        return -1;
    }
    return SD_PrepareSecondaryObjectReuse(result, (u8)value);
}

/* Per-entry sweep over the SDSecondaryObject records at
 * gSD_pSecondaryState->objects: sends the under-0x10 ones through SD_UpdateSecondaryObjectPitch,
 * resets a record whose voice is SPU_OFF, advances the +0xD counter for
 * SPU_ON_ENV_OFF, requests key-off for SPU_ON/SPU_OFF_ENV_ON through the
 * SpuSetKey/SpuGetKeyStatus pair, and ORs the touched masks into one final
 * SpuSetKey call. */

void SD_UpdateSecondaryObjectVoices(void) {
    u8 voice_key_status[0x18];
    SDSecondaryState *p;
    SDSecondaryObject *e;
    u8 *q;
    SDSecondaryRecord *r;
    s32 *t;
    s32 *u;
    s32 i;
    s32 m;
    s32 key_off_env_on;
    s32 a;
    s32 b;
    u32 c;
    s32 v;

    SpuGetAllKeysStatus((char *)voice_key_status);
    p = gSD_pSecondaryState;
    i = 0;
    m = i;

    if (p->object_count > 0) {
        key_off_env_on = SPU_OFF_ENV_ON;
        t = gSD_adwVoiceMasks;
        q = voice_key_status;
        do {
            e = &p->objects[i];
            if (e->channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
                SD_UpdateSecondaryObjectPitch(e, 0);
            }
            if (*q != SPU_OFF || e->lifecycle_stage != 0) {
                if (*q == SPU_OFF) {
                    r = &gSD_pSecondaryState->channels[e->channel_index];
                    a = r->voice_usage;
                    if ((a & 0xF) != 0) {
                        r->voice_usage = a - 1;
                    }
                    e->lifecycle_stage = 0;
                    e->reuse_countdown = 0;
                    e->channel_index = SD_SECONDARY_RECORD_NONE;
                }

                b = e->lifecycle_stage;
                c = b & 0xFF;
                if (c != 0 && *q == SPU_ON_ENV_OFF) {
                    if (c >= 2) {
                        m = m | *t;
                        SD_KeyOffSecondaryObject(i);
                    } else {
                        e->lifecycle_stage = b + 1;
                    }
                }
            }

            if (e->key_held == 0 && (u32)(*q - SPU_ON) < 2) {
                u = t;
                while (1) {
                    SpuSetKey(SPU_OFF, *u);
                    v = SpuGetKeyStatus(*u);
                    if (v == key_off_env_on) {
                        break;
                    }
                    if (v == SPU_OFF) {
                        break;
                    }
                }
                m = m | *u;
            }

            t++;
            q++;
            p = gSD_pSecondaryState;
            i++;
        } while (i < p->object_count);
    }

    if (m != 0) {
        SpuSetKey(SPU_OFF, m);
    }
}
