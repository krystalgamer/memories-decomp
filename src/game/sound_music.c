#include "../types.h"
#include "../psyq/libspu.h"
#include "file_constants.h"
#include "sound.h"
#include "sound_driver.h"
#include "sound_output_state.h"
#include "sound_secondary.h"
#include "sound_music.h"

/* Sound driver initialisation after the SPU is up: enables reverb in studio-A
 * mode at full depth, clears and seeds the driver's level and track fields,
 * points the music track at its buffer at 0x801EA800 with an empty header,
 * and finishes through SD_SetSequenceTickMode, SD_SetSecondaryObjectCount and
 * SD_EnableSecondarySequenceUpdates.
 * sound_driver.c (SD_InitState) calls it once. */
void SD_InitSecondaryRuntime(void)
{
    SpuReverbAttr reverb_attr;
    /* g_SDValue is reloaded three times, as retail does. The first load gets
       its own pointer so each lives only as long as its stores. */
    SDValue *bgm_state;
    SDValue *sequence_state;
    SDValue *music_state;

    SpuReserveReverbWorkArea(SPU_ON);
    SpuSetReverb(SPU_ON);
    reverb_attr.mask = SPU_REV_MODE | SPU_REV_DEPTHL | SPU_REV_DEPTHR;
    reverb_attr.mode = SPU_REV_MODE_STUDIO_A;
    reverb_attr.depth.left = 0x7FFF;
    reverb_attr.depth.right = 0x7FFF;
    SpuSetReverbModeParam(&reverb_attr);
    sequence_state = g_SDValue;
    sequence_state->s_wSD_SEQ_FADE_LEVEL = 0;
    sequence_state->s_wSD_SEQ_FADE_STEP = 0;
    sequence_state->s_bSD_SEQ_FADE_TARGET = 0;
    bgm_state = g_SDValue;
    bgm_state->s_wSD_BGM_FADE_LEVEL = 0xFF;
    bgm_state->s_bSD_BGM_FADE_TARGET = 0xFF;
    music_state = g_SDValue;
    bgm_state->s_wSD_BGM_FADE_STEP = 0;
    music_state->s_wSD_OPEN_MUSIC_PACKAGE = -1;
    music_state->s_wSD_MUSIC_VAB_ID = -1;
    music_state->s_wSD_OPEN_SEQUENCE_COMMAND = -1;
    music_state->s_wSD_SEQ_ACCESS = -1;
    music_state->s_pwSD_MUSIC_TRACK = (SDMusicPackage *)0x801EA800;
    music_state->s_pstSD_MUSIC_TABLE = (SDMusicTableEntry *)0x801E2000;
    music_state->s_pwSD_MUSIC_TRACK->package_id = 0xFFFF;
    music_state->s_pwSD_MUSIC_TRACK->sequence_count = 0;
    music_state->s_pwSD_MUSIC_TRACK->field_04 = 0;
    music_state->s_pwSD_MUSIC_TRACK->field_08 = 0;
    music_state->s_pwSD_MUSIC_TRACK->spu_address = 0x40000;
    SD_SetSequenceTickMode(2);
    SD_SetSecondaryObjectCount(0x14);
    SD_EnableSecondarySequenceUpdates();
}

/* Retires the driver's two pending requests, in the same *g_SDValue struct
   whose s_wSD_BGM_FADE_STEP/s_bSD_BGM_FADE_TARGET SD_StartBgmFadeCopy and SD_StartBgmFade
   write.

   If the 0x157E slot is active, it stops that sequence through SD_StopSequence
   when the 0x80 flag bit is set (clearing the bit), releases the slot through
   SD_CloseSequence, and marks it inactive. Separately, if the VAB slot at 0x157A
   is ready for release, it resets that VAB transfer state and marks
   0x157A/0x1578 inactive. Always zeroes 0x1586/0x1588. */

void SD_ResetMusicState(void) {
    if (g_SDValue->s_wSD_SEQ_ACCESS != -1) {
        if (g_SDValue->s_wSD_STATUS & 0x80) {
            SD_StopSequence(g_SDValue->s_wSD_SEQ_ACCESS);
            g_SDValue->s_wSD_STATUS =
                g_SDValue->s_wSD_STATUS & 0xFF7F;
        }
        SD_CloseSequence(g_SDValue->s_wSD_SEQ_ACCESS);
        g_SDValue->s_wSD_SEQ_ACCESS = -1;
    }

    if (g_SDValue->s_wSD_MUSIC_VAB_ID == 0) {
        SD_ResetVabTransferState(0);
        g_SDValue->s_wSD_MUSIC_VAB_ID = -1;
        g_SDValue->s_wSD_OPEN_MUSIC_PACKAGE = -1;
    }

    g_SDValue->s_wSD_SEQ_FADE_STEP = 0;
    g_SDValue->s_wSD_SEQ_FADE_LEVEL = 0;
}

void SD_StartBgmFadeCopy(s16 value, u8 flag)
{
    g_SDValue->s_wSD_BGM_FADE_STEP = value;
    g_SDValue->s_bSD_BGM_FADE_TARGET = flag;
}

void SD_StartBgmFade(s16 step, u8 target)
{
    g_SDValue->s_wSD_BGM_FADE_STEP = step;
    g_SDValue->s_bSD_BGM_FADE_TARGET = target;
}

s32 SD_IsBgmFadeActive(void)
{
    return g_SDValue->s_wSD_BGM_FADE_STEP != 0;
}

s32 SD_RequestMusicPackageLoad(s16 command_id, s32 unused) {
    SDValue *p = g_SDValue;

    if (p->s_pwSD_MUSIC_TRACK->package_id != command_id >> 4) {
        SDValue *q;
        SDMusicTableEntry *entry;
        s32 off;

        SD_CancelMusicRequests();
        SD_ResetMusicState();
        SpuSetReverbModeType(SPU_REV_MODE_OFF);

        q = g_SDValue;
        q->s_wSD_STATUS |= 2;
        off = ((command_id & 0xFF0) >> 4) * sizeof(SDMusicTableEntry);
        /* Byte-offset addition keeps the retail addu operand order. */
        entry = (SDMusicTableEntry *)(off + (s32)q->s_pstSD_MUSIC_TABLE);

        SD_EnqueueSpuRamTransfer(entry->spu_address, (s32)q->s_pwSD_MUSIC_TRACK,
                                 entry->sector_offset,
                                 entry->spu_sector_count << FILE_SECTOR_SHIFT,
                                 entry->ram_sector_count << FILE_SECTOR_SHIFT, 0x20);

        g_SDValue->s_pwSD_MUSIC_TRACK->package_id = 0xFFFF;
    }

    return 0;
}

void SD_RequestBgmPlaybackDefaultStep(s32 command_id)
{
    SD_RequestBgmPlayback((s16)command_id, g_SDValue->s_bSD_BGM_FADE_STEP_DEFAULT);
}

void SD_RequestBgmPlayback(s32 command_id, s32 step)
{
    SDCommand command;
    s16 signed_command = command_id;

    if (signed_command < 0) {
        SD_ResetMusicState();
        return;
    }
    if (g_SDValue->s_pwSD_MUSIC_TRACK->package_id != (signed_command >> 4))
        SD_RequestMusicPackageLoad(signed_command, 1);
    command.command = SD_COMMAND_BGM_PLAY;
    command.value = command_id;
    command.args.fade.step = (s16)step;
    SD_EnqueueCommand(&command);
    g_SDValue->s_wSD_BGM_FADE_STEP = step;
    g_SDValue->s_bSD_BGM_FADE_TARGET = 255;
}

void SD_Init(void)
{
    SD_CancelMusicRequests();
    SD_ResetMusicState();
    SpuSetReverbModeType(SPU_REV_MODE_OFF);
}

void SD_OpenMusicVab(void)
{
    SDValue *open_state = g_SDValue;

    if (open_state->s_wSD_MUSIC_VAB_ID != 0) {
        SDMusicPackage *package = open_state->s_pwSD_MUSIC_TRACK;
        s32 vab_result =
            SD_VabOpenHead(&package->vab_header, 0, package->spu_address);

        g_SDValue->s_wSD_MUSIC_VAB_ID = vab_result;
        if ((vab_result << 16) != 0) {
            g_SDValue->s_wSD_OPEN_MUSIC_PACKAGE = -1;
            g_SDValue->s_wSD_MUSIC_VAB_ID = -1;
            return;
        }
    }
    {
        register SDValue *publish_state = g_SDValue;
        register SDMusicPackage *package = publish_state->s_pwSD_MUSIC_TRACK;
        publish_state->s_wSD_OPEN_MUSIC_PACKAGE = package->package_id;
    }
}

void SD_SetMusicTrackBuffer(SDMusicPackage *track_buffer)
{
    SD_ResetMusicState();
    if (track_buffer != 0)
        g_SDValue->s_pwSD_MUSIC_TRACK = track_buffer;
    else
        g_SDValue->s_pwSD_MUSIC_TRACK = (SDMusicPackage *)0x801EA800;
    g_SDValue->s_pwSD_MUSIC_TRACK->package_id = 0xFFFF;
}

void SD_ResetMusicTrackBuffer(void)
{
    SDMusicPackage *default_track_buffer;

    SD_ResetMusicState();
    default_track_buffer = (SDMusicPackage *)0x801EA800;
    g_SDValue->s_pwSD_MUSIC_TRACK = default_track_buffer;
    default_track_buffer->package_id = 0xFFFF;
}
