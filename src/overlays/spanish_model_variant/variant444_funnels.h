#ifndef SPANISH_MODEL_VARIANT444_FUNNELS_H
#define SPANISH_MODEL_VARIANT444_FUNNELS_H

#include "variant411_funnels.h"

/* One command record of the French header-444 entry's table. The funnels read
   the first record's fade start (old blob + 0x114) whatever the command. */
typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 sheet_start, tube_start, tube_end, fade_start, fade_end, end;
} Variant444EntryConfig;

extern GsIMAGE gFrenchModelVariant444_aImages[9];
extern Variant444EntryConfig gFrenchModelVariant444_aConfigs[];

#endif
