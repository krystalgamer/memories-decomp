#include "../../types.h"

/* SLES-03947 build of src/game/text_box_build_step.c, with its European bodies. */

#define VERSION_EUROPE
#define TEXT_BOX_BUILD_KEEP_FLAG 4
#define TEXT_BOX_BUILD_OWN_ENTRIES_FLAG 0x80
#define TEXT_BOX_ENTRY_TYPE EuropeanDuelEffectEntry
#define TEXT_BOX_GLYPH_COUNT(o) (*(u16 *)&(o)->field_60)
#define TEXT_BOX_GLYPH_LIMIT(o) (*(u16 *)&(o)->field_62)
#define TEXT_BOX_RANGE_START(o) ((o)->range_start_5C)

#include "../text_box_build_step.c"
