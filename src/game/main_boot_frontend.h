#ifndef MEMORIES_DECOMP_MAIN_BOOT_FRONTEND_H
#define MEMORIES_DECOMP_MAIN_BOOT_FRONTEND_H
#include "../types.h"

void Main_RunBootSequence(s32 mode);

/* PAL resident helpers shared by boot and the language-options overlay. */
void Main_InitDebugFont(void);
void Main_RequestLanguageBootPackage(s32 language, s32 wait);

s32 Main_RunFrontendLoop(void);

/* The two entries of the boot module (module ID 0x16, WA_MRG 0xB61800,
 * loaded at 0x80168000) that the boot sequence calls: the first reads the
 * BIOS region byte and initializes the module, the second steps its 20-state
 * controller until it returns zero. The module is not decompiled; see
 * notes/boot-frontend-sequence.md. */
void func_801680F4(void);
s32 func_80168160(s32);

#endif
