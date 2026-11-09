#include "../../types.h"
#include "../duel_effect_entry_occupancy.h"

/* SLES-03947 build of src/game/text_commands.c, with the European bodies,
 * glyph-fade callbacks and 0x16-byte entries. Values that differ in the
 * European release; the US source names each with an #ifndef default. */
#define VERSION_EUROPE
#define DUEL_EFFECT_SELECTOR_FLAG 0x100
#define DUEL_EFFECT_CLEARED_CHANNEL_FLAG 0x10
#define TEXT_BOX_LAYOUT_QUAD_FLAG 0x40
#define TEXT_BOX_GLYPH_CLUT_Y 0xC0
#define TEXT_BOX_BUILD_KEEP_FLAG 4
#define TEXT_BOX_BUILD_OWN_ENTRIES_FLAG 0x80
#define TEXT_BOX_ENTRY_TYPE EuropeanDuelEffectEntry
#define TEXT_BOX_GLYPH_COUNT(o) (*(u16 *)&(o)->output_count)
#define TEXT_BOX_GLYPH_LIMIT(o) (*(u16 *)&(o)->field_62)
#define FUNC_80039794_CHOICE_FLAG 0x10
/* This build's entries at 0x800EBF70-0x800F0430 (#2602). */
#define DUEL_EFFECT_ENTRY_STORAGE_TYPE EuropeanDuelEffectEntry
#define DUEL_EFFECT_ENTRY_STORAGE_COUNT EUROPEAN_DUEL_EFFECT_ENTRY_COUNT
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE EuropeanDuelEffectEntry
#include "../text_commands.c"
