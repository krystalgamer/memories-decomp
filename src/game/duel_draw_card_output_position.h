#ifndef MEMORIES_DECOMP_DUEL_DRAW_CARD_OUTPUT_POSITION_H
#define MEMORIES_DECOMP_DUEL_DRAW_CARD_OUTPUT_POSITION_H

#include "display_object_interpolation.h"

/* Draws one duel card using the output coordinates stored on its object. */
void Duel_DrawCardAtOutputPosition(
    DisplayObjectPosition *object, s32 value
);

#endif
