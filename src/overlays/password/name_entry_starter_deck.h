#ifndef MEMORIES_DECOMP_NAME_ENTRY_STARTER_DECK_H
#define MEMORIES_DECOMP_NAME_ENTRY_STARTER_DECK_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"

#define NAME_ENTRY_STARTER_DECK_POOL_COUNT 7
#define NAME_ENTRY_STARTER_DECK_POOL_POINTER_COUNT \
    (NAME_ENTRY_STARTER_DECK_POOL_COUNT + 1)

extern NameEntryStarterDeckPool
    gNameEntry_aStarterDeckPools[NAME_ENTRY_STARTER_DECK_POOL_COUNT];
extern NameEntryStarterDeckPool
    *gNameEntry_apStarterDeckPools[NAME_ENTRY_STARTER_DECK_POOL_POINTER_COUNT];

/* NameEntry_BuildStarterDeck keeps a u16 cursor after selecting a typed pool:
 * it starts at draw_count and advances to weights with `entry++`, which the
 * original GCC allocation and instruction count require. */

#endif
