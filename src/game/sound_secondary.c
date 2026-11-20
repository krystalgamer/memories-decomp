#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_driver.h"
#include "sound_secondary.h"
#include "sound_transfer_lifecycle.h"
#include "sound_secondary_voice.h"
#include "sound_sequence_timing.h"

void SD_ResetSecondaryState(void)
{
    SDSecondaryState *levels_state;
    SDSecondaryState *pan_state;
    SDSecondaryState *transfer_state;
    SDSecondaryState *transfer_aux_state;
    SDSecondaryState *transfer_pan_state;
    SDSecondaryState *sequence_state;

    gSD_pSecondaryState->timer_update_inhibit = 0;
    gSD_pSecondaryState->update_in_progress = 0;
    gSD_pSecondaryState->shared_track_updates_enabled = 0;
    levels_state = gSD_pSecondaryState;
    levels_state->object_count = SD_SECONDARY_OBJECT_COUNT;
    levels_state->sequence_updates_enabled = 0;
    pan_state = gSD_pSecondaryState;
    levels_state->sequence_level = 127;
    levels_state->master_level_left = 127;
    levels_state->master_level_right = 127;
    pan_state->pan_center_override = 0;
    transfer_state = gSD_pSecondaryState;
    transfer_state->transfer.field_0019 = 127;
    transfer_aux_state = gSD_pSecondaryState;
    transfer_state->transfer.state_token = SD_TRANSFER_STATE_INACTIVE;
    transfer_aux_state->transfer.field_001A = 127;
    transfer_pan_state = gSD_pSecondaryState;
    transfer_pan_state->transfer.vab_pan = 64;
    sequence_state = gSD_pSecondaryState;
    sequence_state->sequence.sequence_open_token = -1;
    sequence_state->sequence.sequence_state = 0;
    sequence_state->sequence.spatial_level_right = SD_SECONDARY_LEVEL_MAX;
    sequence_state->sequence.spatial_level_left = SD_SECONDARY_LEVEL_MAX;
    sequence_state->fast_forward_target_address = 0;
    sequence_state->last_track_cursor_address = 0;
    sequence_state->sequence_tick_mode = 0x1000;
}

void SD_InitSecondaryWorkArea(SDSecondaryState *work_area)
{
    s32 *work_area_cursor = (s32 *)work_area;
    u32 cleared_word_count = 0;

    gSD_pSecondaryState = work_area;
    do {
        *work_area_cursor = 0;
        cleared_word_count++;
        work_area_cursor++;
    } while (cleared_word_count < sizeof(SDSecondaryState) / sizeof(s32));
    SpuInit();
    SD_ResetSecondaryPlaybackThunk();
    SD_ResetSecondaryState();
}

void SD_EnableSecondarySequenceUpdates(void)
{
    s32 tick_mode = gSD_pSecondaryState->sequence_tick_mode;

    if (tick_mode > 0) {
        if (tick_mode < 4)
            SD_OpenSequenceTimerEvent();
    }
    gSD_pSecondaryState->sequence_updates_enabled = 1;
}

void SD_SetSequenceTickMode(s32 tick_mode)
{
    gSD_pSecondaryState->sequence_tick_mode = tick_mode;
}

void SD_UpdateSecondarySequenceIfEnabled(void)
{
    if (gSD_pSecondaryState->sequence_updates_enabled)
        SD_UpdateSecondarySequence();
}

void SD_EnableSecondaryPanning(void)
{
    gSD_pSecondaryState->pan_center_override = 0;
}

void SD_DisableSecondaryPanning(void)
{
    gSD_pSecondaryState->pan_center_override = 1;
}

/* Validates a byte value against the secondary-object capacity and stores it
   into the s16 field 0x510 of *gSD_pSecondaryState. Returns the stored value, or 0xFF
   if out of range or zero. */
s32 SD_SetSecondaryObjectCount(u32 count) {
    u8 count_byte = count & 0xFF;

    if (count_byte >= SD_SECONDARY_OBJECT_COUNT + 1) {
        return 0xFF;
    }
    if (count_byte == 0) {
        return 0xFF;
    }
    gSD_pSecondaryState->object_count = count_byte;
    return count_byte;
}

void SD_ResetSecondaryRuntime(void)
{
    s32 tick_mode;

    SD_ResetSecondaryPlaybackThunk();
    tick_mode = gSD_pSecondaryState->sequence_tick_mode;
    if (tick_mode > 0) {
        if (tick_mode < 4)
            SD_CloseSequenceTimerEvent();
    }
    SD_ResetSecondaryState();
}

void SD_Term(void)
{
    SD_ResetSecondaryPlaybackThunk();
    SD_ResetSecondaryState();
    SpuQuit();
}

/*
 * SD_VabOpenHead: opens a VAB header for the secondary sound bank
 *
 * `vab` points at a "pBAV" header; both callers pass s_pwSD_MUSIC_TRACK + 0x50,
 * which is where every SD_BGM.DAT package keeps one. The header size is
 * (ps << 9) + 0xA20: the 0x20-byte VabHdr, 128 0x10-byte ProgAtr records,
 * ps * 16 0x20-byte VagAtr records and the 0x200-byte VAG offset table.
 * fsize (+0x0C) minus that is the body size, and mvol (+0x18) and pan
 * (+0x19) are kept beside them. `spu_addr` is where the body goes: the
 * packages' own +0x0C word, which with the body size at +8 ends every one at
 * SPU address 0x79020. SD_VabTransBody and SD_VabTransBodyChunk later
 * SpuWrite the body there. Asking for id -1 fails while the slot is still
 * live.
 *
 * The 152-byte function at `0x800496C4` matches under the existing uniform
 * `gcc_2_8_1_g0` profile using `SDSecondaryState` and `SDSecondaryTransfer` from
 * `sound.h`. The shared `s32(SDVabHeader *, s16, s32)` contract is unchanged.
 * No new declaration view, compiler profile, inline assembly, or literal
 * global address is needed.
 *
 * The zero value is initialized in a single-iteration scope, then the existing
 * signed-halfword argument is captured into a word local before the first
 * `gSD_pSecondaryState` load. This replaces both hard-register bindings. Flattening the
 * zero initialization loses one instruction; moving the argument capture before
 * that scope exchanges three prologue words; initializing the state pointer at
 * declaration moves its absolute load too early. The inherited eight-byte local
 * frame remains: retail adjusts `$sp` on entry and exit but never accesses it.
 *
 * The first state's consumed-byte count is cleared before the conditional
 * inactive-state rejection. The function then reloads the state pointer, clears
 * the transfer status, and populates the header pointer, header-derived byte
 * counts, two control bytes, and the final pointer-valued field in retail order.
 * The header is taken as SDVabHeader * and stored as one in the transfer
 * record; the SPU destination is stored as the SPU address it is.
 *
 * All 38 target instructions agree with retail. The original canonical match
 * and six-entry inline-refinement series ending in deferral remain intact; a new
 * `post_terminal_resolution` row records the initialization-order discriminator.
 */
s32 SD_VabOpenHead(SDVabHeader *vab, s16 vab_id, s32 spu_addr)
{
    /* Retail reserves eight stack bytes without accessing them. */
    volatile s32 pad[2];
    s32 zero;
    s32 requested;
    SDSecondaryState *initial;
    SDSecondaryState *state;
    SDSecondaryTransfer *entry;

    do {
        zero = 0;
    } while (0);
    requested = vab_id;
    initial = gSD_pSecondaryState;
    initial->bytes_consumed = 0;
    if (requested == SD_TRANSFER_STATE_INACTIVE &&
        initial->transfer.state_token != requested) {
        return SD_TRANSFER_ERROR;
    }
    state = gSD_pSecondaryState;
    state->transfer.state_token = zero;
    entry = &state->transfer;
    entry->vab_header = vab;
    entry->header_size_bytes = (vab->program_count << 9) + 0xA20;
    entry->body_size_bytes = vab->file_size - entry->header_size_bytes;
    entry->vab_master_volume = vab->master_volume;
    entry->vab_pan = vab->pan;
    entry->spu_body_base = spu_addr;
    return 0;
}

s32 SD_VabTransBody(u8 *source_address, s16 expected_token)
{
    register u8 *saved_source_address;
    SDSecondaryState *transfer_state = gSD_pSecondaryState;
    short current_token = transfer_state->transfer.state_token;

    if (current_token != expected_token)
        return SD_TRANSFER_ERROR;
    saved_source_address = source_address;
    {
        SDSecondaryTransfer *transfer = &transfer_state->transfer;
        SpuSetTransferStartAddr(transfer->spu_body_base);
        if (SpuWrite(saved_source_address, (u32)transfer->body_size_bytes) !=
            transfer->body_size_bytes)
            return SD_TRANSFER_ERROR;
        transfer->full_body_source_address = saved_source_address;
    }
    return current_token;
}

/* Validates the caller's state token against the pending transfer's
   discriminant, clamps the requested byte count to what remains in the
   window, submits it to the SPU transfer entrypoint, and advances the
   consumed-byte counter. Returns the state token once the window fills,
   SD_TRANSFER_INCOMPLETE while more remains, or SD_TRANSFER_ERROR on a
   state mismatch or short transfer. */
s32 SD_VabTransBodyChunk(u8 *source_bytes, s32 byte_count, s32 expected_token) {
    SDSecondaryState *transfer_state = gSD_pSecondaryState;
    SDSecondaryTransfer *transfer;
    s32 written_bytes;

    if (transfer_state->transfer.state_token != (s16)expected_token) {
        return SD_TRANSFER_ERROR;
    }

    transfer = &transfer_state->transfer;
    SpuSetTransferStartAddr(
        transfer->spu_body_base + transfer_state->bytes_consumed
    );

    {
        s32 remaining_bytes = transfer->body_size_bytes - gSD_pSecondaryState->bytes_consumed;
        if ((u32)remaining_bytes < (u32)byte_count) {
            byte_count = remaining_bytes;
        }
    }

    written_bytes = SpuWrite(source_bytes, (u32)byte_count);
    if (written_bytes != byte_count) {
        return SD_TRANSFER_ERROR;
    }

    {
        u32 consumed_total = gSD_pSecondaryState->bytes_consumed + byte_count;
        gSD_pSecondaryState->bytes_consumed = consumed_total;
        if (consumed_total >= (u32)transfer->body_size_bytes) {
            return (s16)expected_token;
        }
    }

    return SD_TRANSFER_INCOMPLETE;
}

s16 SD_VabTransCompleted(s16 wait_mode)
{
    if (wait_mode == 0)
        return SpuIsTransferCompleted(SPU_TRANSFER_PEEK);
    return SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
}

void SD_ResetVabTransferState(s32 unused)
{
    s16 *state_token_ptr = &gSD_pSecondaryState->transfer.state_token;

    if (*state_token_ptr != SD_TRANSFER_STATE_INACTIVE)
        *state_token_ptr = SD_TRANSFER_STATE_INACTIVE;
}

/* objects[i] makes loop.c give the record giv and the mask cursor swapped
   registers; the byte offset from the state base keeps retail allocation. */
#define SD_SECONDARY_STATE_AT_OFFSET(state, offset) \
    ((SDSecondaryState *)((u8 *)(state) + (offset)))

/* Second pass over the 0x28-byte records at gSD_pSecondaryState: for each record
 * whose +0x18D counter is set and whose channel reports SPU_ON_ENV_OFF, either
 * bumps the counter or, once it reaches 2, requests key-off through the
 * SpuSetKey/SpuGetKeyStatus pair, clears the counter and ORs the channel mask
 * into one final SpuSetKey call. No caller was found in SLUS_014.11, SU.MRG
 * or WA_MRG.MRG. SD_UpdateSecondaryObjectVoices uses the same SPU_ON_ENV_OFF
 * trigger and stage threshold inline, but its cleanup differs: it keys off
 * through SD_KeyOffSecondaryObject, which skips records whose channel_index
 * is >= SD_SEQUENCE_CHANNEL_COUNT and clears key_held rather than the stage;
 * the stage is cleared, channel voice usage decremented and the record
 * detached only later, in its separate SPU_OFF cleanup path. This helper has
 * no channel guard, leaves key_held and channel ownership untouched, and
 * clears the stage right after its own key-off loop. */
void SD_KeyOffSilentSecondaryObjects(void) {
    SDSecondaryState *base;
    s32 i;
    s32 mask;
    s32 off;
    s32 *tbl;
    s32 *q;
    s32 v;
    s32 key_off_env_on;
    u8 b;
    SDSecondaryState *p;
    s32 z;
    SDSecondaryState *b1;
    SDSecondaryState *b2;

    mask = 0;
    base = gSD_pSecondaryState;
    i = 0;
    mask = i;
    z = 0;
    if (base->object_count > z) {
        key_off_env_on = SPU_OFF_ENV_ON;
        off = 0;
        tbl = gSD_adwVoiceMasks;
        do {
            if (SD_SECONDARY_STATE_AT_OFFSET(base, off)
                    ->objects[0].lifecycle_stage != 0) {
                if (SpuGetKeyStatus(*tbl) == SPU_ON_ENV_OFF) {
                    b1 = gSD_pSecondaryState;
                    p = SD_SECONDARY_STATE_AT_OFFSET(b1, off);
                    b = p->objects[0].lifecycle_stage;
                    if (b >= 2) {
                        mask |= *tbl;
                        q = tbl;
                        do {
                            SpuSetKey(SPU_OFF, *q);
                            v = SpuGetKeyStatus(*q);
                        } while (v != key_off_env_on && v != SPU_OFF);
                        b2 = gSD_pSecondaryState;
                        SD_SECONDARY_STATE_AT_OFFSET(b2, off)
                            ->objects[0].lifecycle_stage = 0;
                    } else {
                        p->objects[0].lifecycle_stage = b + 1;
                    }
                }
            }
            off += sizeof(SDSecondaryObject);
            base = gSD_pSecondaryState;
            tbl++;
        } while (++i < base->object_count);
    }
    if (mask != 0) {
        SpuSetKey(SPU_OFF, mask);
    }
}

int SD_OpenSequence(u8 *input, short vab_id)
{
    unsigned int tag;
    SDSecondaryState *state;
    gSD_pSecondaryState->timer_update_inhibit = 1;
    /* The four-byte file tag is compared as one word load. */
    tag = *(unsigned int *)input;
    if (tag != SD_SEQUENCE_TAG_SEQ && tag != SD_SEQUENCE_TAG_MIDI &&
        tag != SD_SEQUENCE_TAG_KDT && tag != SD_SEQUENCE_TAG_KDT1)
        return -1;
    state = gSD_pSecondaryState;
    if (state->sequence.sequence_open_token == -1) {
        state->sequence.sequence_source = input;
        state->sequence.sequence_open_token = vab_id;
        state->sequence.sequence_state = 2;
        state->timer_update_inhibit = 0;
        return 0;
    }
    state->timer_update_inhibit = 0;
    return -1;
}

void SD_PlaySequence(s32 start, s32 unused)
{
    u8 one = 1;

    gSD_pSecondaryState->timer_update_inhibit = one;
    if (gSD_pSecondaryState->sequence.sequence_open_token == -1) {
        gSD_pSecondaryState->timer_update_inhibit = 0;
        return;
    }
    gSD_pSecondaryState->sequence.sequence_read_limit = 0x10000;
    gSD_pSecondaryState->sequence.sequence_read_base = gSD_pSecondaryState->sequence.sequence_source;
    SD_StartSequenceTracks();

    start = (u16)start;
    if ((start & 0xFF) == 0) {
        gSD_pSecondaryState->sequence.sequence_state = 4;
    } else {
        gSD_pSecondaryState->shared_track_updates_enabled = one;
        gSD_pSecondaryState->sequence.sequence_state = 1;
    }
    gSD_pSecondaryState->timer_update_inhibit = 0;
}

/* The play branch of SD_PlaySequence plus a fast-forward target in
   fast_forward_target_address. While that is nonzero, SD_ProcessSequenceTracks dispatches each
   track's next event group on every tick instead of waiting out its delta
   time, and clears it once a track's read address (sequence_read_base + pos) reaches
   target. */
void SD_PlaySequenceFastForward(s32 target)
{
    SDSecondaryState *state;

    gSD_pSecondaryState->timer_update_inhibit = 1;
    state = gSD_pSecondaryState;
    if (state->sequence.sequence_open_token == -1) {
        state->timer_update_inhibit = 0;
    } else {
        SDSecondaryState *first;
        SDSecondaryState *second;
        state->sequence.sequence_read_limit = 0x10000;
        state->sequence.sequence_read_base = state->sequence.sequence_source;
        SD_StartSequenceTracks();
        first = gSD_pSecondaryState;
        first->shared_track_updates_enabled = 1;
        second = gSD_pSecondaryState;
        first->fast_forward_target_address = target;
        second->sequence.sequence_state = 1;
        second->timer_update_inhibit = 0;
    }
}

void SD_StopSequence(s32 unused)
{
    SDSecondaryState *initial = gSD_pSecondaryState;

    if (initial->sequence.sequence_open_token != -1) {
        SDSecondaryState *state;
        SDSecondaryState *other;
        initial->timer_update_inhibit = 1;
        SD_ResetSequenceTracks();
        SD_ResetSecondaryPlayback();
        state = gSD_pSecondaryState;
        state->timer_update_inhibit = 0;
        other = gSD_pSecondaryState;
        state->sequence.spatial_level_right = SD_SECONDARY_LEVEL_MAX;
        state->sequence.spatial_level_left = SD_SECONDARY_LEVEL_MAX;
        state->sequence.sequence_state = 2;
        other->shared_track_updates_enabled = 0;
    }
}

void SD_CloseSequence(s32 unused)
{
    SDSecondaryState *state = gSD_pSecondaryState;

    if (state->sequence.sequence_open_token != -1) {
        state->timer_update_inhibit = 1;
        gSD_pSecondaryState->sequence.sequence_open_token = -1;
        gSD_pSecondaryState->sequence.sequence_state = 0;
        gSD_pSecondaryState->sequence.spatial_level_right = SD_SECONDARY_LEVEL_MAX;
        gSD_pSecondaryState->sequence.spatial_level_left = SD_SECONDARY_LEVEL_MAX;
        gSD_pSecondaryState->timer_update_inhibit = 0;
    }
}

/*
 * The 224-byte mute routine matches with the existing G8 split profile and
 * DATA state-pointer view. Reusing control for the channel/activity tests
 * and packet mask prevents an extra loop-invariant register and stack save.
 * The counter scope preserves register allocation; the state/count snapshot
 * keeps the voice cursor increment before the absolute state reload.
 *
 * Read the object fields as state->objects[i] off the reloaded state base;
 * a separate typed object pointer changes the loop code.
 * Only the selected SpuVoiceAttr fields are initialized, as its mask requires.
 */
void SD_MuteSecondaryVoices(void)
{
    SpuVoiceAttr packet;
    SDSecondaryState *initial = gSD_pSecondaryState;
    SDSecondaryState *state;
    s32 i;
    s32 *voice_bits;

    initial->timer_update_inhibit = 1;
    state = gSD_pSecondaryState;
    if (state->object_count > 0) {
        s32 count;

        do {
            i = 0;
        } while (0);
        voice_bits = gSD_adwVoiceMasks;
        do {
            u32 control = state->objects[i].channel_index >> 4;

            if (control == 0) {
                control = state->objects[i].lifecycle_stage;
                if (control != 0) {
                    control = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
                    packet.mask = control;
                    packet.voice = *voice_bits;
                    packet.volume.left = 0;
                    packet.volume.right = 0;
                    SpuSetVoiceAttr(&packet);
                }
            }
            voice_bits++;
            do {
                state = gSD_pSecondaryState;
                count = state->object_count;
            } while (0);
            i++;
        } while (i < count);
    }
    state = gSD_pSecondaryState;
    state->sequence.sequence_state = 4;
    state->timer_update_inhibit = 0;
}

/*
 * The 240-byte restoration routine matches with the existing G8 split
 * profile and DATA state-pointer view. Reusing control for the two tests
 * and packet mask prevents an extra loop-invariant register and stack save.
 * The counter scope preserves allocation; the state/count snapshot keeps
 * the bound load before the index increment.
 *
 * Object fields are read as state->objects[i] off the reloaded state base.
 * The SDK mask selects only the initialized voice and cached volume fields.
 */

void SD_RestoreSecondaryVoiceVolumes(void)
{
    SpuVoiceAttr packet;
    SDSecondaryState *initial = gSD_pSecondaryState;
    SDSecondaryState *state;
    s32 i;
    s32 *voice_bits;

    initial->timer_update_inhibit = 1;
    state = gSD_pSecondaryState;
    if (state->object_count > 0) {
        s32 count;

        do {
            i = 0;
        } while (0);
        voice_bits = gSD_adwVoiceMasks;
        do {
            u32 control = state->objects[i].channel_index >> 4;

            if (control == 0) {
                control = state->objects[i].lifecycle_stage;
                if (control != 0) {
                    control = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
                    packet.mask = control;
                    packet.voice = *voice_bits;
                    packet.volume.left = state->objects[i].level_left;
                    packet.volume.right = state->objects[i].level_right;
                    SpuSetVoiceAttr(&packet);
                }
            }
            voice_bits++;
            do {
                state = gSD_pSecondaryState;
                count = state->object_count;
            } while (0);
            i++;
        } while (i < count);
    }
    state = gSD_pSecondaryState;
    state->sequence.sequence_state = 1;
    state->timer_update_inhibit = 0;
}

void SD_SetSecondarySpatialLevels(s32 left, s32 right)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    s32 mode = state->sequence.sequence_state;

    state->sequence.spatial_level_left = left & SD_SECONDARY_LEVEL_MASK;
    state->sequence.spatial_level_right = right & SD_SECONDARY_LEVEL_MASK;
    if (mode != 2)
        SD_UpdateSecondaryObjectVolumes();
}

void SD_SetSecondaryMasterLevels(s16 left, s16 right)
{
    SDSecondaryState *state = gSD_pSecondaryState;

    state->master_level_left = left;
    state->master_level_right = right;
    if (state->sequence.sequence_state != 2)
        SD_UpdateSecondaryObjectVolumes();
}

s16 SD_PollSequenceState(void)
{
    s32 value;

    if (gSD_pSecondaryState->sequence.sequence_state == 1) {
        value = (s16)SD_GetSequenceStatus();
        if (value == 3)
            gSD_pSecondaryState->sequence.sequence_state = value;
    }
    return gSD_pSecondaryState->sequence.sequence_state;
}
