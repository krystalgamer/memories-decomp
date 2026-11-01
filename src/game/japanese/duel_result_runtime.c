#include "../../types.h"

#define VERSION_JAPAN
#define DUEL_RESULT_OUTRO_START_SECTOR 0x1DB6
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x1D3E
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_8009B360 gJapanese_DuelPlayerState

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_REWARDS_CONFIRM_BUTTON PAD_BUTTON_CIRCLE

#include "../duel_result_runtime.c"
