#include "../../types.h"
/* SLES-03947..51 build of src/game/model.c. VERSION_EUROPE selects the
 * European scene-controller and slot-control paths. */
#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define MODEL_INTRO_PACKAGE_START_SECTOR 0x6E7
#define MODEL_TEXTURE_RESET_ROW_X 0x280

#define BACKGROUND_RENDER_CLIP_LIMIT 257
#define MODEL_RENDER_BLEND_MODE_5_WORD 0x3C280000
#define MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT 1
#define MODEL_SPECIAL_BATTLE_FILE_START_SECTOR 0x5D4
#define MODEL_AUX_FILE_START_SECTOR 0x2A8
#define MODEL_TEXTURE_STAGE3_RECT_X(index) 0x280
#define MODEL_TEXTURE_STAGE3_RECT_Y(index) (((index) << 4) + 0xD8)
#define MODEL_TEXTURE_PALETTE_RECT_X 0x280
#define MODEL_TEXTURE_TAIL_ROW_Y 0xE0

/* The European executables keep a different object (libpress data) at the US
 * debug-state word's address, so each region's symbols.txt binds D_8009B004 at
 * its own address. */
#include "../model.c"
