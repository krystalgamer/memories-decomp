#include "../../types.h"
#include "duel_effect_channel.h"

#define VERSION_JAPAN
/* Selects the Japanese text-bank layout and mask in text_constants.h. */
#define VERSION_JAPAN_TEXT_LOOKUP_STRING
#define TEXT_BOX_GLYPH_COUNT(o) (((JapaneseDuelEffectChannel *)(o))->pad_5E)
#define TEXT_BOX_GLYPH_LIMIT(o) (((JapaneseDuelEffectChannel *)(o))->field_5F)
#define TEXT_BOX_RANGE_START(o) (((JapaneseDuelEffectChannel *)(o))->range_start_5C)
#define TEXT_BOX_ENTRY_TYPE JapaneseDuelEffectEntry
#define TEXT_BOX_ADVANCE_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define D_8009B357 D_8009B247
#define D_8009B350 gJapanese_D_8009B350
#define D_8009B35A gJapanese_D_8009B35A
#define D_80090F18 D_80090DC8
#define D_80090E64 D_80090D14
#define func_80035CA8 DuelEffect_ClearOccupancyValue
#include "../text_box_build_step.c"
