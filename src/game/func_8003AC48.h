#ifndef MEMORIES_DECOMP_FUNC_8003AC48_H
#define MEMORIES_DECOMP_FUNC_8003AC48_H

#include "../types.h"
#include "menu_record.h"

/* D_80090F68 display effect step: draws one record's glyph run. It sets bit 0x10
 * of the record's +0x32, reads the position out of the object the record points
 * at, and submits the run through func_8003A440 with GsALON | GsAONE. It does
 * nothing while func_80039F1C reports the record is not ready.
 *
 * The record is a MenuRecord: the function reached it through thirteen raw
 * byte offsets while already casting the same pointer to DisplayEffectState,
 * a narrower view of that record, on its own first line. Its two object rows
 * are grid[0] and grid[1] -- the +0 and +0xC func_8003A1EC fills. */
void func_8003AC48(MenuRecord *record);

#endif
