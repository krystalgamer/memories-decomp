#ifndef GAME_MEM_CARD_DIRECTORY_PATTERNS_H
#define GAME_MEM_CARD_DIRECTORY_PATTERNS_H

#include "../types.h"

/* Both are separate four-byte "*" objects. Declared unsized so the -G8 save
 * machine materialises their addresses with %hi/%lo, as retail does. */
extern u8 gMemCardDialog_szAllFilesPattern[];
extern u8 gMemCard_szAllFilesPattern[];

#endif
