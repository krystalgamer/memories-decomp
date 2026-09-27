#include "../../types.h"

/* SLES-03947 build of src/game/func_80028B08.c, with its European bodies. */

#define VERSION_EUROPE

#define FUNC_80028B08_CLUT_Y 0xE8
#define FUNC_80028B08_CLUT_X 0x340
#define FUNC_80028B08_FRAME_ATTRIBUTE 0x50000000
#define FUNC_80028B08_ATK_X 0x30
#define FUNC_80028B08_ATK_Y 0xBD
#define FUNC_80028B08_DEF_X 0x63
#define FUNC_80028B08_DEF_Y 0xBD

/* As in duel_effect_resource_setup.c. */
#define func_80028B08 func_80028AC8

#include "../func_80028B08.c"
