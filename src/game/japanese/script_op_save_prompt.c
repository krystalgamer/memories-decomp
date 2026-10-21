#include "../../types.h"
#include "duel_effect_channel.h"

#define VERSION_JAPAN
#define SAVE_PROMPT_CHANNEL_TYPE JapaneseDuelEffectChannel
#define D_8009B268 gJapanese_bMenuRequest
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B26D gJapanese_bRequestedMenu
#include "../script_op_save_prompt.c"
