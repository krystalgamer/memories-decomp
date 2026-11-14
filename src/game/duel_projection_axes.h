#ifndef MEMORIES_DECOMP_DUEL_PROJECTION_AXES_H
#define MEMORIES_DECOMP_DUEL_PROJECTION_AXES_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* One vertex of the globe's line packet. The GTE stores its screen position
 * as a single packed SXY word (RotAverage3's sxy outputs, gte_stsxy); the
 * GPU and Library_DrawWireframeGlobe read the halves as x and y. */
typedef union {
    long sxy;
    struct {
        s16 x;
        s16 y;
    } h;
} GlobeLineVertex;

/* The LINE_G3 at scratchpad 0x1F800000 that Library_DrawWireframeGlobe
 * builds and the span helpers project into: LINE_G3's layout with each
 * vertex position typed as GlobeLineVertex. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    GlobeLineVertex xy0;
    u8 r1, g1, b1, p1;
    GlobeLineVertex xy1;
    u8 r2, g2, b2, p2;
    GlobeLineVertex xy2;
    u32 pad;
} GlobeLinePacket;

/* Three-point coordinate spans; the other components come from the caller.
 * Negative projection flags cause one retry before unconditional submission. */
void Library_DrawGlobeXSpan(GlobeLinePacket *packet, GsOT *ot, SVECTOR *points,
                  long *control, s32 origin, s32 span);
void Library_DrawGlobeZSpan(GlobeLinePacket *packet, GsOT *ot, SVECTOR *points,
                  long *control, s32 origin, s32 span);

#endif
