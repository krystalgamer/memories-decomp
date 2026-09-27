#include "../../types.h"

/* SLES-03947 build of src/game/library_runtime.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_FUNC_8002ACA4
#define VERSION_EUROPE_FUNC_8002BAA0

#define LIBRARY_CARD_VIEW_X 0x98
#define LIBRARY_CARD_VIEW_BOX_WIDTH 0xA0
#define LIBRARY_CARD_VIEW_Y 6
#define LIBRARY_LOADING_ICON_Y 0xDD
#define LIBRARY_CARD_VIEW_BOX_HEIGHT 0xD0
#define LIBRARY_CARD_ZOOM_Y 0
#define LIBRARY_CARD_ZOOM_MIN_SCALE 0xD7
#define LIBRARY_EFFECT_CHANNEL_STRIDE 100
/* This build names the loader words directly. */
#define Library_DrawCardGrid func_80029F18
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#include "../library_runtime.c"
