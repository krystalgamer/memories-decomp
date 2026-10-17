#ifndef MEMORIES_DECOMP_SOUND_VOICE_SELECTION_H
#define MEMORIES_DECOMP_SOUND_VOICE_SELECTION_H

#include "../types.h"

void SD_ApplyVoiceSlotVolume(s32 index);
void SD_UpdateVoiceSlots(void);
void SD_KeyOffDynamicVoices(void);
s32 SD_ReplaceValueLink(s32 value);
s32 SD_RequestValueLink(u16 value);
void SD_QueueValueLinkList(u16 *items, s32 count);
s32 tent_SD_FilterCdFlaggedId(s32 value);
void SD_KeyOffVoice(s32 voice);
void tent_SD_KeyOffVoiceById(s32 value);
void SD_SEStop(s32 id);
s32 SD_KeyOffVoiceGroup(u8 group);

#endif
