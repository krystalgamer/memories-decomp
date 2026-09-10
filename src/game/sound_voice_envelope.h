#ifndef MEMORIES_DECOMP_SOUND_VOICE_ENVELOPE_H
#define MEMORIES_DECOMP_SOUND_VOICE_ENVELOPE_H

#include "../types.h"

/* The two ADSR writers for one hardware voice, selected by index through
 * D_80011434. Both build their SpuVoiceAttr in the 0x40-byte hole at +0x4C0
 * of the sound state and hand it to SpuSetVoiceAttr; the unit's own comment
 * records why that hole is the block's home.
 *
 * func_8004A6F8 takes the envelope from a tone record, reading adsr1, adsr2
 * and a_mode as the three halfwords at +0x20, +0x22 and +0x24. That record is
 * not a Psy-Q VagAtr -- its adsr1 sits at +0x10 -- so the offsets stay
 * numeric until something names them, as the definition says.
 *
 * func_8004A764 sets the same three from constants instead: both ADSR words
 * zero and a_mode SPU_VOICE_EXPIncN. sound_voice_setup.c calls it per voice,
 * and did so with no declaration in scope at all before this header. */
void func_8004A6F8(s32 index, u8 *tone);
void func_8004A764(s32 index);

#endif
