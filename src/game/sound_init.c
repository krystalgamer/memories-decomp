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

    gSD_pSecondaryState->flag_0500 = 0;
    gSD_pSecondaryState->flag_0501 = 0;
    gSD_pSecondaryState->flag_0502 = 0;
    state = gSD_pSecondaryState;
    state->object_count = SD_SECONDARY_OBJECT_COUNT;
    state->field_0814 = 0;
    other = gSD_pSecondaryState;
    state->field_0512 = 127;
    state->master_level_left = 127;
    state->master_level_right = 127;
    other->field_0815 = 0;
    third = gSD_pSecondaryState;
    third->transfer.field_0019 = 127;
    fourth = gSD_pSecondaryState;
    third->transfer.field_0000 = SD_TRANSFER_STATE_INACTIVE;
    fourth->transfer.field_001A = 127;
    fifth = gSD_pSecondaryState;
    fifth->transfer.field_001B = 64;
    final = gSD_pSecondaryState;
    final->field_07E0 = -1;
    final->field_07E2 = 0;
    final->field_07E6 = SD_SECONDARY_LEVEL_MAX;
    final->field_07E4 = SD_SECONDARY_LEVEL_MAX;
    final->field_080C = 0;
    final->field_0810 = 0;
    final->field_081C = 0x1000;
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
    s32 value = gSD_pSecondaryState->field_081C;

    if (value > 0) {
        if (value < 4)
            SD_OpenSequenceTimerEvent();
    }
    gSD_pSecondaryState->field_0814 = 1;
}

void SD_SetSequenceTickMode(s32 value)
{
    gSD_pSecondaryState->field_081C = value;
}

void SD_UpdateSecondarySequenceIfEnabled(void)
{
    if (gSD_pSecondaryState->field_0814)
        SD_UpdateSecondarySequence();
}

void SD_EnableSecondaryPanning(void)
{
    gSD_pSecondaryState->field_0815 = 0;
}

void SD_DisableSecondaryPanning(void)
{
    gSD_pSecondaryState->field_0815 = 1;
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
