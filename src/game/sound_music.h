#ifndef MEMORIES_DECOMP_SOUND_MUSIC_H
#define MEMORIES_DECOMP_SOUND_MUSIC_H

#include "../types.h"

/* One eight-byte entry in the table selected through SDValue.music_table.
 * Only SD_RequestMusicPackageLoad currently establishes its field layout. */
typedef struct {
    u8 spu_sector_count;
    u8 ram_sector_count;
    u16 sector_offset;
    s32 spu_address;
} SDMusicTableEntry;

typedef char SDMusicTableEntry_size_must_be_8[
    sizeof(SDMusicTableEntry) == 8 ? 1 : -1
];
typedef char SDMusicTableEntry_transfer_counts_must_match[
    OFFSETOF(SDMusicTableEntry, spu_sector_count) == 0 &&
    OFFSETOF(SDMusicTableEntry, ram_sector_count) == 1 ? 1 : -1
];
typedef char SDMusicTableEntry_sector_offset_must_be_2[
    OFFSETOF(SDMusicTableEntry, sector_offset) == 2 ? 1 : -1
];
typedef char SDMusicTableEntry_spu_address_must_be_4[
    OFFSETOF(SDMusicTableEntry, spu_address) == 4 ? 1 : -1
];

void SD_InitSecondaryRuntime(void);

/* Retires the driver's music resources: stops and closes the sequence when
   s_wSD_SEQ_ACCESS is active, releases a successful s_wSD_MUSIC_VAB_ID, then
   clears the pending pair at s_wSD_SEQ_FADE_LEVEL/s_wSD_SEQ_FADE_STEP. */
void SD_ResetMusicState(void);

/* The two BGM fade setters. Both store a signed per-tick step in s_wSD_BGM_FADE_STEP
 * and a target level in s_bSD_BGM_FADE_TARGET; SD_UpdateFades moves s_wSD_BGM_FADE_LEVEL by the
 * step until it reaches the target. Their bodies are identical -- retail has
 * two symbols for the same two stores. SD_BGMFade reaches SD_StartBgmFade;
 * SD_StartBgmFadeCopy is reached only from SD_BGMFadeCopy, which
 * has no caller. */
void SD_StartBgmFadeCopy(s16 value, u8 flag);
void SD_StartBgmFade(s16 step, u8 target);

/* The state query reads only g_SDValue. */
s32 SD_IsBgmFadeActive(void);

void SD_RequestBgmPlaybackDefaultStep(s32 command_id);
void SD_RequestBgmPlayback(s32 command_id, s32 step);
void SD_Init(void);
void SD_OpenMusicVab(void);
void SD_SetMusicTrackBuffer(u16 *track_buffer);
void SD_ResetMusicTrackBuffer(void);

#endif
