#ifndef GAME_CARD_LIST_ROW_TEXT_H
#define GAME_CARD_LIST_ROW_TEXT_H

#include "card_list_rows.h"

extern CardListRowSet gCardList_aRowSets[];
void CardList_BuildRowSetTextBoxes(u16 *entries, s32 side, s32 flag, s32 ignored);

#endif
