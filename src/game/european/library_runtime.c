#include "../../types.h"

/* SLES-03947 build of src/game/library_runtime.c. */
#define VERSION_EUROPE
#define LIBRARY_CARD_VIEW_X 0x98
#define LIBRARY_CARD_VIEW_BOX_WIDTH 0xA0
#define LIBRARY_CARD_VIEW_Y 6
#define LIBRARY_LOADING_ICON_Y 0xDD
#define LIBRARY_CARD_VIEW_BOX_HEIGHT 0xD0
#define LIBRARY_CARD_ZOOM_Y 0
#define LIBRARY_CARD_ZOOM_MIN_SCALE 0xD7
#define LIBRARY_EFFECT_CHANNEL_STRIDE 100
/* The splat name of its European address. */
/* The language index is read through %hi/%lo, as in
 * duel_effect_resource_setup's wrapper. */
#define D_8009C02B_IN_DATA
/* The Library package's place on the European disc. */
#define LIBRARY_PACKAGE_FIRST_SECTOR 0x233A
#define LIBRARY_PACKAGE_SECTOR_COUNT 0x8F
#define LIBRARY_RESOURCE_FIELD_2C 0x280
#define LIBRARY_SELECTOR_SPRITE_Y 0xE8
#define LIBRARY_CARD_SELECTOR_BASE 0x2E0
#include "../library_runtime.c"
