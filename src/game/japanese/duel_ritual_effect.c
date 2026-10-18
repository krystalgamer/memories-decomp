#include "../../types.h"

#define VERSION_JAPAN
#define gDuel_aCardRecords gJapanese_DuelCardRecords
#define D_8009B360 gJapanese_DuelPlayerState
#define D_800EB0F8 gJapanese_D_800EB0F8
#define DUEL_EFFECT_RITUAL_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#include "../duel_ritual_effect.c"
