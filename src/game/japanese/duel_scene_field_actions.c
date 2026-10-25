#include "../../types.h"
#define VERSION_JAPAN

/* SLPM-86398 build of src/game/duel_scene_field_actions.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009B360 gJapanese_DuelPlayerState

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_ACTIONS_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DUEL_FIELD_ACTIONS_CANCEL_BUTTON PAD_BUTTON_CROSS

#include "../duel_scene_field_actions.c"
