#ifndef MEMORIES_DECOMP_CARD_PRESENTATION_H
#define MEMORIES_DECOMP_CARD_PRESENTATION_H

#include "duel_card.h"
#include "duel_card_turn_animations.h"

extern DuelCardRecord gDuel_aFieldCardRecords[];

void Duel_UpdateCardToSelectionView(DuelCardTurnObject *object);
void Duel_UpdateCardToFieldView(DuelCardTurnObject *object);

#endif
