#include "../types.h"
#include "fade.h"

/* Resident .sbss storage from 0x8009B14A to 0x8009B14C, previously part of the
   generated zero-filled BSS image (#2602). Unnamed bytes keep address names. */
u8 gFade_bTargetBlue __attribute__((section(".sbss"))) = 0;
u8 gFade_bTargetGreen __attribute__((section(".sbss"))) = 0;
