#ifndef YUGIOH_GAME_MEM_CARD_DIALOG_STEPS_H
#define YUGIOH_GAME_MEM_CARD_DIALOG_STEPS_H

#include "../types.h"

/* The memory card dialog step table at 0x80090F9C.
 *
 * Left unsized deliberately: its index is the u8 D_8009B3DE, which nothing
 * at the call site bounds to the five defined entries. */
extern void (*D_80090F9C[])(void);

#endif
