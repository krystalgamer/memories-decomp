#include "../../types.h"

/* SLPM-86398 build of src/game/duel_scene_field_actions.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define gDuel_abFieldGridCardRecordIndices gJapanese_DuelFieldGridCardRecordIndices
#define gDuel_awSideViewHeadings gJapanese_DuelSideViewHeadings
#define D_8009B160 gJapanese_D_8009B160
#define D_8009B170 gJapanese_D_8009B170
#define gDuel_pActiveSelectionRecord gJapanese_DuelActiveSelectionRecord
#define D_8009B1BC gJapanese_D_8009B1BC
#define D_8009B1F8 gJapanese_D_8009B1F8
#define D_8009B300 gJapanese_D_8009B300
#define D_8009B360 gJapanese_DuelPlayerState
#define gDuel_aCardRecords gJapanese_DuelCardRecords
#define func_80028220 func_80028034

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_ACTIONS_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DUEL_FIELD_ACTIONS_CANCEL_BUTTON PAD_BUTTON_CROSS

#include "../duel_scene_field_actions.c"
