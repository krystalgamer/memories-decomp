#ifndef MEMORIES_DECOMP_FUNC_8003AC48_H
#define MEMORIES_DECOMP_FUNC_8003AC48_H

#include "../types.h"

/* D_80090F68 display effect step: draws one record's glyph run. It sets bit 0x10
 * of the record's +0x32, reads the position out of the object the record points
 * at, and submits the run through func_8003A440 with GsALON | GsAONE. It does
 * nothing while func_80039F1C reports the record is not ready. */
void func_8003AC48(u8 *record);

#endif
