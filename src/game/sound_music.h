#ifndef MEMORIES_DECOMP_SOUND_MUSIC_H
#define MEMORIES_DECOMP_SOUND_MUSIC_H

#include "../types.h"
#include "sound_transfer_lifecycle.h"

/* Header of the music package at s_pwSD_MUSIC_TRACK. SD_StepActiveCommand and
 * SD_OpenMusicVab pass the embedded pBAV header and its SPU destination to
 * SD_VabOpenHead; command 72 opens sequence i at index_records[i][0] * 16
 * bytes into the package (only each four-byte record's low halfword is read).
 * SD_ResetMusicState marks the empty buffer with package_id 0xFFFF. */
typedef struct {
    u16 package_id;
    u16 sequence_count;
    s32 field_04;
    s32 field_08;
    s32 spu_address;
    u16 index_records[16][2];
    SDVabHeader vab_header;
} SDMusicPackage;

typedef char SDMusicPackage_spu_address_offset_must_be_0x0C[
    OFFSETOF(SDMusicPackage, spu_address) == 0x0C ? 1 : -1
];
typedef char SDMusicPackage_index_records_offset_must_be_0x10[
    OFFSETOF(SDMusicPackage, index_records) == 0x10 ? 1 : -1
];
typedef char SDMusicPackage_vab_header_offset_must_be_0x50[
    OFFSETOF(SDMusicPackage, vab_header) == 0x50 ? 1 : -1
];
typedef char SDMusicPackage_vab_header_size_must_be_0x20[
    sizeof(((SDMusicPackage *)0)->vab_header) == 0x20 ? 1 : -1
];

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
void SD_SetMusicTrackBuffer(SDMusicPackage *track_buffer);
void SD_ResetMusicTrackBuffer(void);

#endif
