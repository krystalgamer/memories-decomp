#include "../../types.h"

#define VERSION_JAPAN
/* Selects the Japanese text-bank layout and mask in text_constants.h. */
#define VERSION_JAPAN_TEXT_LOOKUP_STRING
#define TEXT_BOX_ENTRY_TYPE JapaneseDuelEffectEntry
#define TEXT_BOX_ADVANCE_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#include "../text_box_build_step.c"
