#include "../../types.h"

/* SLES-03947 build of src/game/func_80024E58.c. The European WA.MRG gives
 * each terrain package 0xF0 sectors (see duel_load_terrain_package.c), and
 * a terrain's effect data starts at 0x1C58 + 0xF0 * terrain. */

#define VERSION_EUROPE
#define DUEL_TERRAIN_EFFECT_SECTOR(terrain) \
    ((((terrain) * 15) * 16) + 0x1C58)

/* This build names the loader words only; the US build reaches them through
 * second .data views named *_abs. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#include "../func_80024E58.c"
