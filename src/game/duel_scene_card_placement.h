#ifndef MEMORIES_DECOMP_DUEL_SCENE_CARD_PLACEMENT_H
#define MEMORIES_DECOMP_DUEL_SCENE_CARD_PLACEMENT_H

#include "../types.h"
#include "display_object.h"

/* The presentation's card/result halfword. Card use also defines and writes
 * gDuel_wPlacementCardId as u16; placement tests its signed value and uses bit 15 to
 * distinguish fusion from equipment. The low twelve bits are the card id. */
extern u16 gDuel_wPlacementCardId;
extern s16 gDuel_wEquipCardId;

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
