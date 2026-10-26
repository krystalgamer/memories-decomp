#include "../../types.h"

/* SLES-03947 build of src/game/duel_effect_update_card_viewer_state.c, with its European body. */

#define VERSION_EUROPE
#define D_8009B248 gDuel_bEffectHandlerFlags
/* The US build reaches these objects through second .data views named
 * *_abs; this build names the objects only. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#define D_8009B3D4_IN_DATA
#include "../duel_effect_states.c"
