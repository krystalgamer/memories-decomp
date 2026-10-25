#ifndef MEMORIES_DECOMP_MAIN_RUN_BOOT_SEQUENCE_H
#define MEMORIES_DECOMP_MAIN_RUN_BOOT_SEQUENCE_H

#include "../types.h"

void Main_RunBootSequence(s32 mode);
extern void func_80043B7C(void);
extern void func_80043BC8(s32 language, s32 wait);

/* PAL resident helpers shared by boot and the language-options overlay. */
extern void Main_InitDebugFont(void) __asm__("func_80043B7C");
extern void Main_RequestLanguageBootPackage(s32 language, s32 wait)
    __asm__("func_80043BC8");

#endif
