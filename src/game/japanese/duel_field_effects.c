#include "../../types.h"

/* SLPM-86398 build of src/game/duel_field_effects.c: the symbols below sit at
 * other addresses in the Japanese executable and their US names are taken
 * there, so they are aliased (config/slpm_86398/symbols.txt has the
 * addresses). */
#define VERSION_JAPAN
#define D_8009AF40 gJapanese_D_8009AF40
#define D_8009B360 gJapanese_DuelPlayerState
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define DUEL_EFFECT_RITUAL_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#include "../duel_field_effects.c"
