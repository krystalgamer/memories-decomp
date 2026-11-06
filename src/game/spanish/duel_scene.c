#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/duel_scene.c: the European
 * values and storage, plus the localized duel-result sprite tables, the outro
 * sprite count and these builds' card-viewer addresses. */
#define DUEL_RESULT_LANGUAGE_SPRITE_TABLES
#define DUEL_RESULT_LOCALIZED_SPRITES
#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009C5D8
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009C5DD

#include "../european/duel_scene.c"
