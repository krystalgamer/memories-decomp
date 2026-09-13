#ifndef MEMORIES_DECOMP_FUNC_80031874_H
#define MEMORIES_DECOMP_FUNC_80031874_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws one page of a Build Deck card list: the sort-menu icons above it and
 * eight rows of card id and ATK/DEF. The object's field_67 picks the list;
 * list 0 also marks ranked cards and shows each card's chest and deck counts,
 * the other list numbers its rows instead. No C code calls it: the still
 * unmatched func_800323F8 stores its address in a display object's field_4C,
 * the (DisplayObject *, GsOT *) draw callback slot. */
void func_80031874(DisplayObject *obj, GsOT *ot);

#endif
