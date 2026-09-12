#ifndef MEMORIES_DECOMP_SOUND_VOICE_PAN_H
#define MEMORIES_DECOMP_SOUND_VOICE_PAN_H

#include "../types.h"

/* Updates active effect voices for a direct or bank-mapped sound ID.
 * Bit 0x8000 routes to the output-pan path instead. Pan arithmetic uses the
 * signed low halfword; the public arguments retain their full word ABI. */
void func_80048768(s32 sound_id, s32 pan);

#endif
