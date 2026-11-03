#ifndef GAME_LIBRARY_REFERENCE_Y_H
#define GAME_LIBRARY_REFERENCE_Y_H

#include "../types.h"

/* Signed coordinate views in the Library overlay work area. The dispatcher
 * compares the copied slot coordinate at 0x80181012 with its reference at
 * 0x80181002 while converging the shared view state. */
extern s16 gLibrary_sCapturedReferenceY __attribute__((section(".data")));
extern s16 gLibrary_sSampledReferenceY __attribute__((section(".data")));

#endif
