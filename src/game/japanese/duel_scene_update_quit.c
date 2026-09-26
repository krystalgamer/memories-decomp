#include "../../types.h"

#define DUEL_QUIT_BOX_X 0x70
#define DUEL_QUIT_BOX_WIDTH 0x60
#define DUEL_QUIT_BOX_HEIGHT 0x20
#define TEXT_BOX_JAPANESE_QUIT_CHANNEL_VIEW
#include "../text_box_lifecycle.h"

#define D_800EB224 gJapanese_DuelQuitChannel
#define func_800235C0 func_80023408
#define gDuel_apfnSceneStateHandler D_80090848
#include "../func_80024200.c"
