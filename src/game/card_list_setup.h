#ifndef MEMORIES_DECOMP_CARD_LIST_SETUP_H
#define MEMORIES_DECOMP_CARD_LIST_SETUP_H

#include "../types.h"
#include "card_constants.h"
#include "file_transfer.h"

/* 0x801D07BC, 0x56C bytes above gLibrary_abCardChest (0x801D0250). */
extern s16 gDuel_awRecentCardDrops[DUEL_RECENT_CARD_DROP_COUNT];
#define DUEL_RECENT_DROPS_CHEST_DISTANCE 0x56C

/* Shared card-list graphics transfer: four stages in NA/JP, five in Europe. */
void CardList_LoadSharedGraphicsStage(FileTransferDescriptor *descriptor, s32 mode);

/* Loads and waits for the graphics shared by Main_RunTrade and
 * BuildDeck_InitScreen. The request is NA 0x2189/0x4C sectors,
 * JP 0x2184/0x4C, or EU 0x2997/0x51. */
void CardList_LoadSharedGraphics(void);

/* Prunes gDuel_awRecentCardDrops in two passes. The first walks the table
 * backwards and clears any ID whose current chest quantity, read from the
 * region 0x56C below the table at ID - 1, is zero. The second moves surviving
 * entries to the front, zeroing each slot it moves out of, so the table ends
 * up densely packed with the order preserved.
 *
 * The ordinary C uses the split-address compiler profile for the initial
 * table address. Its reverse cursor is an integer address, so the final
 * decrement does not form a pointer before the array.
 *
 * BuildDeck_InitScreen and BuildDeck_UpdateExitConfirmation both call it.
 * This is chest quantity, not total ownership: a card with all copies in
 * the deck may lose its recent marker. Duplicates are not removed. */
void Library_PruneRecentCardDrops(void);

#endif
