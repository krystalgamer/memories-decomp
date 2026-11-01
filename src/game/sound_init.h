#ifndef MEMORIES_DECOMP_SOUND_INIT_H
#define MEMORIES_DECOMP_SOUND_INIT_H

#include "../types.h"
#include "../psyq/libspu_internal.h"

extern u8 gSD_szBgmFileName[];
extern u8 gSD_szSeFileName[];
extern u8 gSD_szMasterXaFileName[];

void SD_RequestBgmPlaybackDefaultStep(s32 command_id);
void SD_RequestBgmPlayback(s32 command_id, s32 step);
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
