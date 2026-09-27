#include "../../types.h"

/* SLES-03947 build of src/game/duel_phase_entry.c. The US build reaches these objects
 * through second .data views named *_abs; the European build names the
 * objects only. The US source is included as is. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_PHASE_CURSOR_FIELD_0C 0xBE
#define DUEL_PHASE_FIELD_40_BASE 0x360
#define DUEL_PHASE_HAND_CARD_Y 0x2A2

#include "../duel_phase_entry.c"
