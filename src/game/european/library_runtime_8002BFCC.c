#include "../../types.h"

/* SLES-03947 build of src/game/library_runtime.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_FUNC_8002BFCC

/* The Library package's place on the European disc. */
#define LIBRARY_PACKAGE_FIRST_SECTOR 0x233A
#define LIBRARY_PACKAGE_SECTOR_COUNT 0x8F
#define LIBRARY_RESOURCE_FIELD_2C 0x280
#define LIBRARY_SELECTOR_SPRITE_Y 0xE8
#define LIBRARY_CARD_SELECTOR_BASE 0x2E0

/* Functions that take the splat name of their European address, as in
 * library_draw_card_grid's wrapper. */
#define Library_DrawCardGrid func_80029F18
#define func_8002BD0C func_8002BD4C

#include "../library_runtime.c"
