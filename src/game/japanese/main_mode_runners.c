#include "../../types.h"

#define VERSION_JAPAN
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B268 gJapanese_bMenuRequest
#define D_8009B26D gJapanese_bRequestedMenu
#include "duel_effect_channel.h"
#define TWO_PLAYER_SETUP_CHANNEL_TYPE JapaneseDuelEffectChannel
#define TWO_PLAYER_SETUP_BOX_X 0x20
#define TWO_PLAYER_SETUP_BOX_WIDTH 0x100
#define D_8009B368 gJapanese_D_8009B368
#include "../main_mode_runners.c"
