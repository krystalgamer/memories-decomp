#include "../../types.h"

/* SLES-03947 build of src/game/model_slot_load.c. The US source is included
 * unchanged.
 *
 * The US build reaches the transfer flags through a second .data view named
 * D_8009B0F4_abs; the European build names the object only. The two
 * animation-table builders sit at other addresses in the European
 * executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). */
#define D_8009B0F4_abs D_8009B0F4
#define Model_BuildSlotAnimationTables func_80051C6C
#define Model_LinkSlotAnimationRows func_80051E24

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define MODEL_SPECIAL_BATTLE_FILE_START_SECTOR 0x5D4
#define MODEL_AUX_FILE_START_SECTOR 0x2A8
#define MODEL_TEXTURE_STAGE3_RECT_X(index) 0x280
#define MODEL_TEXTURE_STAGE3_RECT_Y(index) (((index) << 4) + 0xD8)
#define MODEL_TEXTURE_PALETTE_RECT_X 0x280
#define MODEL_TEXTURE_TAIL_ROW_Y 0xE0

/* Selects the European store order in func_800577B0. */
#define VERSION_EUROPE

#include "../model_slot_load.c"
