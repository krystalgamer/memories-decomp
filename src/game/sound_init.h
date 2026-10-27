#ifndef MEMORIES_DECOMP_SOUND_INIT_H
#define MEMORIES_DECOMP_SOUND_INIT_H

#include "../types.h"
#include "../psyq/libspu_internal.h"

extern u8 gSD_szBgmFileName[];
extern u8 gSD_szSeFileName[];
extern u8 gSD_szMasterXaFileName[];

void SD_RequestBgmPlaybackDefaultStep(s32 command_id);
void SD_RequestBgmPlayback(s32 command_id, s32 step);
/* SD_BGMStopWithStep passes the second argument as s16, which the caller-side
 * sign extension depends on. That consumer defines SOUND_INIT_S16_VIEW; every
 * other unit sees only the word prototype above and no assembler label. */
#ifdef SOUND_INIT_S16_VIEW
extern void SD_RequestBgmPlayback_s16(s32 command_id, s16 step)
    asm("SD_RequestBgmPlayback");
#endif
void SD_Init(void);
void SD_OpenMusicVab(void);
void SD_SetMusicTrackBuffer(u16 *track_buffer);
void SD_ResetMusicTrackBuffer(void);
void SD_ResetSecondaryState(void);
void SD_InitSecondaryWorkArea(s32 *work_area_cursor);
void SD_EnableSecondarySequenceUpdates(void);
void SD_SetSequenceTickMode(s32 tick_mode);
void SD_UpdateSecondarySequenceIfEnabled(void);
void SD_EnableSecondaryPanning(void);
void SD_DisableSecondaryPanning(void);

#endif
