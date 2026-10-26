#include "../../types.h"
#include "duel_effect_channel.h"

#define VERSION_JAPAN
/* Selects the Japanese text-bank layout and mask in text_constants.h. */
#define VERSION_JAPAN_TEXT_LOOKUP_STRING
#define TEXT_BOX_GLYPH_COUNT(o) (((JapaneseDuelEffectChannel *)(o))->output_count)
#define TEXT_BOX_GLYPH_LIMIT(o) (((JapaneseDuelEffectChannel *)(o))->output_limit)
#define TEXT_BOX_RANGE_START(o) (((JapaneseDuelEffectChannel *)(o))->entry_range_start)
#define TEXT_BOX_ENTRY_TYPE JapaneseDuelEffectEntry
#define TEXT_BOX_ADVANCE_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define D_80090E64 D_80090D14
#include "../text_box_build_step.c"
