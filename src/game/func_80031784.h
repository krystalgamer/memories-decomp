#ifndef MEMORIES_DECOMP_FUNC_80031784_H
#define MEMORIES_DECOMP_FUNC_80031784_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws the seven sort-menu icons of a Build Deck card list through one
 * scratchpad GsSPRITE, 18 pixels apart. `data` is the list kind's row of
 * D_80090DD8 byte pairs; the icon whose low nibble equals `selected` is
 * drawn at full grey, the others dimmed. */
void func_80031784(GsSPRITE *record, s32 arg1, u8 *data, s32 selected);

#endif
