#include "../../types.h"

/* SLES-03947 to SLES-03951 build of src/game/library.c. */
#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define LIBRARY_GRID_TEXTURE_WORD 0xE702B0
#define LIBRARY_GRID_SCREEN_HEIGHT 0x100
#define LIBRARY_GRID_SKIP_CY_STORE
#define LIBRARY_CARD_TEXT_BOX_Y 0xD5
#define LIBRARY_VIEWPORT_FOLLOW_BOTTOM 0xC0
#define LIBRARY_CARD_VIEW_X 0x98
#define LIBRARY_CARD_VIEW_BOX_WIDTH 0xA0
#define LIBRARY_CARD_VIEW_Y 6
#define LIBRARY_LOADING_ICON_Y 0xDD
#define LIBRARY_CARD_VIEW_BOX_HEIGHT 0xD0
#define LIBRARY_CARD_ZOOM_Y 0
#define LIBRARY_CARD_ZOOM_MIN_SCALE 0xD7
/* The Library package's place on the European disc. */
#define LIBRARY_PACKAGE_FIRST_SECTOR 0x233A
#define LIBRARY_PACKAGE_SECTOR_COUNT 0x8F
#define LIBRARY_RESOURCE_CLUT_X 0x280
#define LIBRARY_SELECTOR_SPRITE_Y 0xE8
#define LIBRARY_CARD_SELECTOR_BASE 0x2E0
#include "../library.c"
