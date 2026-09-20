#ifndef MEMORIES_DECOMP_SOUND_VOICE_SELECTION_H
#define MEMORIES_DECOMP_SOUND_VOICE_SELECTION_H

#include "../types.h"

void SD_ApplyVoiceSlotVolume(s32 index);
void SD_UpdateVoiceSlots(void);
void func_80047A68(void);
s32 func_80047AD0(s32 value);
s32 SD_RequestValueLink(u16 value);
void func_80047BB4(u16 *items, s32 count);
s32 func_80047C50(s32 value);
void SD_KeyOffVoice(s32 voice);
void func_80047CC4(s32 value);
void SD_SEStop(s32 id);
s32 func_80047F38(u8 value);

#endif
