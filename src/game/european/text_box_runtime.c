#include "../../types.h"
/* This build's zero-initialized storage at 0x800F0430-0x800F05D0 (#2602)
 * also holds the battle models, which the North American build keeps in
 * duel_effect_process_entries.c. */
#define TEXT_BOX_RUNTIME_OWNS_BATTLE_MODELS

/* SLES-03947 build of src/game/text_box_runtime.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define FUNC_80039794_CHOICE_FLAG 0x10

#include "../text_box_runtime.c"
