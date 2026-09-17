#ifndef MEMORIES_DECOMP_DUEL_UPDATE_DRAW_CARD_SLIDE_H
#define MEMORIES_DECOMP_DUEL_UPDATE_DRAW_CARD_SLIDE_H

#include "../types.h"
#include "display_object.h"

/* A display object's update callback that slides the object 0x140 pixels to
 * the left. On the first frame it fixes the target x at its current output x
 * minus 0x140, takes its step from the phase value it was given (0x400 /
 * field_60), and starts the phase at -0x400. Each frame then eases it through
 * Widget_SlideSine. When the phase reaches zero it snaps to the target and
 * clears its update hook and field_6C. duel_draw_resolution.c installs it by
 * address. */
void Duel_UpdateDrawCardSlide(DisplayObject *object);

#endif
