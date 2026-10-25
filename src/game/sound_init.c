#include "../types.h"
#include "sd_reset_music_state.h"
#include "sd_reset_secondary_playback_thunk.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_output.h"
#include "sound_init.h"
#include "sound_transfer_lifecycle.h"
#include "../unmatched.h"

void SD_RequestBgmPlaybackDefaultStep(s32 value)
{
    SD_RequestBgmPlayback((s16)value, g_SDValue->field_164B);
}

void SD_RequestBgmPlayback(s32 value, s32 data)
{
    SDCommand command;
    s16 small = value;

    if (small < 0) {
        SD_ResetMusicState();
        return;
    }
    if (*g_SDValue->music_track != (small >> 4))
        SD_RequestMusicPackageLoad(small, 1);
    command.command = 0x48;
    command.field_0002 = value;
    command.field_0008 = (s16)data;
    SD_EnqueueCommand(&command);
    g_SDValue->bgm_fade_step = data;
    g_SDValue->bgm_fade_target = 255;
}

void SD_Init(void)
{
    SD_CancelMusicRequests();
    SD_ResetMusicState();
    SpuSetReverbModeType(SPU_REV_MODE_OFF);
}

void SD_OpenMusicVab(void)
{
    SDValue *state = g_SDValue;

    if (state->music_vab_open_result != 0) {
        SDMusicPackage *entry = (SDMusicPackage *)state->music_track;
        s32 result =
            SD_VabOpenHead(&entry->vab_header, 0, entry->spu_address);

        g_SDValue->music_vab_open_result = result;
        if ((result << 16) != 0) {
            g_SDValue->opened_music_package_id = -1;
            g_SDValue->music_vab_open_result = -1;
            return;
        }
    }
    {
        register SDValue *final = g_SDValue;
        register u16 *entry = final->music_track;
        final->opened_music_package_id = *entry;
    }
}

void SD_SetMusicTrackBuffer(u16 *track)
{
    SD_ResetMusicState();
    if (track != 0)
        g_SDValue->music_track = track;
    else
        g_SDValue->music_track = (u16 *)0x801EA800;
    *g_SDValue->music_track = 0xFFFF;
}

void SD_ResetMusicTrackBuffer(void)
{
    u16 *entry;

    SD_ResetMusicState();
    entry = (u16 *)0x801EA800;
    /* Stored through the member's address as void **: a plain member store
       (or a u16 ** cast, which fold turns back into one) picks a different
       register for the pointer and changes seven words. Measured. */
    *(void **)&g_SDValue->music_track = entry;
    entry[0] = 0xFFFF;
}

void SD_ResetSecondaryState(void)
{
    SDSecondaryState *state;
    SDSecondaryState *other;
    SDSecondaryState *third;
    SDSecondaryState *fourth;
    SDSecondaryState *fifth;
    SDSecondaryState *final;

    gSD_pSecondaryState->timer_update_inhibit = 0;
    gSD_pSecondaryState->update_in_progress = 0;
    gSD_pSecondaryState->shared_track_updates_enabled = 0;
    state = gSD_pSecondaryState;
    state->object_count = SD_SECONDARY_OBJECT_COUNT;
    state->sequence_updates_enabled = 0;
    other = gSD_pSecondaryState;
    state->sequence_level = 127;
    state->master_level_left = 127;
    state->master_level_right = 127;
    other->pan_center_override = 0;
    third = gSD_pSecondaryState;
    third->transfer.field_0019 = 127;
    fourth = gSD_pSecondaryState;
    third->transfer.state_token = SD_TRANSFER_STATE_INACTIVE;
    fourth->transfer.field_001A = 127;
    fifth = gSD_pSecondaryState;
    fifth->transfer.vab_pan = 64;
    final = gSD_pSecondaryState;
    final->sequence_open_token = -1;
    final->sequence_state = 0;
    final->spatial_level_right = SD_SECONDARY_LEVEL_MAX;
    final->spatial_level_left = SD_SECONDARY_LEVEL_MAX;
    final->fast_forward_target_address = 0;
    final->last_track_cursor_address = 0;
    final->sequence_tick_mode = 0x1000;
}

void SD_InitSecondaryWorkArea(s32 *data)
{
    u32 i = 0;

    gSD_pSecondaryState = (SDSecondaryState *)data;
    do {
        *data = 0;
        i++;
        data++;
    } while (i < 530);
    SpuInit();
    SD_ResetSecondaryPlaybackThunk();
    SD_ResetSecondaryState();
}

void SD_EnableSecondarySequenceUpdates(void)
{
    s32 value = gSD_pSecondaryState->sequence_tick_mode;

    if (value > 0) {
        if (value < 4)
            SD_OpenSequenceTimerEvent();
    }
    gSD_pSecondaryState->sequence_updates_enabled = 1;
}

void SD_SetSequenceTickMode(s32 value)
{
    gSD_pSecondaryState->sequence_tick_mode = value;
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
s32 SD_SetSecondaryObjectCount(u32 a0) {
    u8 x = a0 & 0xFF;

    if (x >= SD_SECONDARY_OBJECT_COUNT + 1) {
        return 0xFF;
    }
    if (x == 0) {
        return 0xFF;
    }
    gSD_pSecondaryState->object_count = x;
    return x;
}

void SD_ResetSecondaryRuntime(void)
{
    s32 value;

    SD_ResetSecondaryPlaybackThunk();
    value = gSD_pSecondaryState->sequence_tick_mode;
    if (value > 0) {
        if (value < 4)
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
 * `vab` points at a "pBAV" header; both callers pass music_track + 0x50,
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
 * The header is taken as SDVabHeader *, which this header now declares; the
 * transfer record still keeps it as a byte pointer, so that one field is the
 * only cast left.
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
    entry->vab_header = (u8 *)vab;
    entry->header_size_bytes = (vab->program_count << 9) + 0xA20;
    entry->body_size_bytes = vab->file_size - entry->header_size_bytes;
    entry->vab_master_volume = vab->master_volume;
    entry->vab_pan = vab->pan;
    entry->spu_body_base = (u8 *)spu_addr;
    return 0;
}

s32 SD_VabTransBody(s32 value, s16 expected)
{
    register int saved;
    SDSecondaryState *state = gSD_pSecondaryState;
    short current = state->transfer.state_token;

    if (current != expected)
        return SD_TRANSFER_ERROR;
    saved = value;
    {
        SDSecondaryTransfer *entry = &state->transfer;
        SpuSetTransferStartAddr((u32)entry->spu_body_base);
        if (SpuWrite((u8 *)saved, (u32)entry->body_size_bytes) !=
            entry->body_size_bytes)
            return SD_TRANSFER_ERROR;
        entry->full_body_source_address = saved;
    }
    return current;
}

/* Validates the caller's state token against the pending transfer's
   discriminant, clamps the requested byte count to what remains in the
   window, submits it to the SPU transfer entrypoint, and advances the
   consumed-byte counter. Returns the state token once the window fills,
   SD_TRANSFER_INCOMPLETE while more remains, or SD_TRANSFER_ERROR on a
   state mismatch or short transfer. */
s32 SD_VabTransBodyChunk(u8 *rec, s32 count, s32 state) {
    SDSecondaryState *v1 = gSD_pSecondaryState;
    SDSecondaryTransfer *s1;
    s32 result;

    if (v1->transfer.state_token != (s16)state) {
        return SD_TRANSFER_ERROR;
    }

    s1 = &v1->transfer;
    SpuSetTransferStartAddr(
        (u32)(s1->spu_body_base + v1->bytes_consumed)
    );

    {
        s32 remaining = s1->body_size_bytes - gSD_pSecondaryState->bytes_consumed;
        if ((u32)remaining < (u32)count) {
            count = remaining;
        }
    }

    result = SpuWrite(rec, (u32)count);
    if (result != count) {
        return SD_TRANSFER_ERROR;
    }

    {
        u32 total = gSD_pSecondaryState->bytes_consumed + count;
        gSD_pSecondaryState->bytes_consumed = total;
        if (total >= (u32)s1->body_size_bytes) {
            return (s16)state;
        }
    }

    return SD_TRANSFER_INCOMPLETE;
}

s16 SD_VabTransCompleted(s16 wait)
{
    if (wait == 0)
        return SpuIsTransferCompleted(SPU_TRANSFER_PEEK);
    return SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
}

void SD_ResetVabTransferState(void)
{
    s16 *value = &gSD_pSecondaryState->transfer.state_token;

    if (*value != SD_TRANSFER_STATE_INACTIVE)
        *value = SD_TRANSFER_STATE_INACTIVE;
}
