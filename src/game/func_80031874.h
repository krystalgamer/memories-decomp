#ifndef MEMORIES_DECOMP_FUNC_80031874_H
#define MEMORIES_DECOMP_FUNC_80031874_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

#ifdef VERSION_EUROPE
#include "card_list_text_boxes.h"

typedef struct {
    u32 pad_00;
    s16 id;
    s16 attack;
    s16 defense;
    u8 type;
    u8 pad_0B[2];
    u8 flags;
    u8 pad_0E[2];
} CardListRenderEntry;

typedef char CardListRenderEntrySize[
    sizeof(CardListRenderEntry) == sizeof(CardEntry) ? 1 : -1
];
#endif

/* Draws one page of a Build Deck card list: the sort-menu icons above it and
 * eight rows (nine in Europe) of card id and ATK/DEF. The object's field_67
 * picks the list; list 0 also marks ranked cards and shows chest and deck counts,
 * the other list numbers its rows instead. No C code calls it:
 * func_800323F8 stores its address in a display object's field_4C,
 * the (DisplayObject *, GsOT *) draw callback slot. */
void func_80031874(DisplayObject *obj, GsOT *ot);

#endif
