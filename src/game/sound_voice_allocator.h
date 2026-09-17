#ifndef MEMORIES_DECOMP_SOUND_VOICE_ALLOCATOR_H
#define MEMORIES_DECOMP_SOUND_VOICE_ALLOCATOR_H

#include "../types.h"

/* Allocates one of the four sound-effect voice slots, whose SPU voice numbers
 * begin at SD_VOICE_SLOT_FIRST_VOICE. */
void func_800482B0(s32 id, s16 pitch, u8 volume, s16 pan, u32 mode, u8 value);

#endif
