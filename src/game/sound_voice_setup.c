#include "../types.h"
#define D_8009B458_IN_DATA
#include "sound.h"
#include "sd_pitch_from_note.h"
#include "../psyq/libspu.h"
#include "sound_sequence_constants.h"
#include "sound_spatialization.h"
#include "sd_reset_secondary_playback_thunk.h"

#define SD_SECONDARY_OFFSET_VIEW(type, state, offset) \
    ((type *)((u8 *)(state) + (offset)))

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
 * centre pan and full volume/expression. The two offsets walk the state's
 * `tracks` and `channels` arrays by byte stride, as retail does. */
void SD_ResetSecondaryPlayback(void) {
    SDSecondaryState *base;
    SDSequenceTrack *track;
    SDSecondaryRecord *channel;
    s32 mask;
    s32 i;
    s32 track_off;
    s32 off;
    s32 *tbl;
    s32 key;
    SDSecondaryObject *obj;
    s32 k40;
    s32 c72;
    s32 cff;
    s32 one;
    s32 channel_off;
    u8 b40;
    u8 b7f;
    s32 w7f;

    mask = 0;
    i = mask;
    c72 = 0x72;
    one = 1;
    cff = 0xFF;
    track_off = (u32)&((SDSecondaryState *)0)->tracks;
    base = gSD_pSecondaryState;
    base->sequence_level = 0x7F;
    do {
        track = SD_SECONDARY_OFFSET_VIEW(
            SDSequenceTrack, gSD_pSecondaryState, track_off);
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
        track_off += SD_SEQUENCE_TRACK_RECORD_SIZE;
    } while (i < SD_SEQUENCE_TRACK_COUNT);

    do {
    base = gSD_pSecondaryState;
    if (base->object_count > 0) {
        i = 0;
        k40 = 0x40;
        tbl = gSD_adwVoiceMasks;
        off = (u32)&((SDSecondaryState *)0)->objects;
    top2:
            obj = SD_SECONDARY_OFFSET_VIEW(SDSecondaryObject, base, off);
            key = *tbl;
            obj->channel_index = SD_SECONDARY_RECORD_NONE;
            obj->voice_index = i;
            obj->lifecycle_stage = 0;
            obj->key_held = 0;
            obj->pan = 0x40;
            obj->cached_pitch_bend = k40;
            obj->field_001C = k40;
            obj->reuse_countdown = 0;
            SD_ResetVoiceEnvelope(i);
            SpuSetKey(SPU_OFF, key);
            tbl++;
            off += SD_SECONDARY_OBJECT_SIZE;
            base = gSD_pSecondaryState;
            i++;
            mask |= key;
        if (i < base->object_count) goto top2;
    }
    } while (0);

    i = 0;
    b40 = 0x40;
    b7f = 0x7F;
    w7f = 0x7F;
    channel_off = i;
    do {
        channel = SD_SECONDARY_OFFSET_VIEW(
            SDSecondaryRecord, gSD_pSecondaryState, channel_off);
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
        channel_off += SD_SEQUENCE_CHANNEL_RECORD_SIZE;
    } while (i < SD_SEQUENCE_CHANNEL_COUNT);

    SpuSetKey(SPU_OFF, mask);
}

void SD_ResetSecondaryPlaybackThunk(void)
{
    SD_ResetSecondaryPlayback();
}
