#ifndef GAME_SAVE_PAIR_WORKSPACE_H
#define GAME_SAVE_PAIR_WORKSPACE_H

#include "../types.h"
#include "mem_card.h"

/* 0x801D1200: two TWO_PLAYER_SAVE_SLOT_STRIDE-byte slots, one per player,
 * used by the two-player duel and the trade screen.
 *
 * SaveData_UpdateLoadPair (mem_card.c) reads each memory card's state block
 * (SAVE_DATA_STATE_SIZE bytes) into `loaded` of slots 0 and 1 and compares
 * their duelist codes; SaveData_UpdateDuelLoad checks both decks and converts
 * both names (player_name_sjis); Duel_InitScene (duel_scene.c) uses the two
 * loaded states as the sides' save data; MainMenu_RefreshTradeInventory (main_menu/trade_screen.c)
 * lists a slot's card_quantities.
 *
 * MainMenu_UpdateTradeScreen copies the first TWO_PLAYER_SAVE_TRANSFER_SIZE
 * bytes of each `loaded` block to `trade_write` at +0x680, moves card counts
 * between the two trade_write card_quantities, and SaveData_RequestTradeWrite seals both
 * trade_write blocks before writing them; after the write the screen copies
 * trade_write back over `loaded`. That unit still walks the slots through
 * its 16-byte block view. Nothing is known to use the slot past +0xD00. */
typedef struct {
    SaveDataStateBlock loaded;
    SaveDataStateBlock trade_write;
    u8 unused_D00[TWO_PLAYER_SAVE_SLOT_STRIDE - 2 * SAVE_DATA_STATE_SIZE];
} SaveDataPairSlot;

typedef char SaveDataPairSlot_size_must_be_the_slot_stride[
    sizeof(SaveDataPairSlot) == TWO_PLAYER_SAVE_SLOT_STRIDE ? 1 : -1
];
typedef char SaveDataPairSlot_trade_write_offset_must_be_0x680[
    OFFSETOF(SaveDataPairSlot, trade_write) == SAVE_DATA_STATE_SIZE ? 1 : -1
];

extern SaveDataPairSlot gSaveData_aPairWorkspace[2];

#endif
