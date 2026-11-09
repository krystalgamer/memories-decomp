#include "../../types.h"
#include "../duel_effect_entry_occupancy.h"

/* SLPM-86398 build of src/game/text_commands.c. */
#define VERSION_JAPAN
#define VERSION_JAPAN_TEXT_LOOKUP_STRING
#define DUEL_EFFECT_STREAM_HIGH_MASK 0x801F0000
#define TEXT_STREAM_CURSOR_HIGH_MASK 0x801F0000
#define TEXT_BOX_GLYPH_CLUT_X 0x200
#define TEXT_BOX_GLYPH_CLUT_Y 0xFF
#define TEXT_BOX_ENTRY_TYPE JapaneseDuelEffectEntry
#define TEXT_BOX_ADVANCE_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DISPLAY_EFFECT_MENU_CONFIRM_MASK \
    (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
/* This build's entries at 0x800EB158-0x800ECD78 (#2602). */
#define DUEL_EFFECT_ENTRY_STORAGE_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_ENTRY_STORAGE_COUNT JAPANESE_DUEL_EFFECT_ENTRY_COUNT
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE JapaneseDuelEffectEntry
#include "../text_commands.c"
