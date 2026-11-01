#include "../../types.h"

#define VERSION_JAPAN
#define DUEL_EFFECT_CHOICE_CONFIRM_MASK \
    (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define gFile_wTransferStepFlags_abs gFile_wTransferStepFlags
#include "../duel_effect_state_callbacks.c"
