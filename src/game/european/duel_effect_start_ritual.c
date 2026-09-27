#include "../../types.h"

/* SLES-03947 build of src/game/duel_ritual_effect.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_DUEL_EFFECT_START_RITUAL
#define VERSION_EUROPE_DUEL_EFFECT_APPLY_RITUAL

#define DUEL_RITUAL_CARD_START_Y -42
#define DUEL_RITUAL_CARD_REST_Y 14
#define DUEL_RITUAL_SCREEN_HEIGHT 256

/* The US build reaches these objects through second .data views named
 * *_abs; this build names the objects only. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134
/* As in func_8001B170.c. */
#define D_8009B19C D_8009C17C

#include "../duel_ritual_effect.c"
