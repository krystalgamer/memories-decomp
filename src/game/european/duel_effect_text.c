#include "../../types.h"

/* SLES-03947 build of src/game/duel_effect_text.c, with the European
 * bodies. Values that differ in the European release; the US source names
 * each with an #ifndef default. */
#define VERSION_EUROPE
/* The one row step of the Japanese-shaped choice layout; 8 in the Japanese
 * build. */
#define DUEL_EFFECT_LAYOUT_ROW_STEP 10
#define WIDGET_PULSE_BASE 0x60
#define DIALOG_UPDATE_CHOICE_SKIP_FLAG 8
#include "../duel_effect_text.c"
