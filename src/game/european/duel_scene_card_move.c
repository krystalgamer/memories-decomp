#include "../../types.h"

/* SLES-03947 build of src/game/duel_scene_card_move.c, with its European bodies. */

#define VERSION_EUROPE
#define DUEL_CARD_OFFSCREEN_Y -0x100

/* As in duel_scene_field_actions' wrapper: the splat name of the European
 * address. */

#define DUEL_HAND_STACK_INNER_Y 210
#define DUEL_HAND_STACK_COUNT_FIELD_5D 0xB0
#define DUEL_HAND_STACK_COUNT_CX 0x2D0
#define DUEL_SELECTION_CURSOR_FIELD_0C 0x84
#include "../duel_scene_card_move.c"
