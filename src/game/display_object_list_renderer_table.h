#ifndef YUGIOH_GAME_DISPLAY_OBJECT_LIST_RENDERER_TABLE_H
#define YUGIOH_GAME_DISPLAY_OBJECT_LIST_RENDERER_TABLE_H

#include "../types.h"
#include "display_object_layout.h"

/* The per-list display object renderers at 0x80090FB0.
 *
 * Sized, unlike the other tables carved alongside it. DISPLAY_OBJECT_LIST_COUNT
 * is seven, equals the entry count, and is the same constant that bounds the
 * list heads in display_object.h and DisplayObject_ResetPool's loop. */
extern void (*D_80090FB0[DISPLAY_OBJECT_LIST_COUNT])(void);

#endif
