#include "../../types.h"

/* SLES-03947 build of src/game/duel_ritual_effect.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_DUEL_EFFECT_START_RITUAL

/* The US build reaches these objects through second .data views named
 * *_abs; this build names the objects only. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#include "../duel_ritual_effect.c"
