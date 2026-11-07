#ifndef GAME_SAVE_PAIR_WORKSPACE_H
#define GAME_SAVE_PAIR_WORKSPACE_H

#include "../types.h"

/* 0x801D1200. Five functions in four units take its address and nothing
 * else: Duel_InitScene (src/game/duel_scene.c, `pool =
 * gSaveData_aPairWorkspace;`, then `pool + 0x1000`), SaveData_UpdateLoadPair
 * (mem_card_dialog.c as the first argument to MemCardDialog_Request
 * and through SaveDataState casts, the second at
 * `gSaveData_aPairWorkspace + 0x1000`; that unit also names +0x1000 as
 * gSaveData_aSecondPlayerState),
 * SaveData_UpdateDuelLoad (mem_card_dialog.c, the
 * second `+ TWO_PLAYER_SAVE_SLOT_STRIDE`), MainMenu_RefreshTradeInventory
 * (MainMenu_RefreshTradeInventory in src/overlays/main_menu/trade_update.c, `+ slot * 0x1000`) and
 * MainMenu_UpdateTradeScreen (src/overlays/main_menu/trade_update.c, through
 * its 16-byte block view, cast at the use). The
 * resident listings form the address with lui/addiu (Duel_InitScene.s:226-227,
 * SaveData_UpdateDuelLoad.s:22-23, SaveData_UpdateLoadPair.s:115-116 and :136-137) and say
 * nothing about the object's width. The linker scripts name
 * gSaveData_aFirstPlayerNameSjis (+0x40C) and gSaveData_aTradeWriteStaging (+0x680)
 * inside it, and the Block16 indexes above reach +0x400, +0x680, +0x6D0,
 * +0xA80, +0x1000, +0x1680 and +0x16D0; the interior names keep their own
 * declarations. Of the five units that used to declare
 * it privately, four said `u8 []` and trade_update.c `Block16 []`. */
extern u8 gSaveData_aPairWorkspace[];
extern u8 gSaveData_aSecondPlayerState[];

#endif
