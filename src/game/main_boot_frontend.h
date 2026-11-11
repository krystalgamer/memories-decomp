#ifndef MEMORIES_DECOMP_MAIN_BOOT_FRONTEND_H
#define MEMORIES_DECOMP_MAIN_BOOT_FRONTEND_H
#include "../types.h"

void Main_RunBootSequence(s32 mode);

/* PAL resident helpers shared by boot and the language-options overlay. */
void Main_InitDebugFont(void);
void Main_RequestLanguageBootPackage(s32 language, s32 wait);

s32 Main_RunFrontendLoop(void);

#endif
