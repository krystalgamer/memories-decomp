#include "../../types.h"
#define TEXT_BOX_RUNTIME_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x800EAFD8-0x800EB158 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 gJapanese_D_800EB0F8[0x180];

#define DISPLAY_EFFECT_MENU_CONFIRM_MASK \
    (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DISPLAY_EFFECT_MENU_CHANNEL_STRIDE 0x60
#include "../text_box_runtime.c"
