#ifndef MEMORIES_DECOMP_MODEL_PACKET_HANDLERS_H
#define MEMORIES_DECOMP_MODEL_PACKET_HANDLERS_H

#include "../types.h"

/* Interpolates one animation track; func_8005C768 returns it for type 0x2019. */
u8 *func_8005CEF0(u8 **animation);

/* Installs the packet handler for one model's animation stream.
 *
 * func_8005C768 maps a packet's type word to the routine that consumes it,
 * returning it as a plain address because the slot it is written into is
 * untyped. It answers with the library's own GsU_00000000 for anything it
 * does not recognise, so callers get a valid handler rather than a null. */
void func_8005C6A0(s32 *object, u8 *entry);
void *func_8005C768(u32 value);

#endif
