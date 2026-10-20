#include "../../types.h"
/* SLES-03947 build of src/game/model_slot_control.c. The US source is
 * included unchanged.
 *
 * The symbols below sit at other addresses in the European executable and
 * their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US build reaches
 * the transfer flags through a second .data view named D_8009B0F4_abs; the
 * European build names the object only. */
#define D_80011574 D_800114A0
#define D_8009B030 D_8009BFB8
#define D_8009B038 D_8009BFC0
#define D_8009B040 D_8009BFC8
#define D_8009B048 D_8009BFD0
#define D_8009B050 D_8009BFD8
#define D_8009AFF4 D_8009BF7C
#define D_8009B0F4_abs D_8009B0F4
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
#include "../model_slot_control.c"
