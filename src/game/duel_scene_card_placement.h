#ifndef MEMORIES_DECOMP_DUEL_SCENE_CARD_PLACEMENT_H
#define MEMORIES_DECOMP_DUEL_SCENE_CARD_PLACEMENT_H

#include "../types.h"
#include "display_object.h"

/* The presentation's card/result halfword. Card use also defines and writes
 * D_8009B150 as u16; placement tests its signed value and uses bit 15 to
 * distinguish fusion from equipment. The low twelve bits are the card id. */
extern u16 D_8009B150;
/* Shared work-slot selector. Exodia uses its unsigned view; placement takes
 * the measured signed byte explicitly at its bound and array-index reads. */
extern u8 D_8009B1B9;
extern s16 D_8009B206;
extern s8 D_8009B218;

/* A signed read of the same two bytes, not another allocation. The aggregate
 * read preserves the result-publication dependency in the combination path. */
typedef struct {
    s16 value;
} DuelPlacementCardIdCell;

typedef char DuelPlacementCardIdCell_size_must_be_2[
    sizeof(DuelPlacementCardIdCell) == 2 ? 1 : -1
];

void DuelScene_UpdateCardPlacement(void);
void func_80019BD0(DisplayObject *object);

#endif
