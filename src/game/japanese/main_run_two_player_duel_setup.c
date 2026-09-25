#include "../../types.h"
#include "duel_effect_channel.h"

#define TWO_PLAYER_SETUP_CHANNEL_TYPE JapaneseDuelEffectChannel

#define TWO_PLAYER_SETUP_BOX_X 0x20
#define TWO_PLAYER_SETUP_BOX_WIDTH 0x100
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B234 gJapanese_D_8009B234
#define D_8009B368 gJapanese_D_8009B368
#define D_800EB0F8 gJapanese_D_800EB0F8
#define MainMenu_StartValueSetup func_80180FD8
#define MainMenu_UpdateValueSetup func_801812B4
#define MainMenu_FinishValueSetup func_80181E30
#define Main_RunTwoPlayerDuelSetup func_8002DA18
#include "../main_run_two_player_duel_setup.c"
