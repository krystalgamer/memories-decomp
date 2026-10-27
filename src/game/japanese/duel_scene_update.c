#include "../../types.h"

#define DUEL_QUIT_BOX_X 0x70
#define DUEL_QUIT_BOX_WIDTH 0x60
#define DUEL_QUIT_BOX_HEIGHT 0x20
#define TEXT_BOX_JAPANESE_QUIT_CHANNEL_VIEW
#include "../text_box_lifecycle.h"

#define gText_Channel3 gJapanese_DuelQuitChannel
#include "../duel_scene_update.c"
