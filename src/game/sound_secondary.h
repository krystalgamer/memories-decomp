#ifndef MEMORIES_DECOMP_SOUND_SECONDARY_H
#define MEMORIES_DECOMP_SOUND_SECONDARY_H

#include "../types.h"
#include "../psyq/libspu_internal.h"

void SD_ResetSecondaryState(void);
void SD_InitSecondaryWorkArea(void *work_area);
void SD_EnableSecondarySequenceUpdates(void);
void SD_SetSequenceTickMode(s32 tick_mode);
void SD_UpdateSecondarySequenceIfEnabled(void);
void SD_EnableSecondaryPanning(void);
void SD_DisableSecondaryPanning(void);

void SD_MuteSecondaryVoices(void);
void SD_RestoreSecondaryVoiceVolumes(void);

#endif
