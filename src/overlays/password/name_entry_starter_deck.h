#ifndef MEMORIES_DECOMP_NAME_ENTRY_STARTER_DECK_H
#define MEMORIES_DECOMP_NAME_ENTRY_STARTER_DECK_H

#include "../../types.h"
#include "../../game/card_constants.h"

#define NAME_ENTRY_STARTER_DECK_POOL_COUNT 7
#define NAME_ENTRY_STARTER_DECK_POOL_PADDING_SIZE 18
#define NAME_ENTRY_STARTER_DECK_POOL_POINTER_COUNT \
    (NAME_ENTRY_STARTER_DECK_POOL_COUNT + 1)

typedef struct {
    u16 draw_count;
    u16 weights[CARD_COUNT];
    u8 padding[NAME_ENTRY_STARTER_DECK_POOL_PADDING_SIZE];
} NameEntryStarterDeckPool;

typedef char NameEntryStarterDeckPool_size_must_be_0x5B8[
    sizeof(NameEntryStarterDeckPool) == 0x5B8 ? 1 : -1
];

extern NameEntryStarterDeckPool
    gNameEntry_aStarterDeckPools[NAME_ENTRY_STARTER_DECK_POOL_COUNT];
extern NameEntryStarterDeckPool
    *gNameEntry_apStarterDeckPools[NAME_ENTRY_STARTER_DECK_POOL_POINTER_COUNT];

/* NameEntry_BuildStarterDeck keeps a u16 cursor after selecting a typed pool:
 * advancing from draw_count to weights as `entry++` is required for the
 * original GCC allocation and instruction count. The cast is an exact-code
 * view of this checked layout, not a second record definition. */

#endif
