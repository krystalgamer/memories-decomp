#include "../../types.h"

/* SLES-03947 to SLES-03951 build of src/game/duel_field_effects.c. */
#define VERSION_EUROPE
#define DUEL_RITUAL_CARD_START_Y -42
#define DUEL_RITUAL_CARD_REST_Y 14
#define DUEL_RITUAL_SCREEN_HEIGHT 256
/* The US build reaches these objects through second .data views named
 * *_abs; this build names the objects only. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134
/* As in duel_scene_card_move.c. */
#include "../duel_field_effects.c"
