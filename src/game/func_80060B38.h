#ifndef MEMORIES_DECOMP_FUNC_80060B38_H
#define MEMORIES_DECOMP_FUNC_80060B38_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws a deck box's six type-count digits and its seven card stat rows into
 * the ordering table. func_80061008 does not call this; it stores the address
 * in a display object's field_4C, so the object's own draw pass supplies both
 * arguments. That is why the declaration it replaced could say (void) without
 * the build noticing. */
void func_80060B38(u8 *obj, GsOT *ot);

#endif
