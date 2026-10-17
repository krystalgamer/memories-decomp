#ifndef MEMORIES_DECOMP_DUEL_PROJECTION_AXES_H
#define MEMORIES_DECOMP_DUEL_PROJECTION_AXES_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Three-point coordinate spans; the other components come from the caller.
 * Negative projection flags cause one retry before unconditional submission. */
void Library_DrawGlobeXSpan(LINE_G3 *packet, GsOT *ot, SVECTOR *points,
                  PSXLONG *control, s32 origin, s32 span);
void Library_DrawGlobeZSpan(LINE_G3 *packet, GsOT *ot, SVECTOR *points,
                  PSXLONG *control, s32 origin, s32 span);

#endif
