#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_H

#include "../types.h"

/* The sound output unit's exported entry points.
 *
 * SD_SetOutputType is deliberately absent: sound.h already declares it, and
 * one function belongs in one header.
 *
 * func_8004703C returns the driver's flag word sign-extended from a
 * halfword, so it is s32 rather than the u32 its callers assumed. Every
 * observed use masks the result, so the two spellings agree in practice --
 * but only one of them matches the definition. */
s32 func_8004703C(void);
void func_80047050(void);
void func_800470B0(s32 first, s32 second, s32 third, s32 fourth);
void func_800471D0(s32, s32, s32, s32, s32, s32);
void func_800472A8(s32 arg0);
void func_80047314(u32 value);
void func_8004733C(s32 arg0, s32 arg1);
void func_800473CC(u32 value);
void func_800473F0(u16 flags, s32 value);
void func_80047430(s32 value, s32 flag);

#endif
