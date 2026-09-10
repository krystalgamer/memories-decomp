#ifndef MEMORIES_DECOMP_SOUND_STATE_CONTROL_H
#define MEMORIES_DECOMP_SOUND_STATE_CONTROL_H

#include "../types.h"

/* Stores `value` in the secondary sound state's byte 0x509 and mirrors
 * whether it is non-zero into flag_0500. */
void func_8004695C(s32 value);

/* Takes the LBAs of the three sound data sources, in order SE, BGM and XA
 * (Sound_InitFrontend passes them from gFile_anLba). A zero LBA clears that
 * source's bit in g_SDValue->flags_004A (0x01, 0x02 and 0x40). It also zeroes
 * field_003C and raises 0xA in flags_0040. */
void func_80046990(s32 first, s32 second, s32 third);

#endif
