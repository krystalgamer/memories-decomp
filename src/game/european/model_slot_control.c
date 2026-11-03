#include "../../types.h"
/* SLES-03947 build of src/game/model_slot_control.c. The US source is
 * included unchanged. */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define MODEL_SPECIAL_BATTLE_FILE_START_SECTOR 0x5D4
#define MODEL_AUX_FILE_START_SECTOR 0x2A8
#define MODEL_TEXTURE_STAGE3_RECT_X(index) 0x280
#define MODEL_TEXTURE_STAGE3_RECT_Y(index) (((index) << 4) + 0xD8)
#define MODEL_TEXTURE_PALETTE_RECT_X 0x280
#define MODEL_TEXTURE_TAIL_ROW_Y 0xE0
/* Selects the European store order in Model_LoadSpecialBattlePackageStage. */
#define VERSION_EUROPE
#include "../model_slot_control.c"
