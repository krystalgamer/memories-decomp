#ifndef MEMORIES_DECOMP_DUEL_CARD_STAT_DISPLAY_H
#define MEMORIES_DECOMP_DUEL_CARD_STAT_DISPLAY_H

#include "../types.h"
#include "display_object.h"

/* Spawns the little card-type icon for card `index` at (x, y): a 0x10 by 0x10
 * display object whose texture page is stepped by the card's type -- +0x10 for
 * magic and equip, +0x20 for trap, +0x30 for ritual, nothing for a monster --
 * read out of gDuel_adwCardStats[index - 1].
 *
 * `arg3` reaches the object's field_18 and `arg4` its +0x1A, both narrowed
 * by halfword stores. The object is returned in v0, although both current
 * callers discard it; the old void declaration hid that return value. */
DisplayObject *func_80031574(s32 index, s32 x, s32 y, s32 arg3, s32 arg4);

void func_800313E8(DisplayObject *object);

#endif
