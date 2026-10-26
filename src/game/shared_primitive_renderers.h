#ifndef GAME_SHARED_PRIMITIVE_RENDERERS_H
#define GAME_SHARED_PRIMITIVE_RENDERERS_H

#include "../types.h"
#include "../psyq/libhmd.h"

/*
 * HMD shared-vertex drivers. Model_PrepareSharedPrimitiveCaches projects and
 * lights a block's shared vertices and normals into the vertop2/nortop2
 * caches; the Gt3/Gt4 drawers then build packets from those caches. Each
 * consumes one primitive block and returns the next.
 */
u32 *Model_PrepareSharedPrimitiveCaches(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedGt3Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedGt4Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedGt3Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedGt4Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedSemiGt3Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedSemiGt4Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedSemiGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedSemiGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedSemiGt3Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedSemiGt4Primitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedSemiGt3WindowRecordPrimitives(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedTwoSidedSemiGt4WindowRecordPrimitives(GsARGUNIT_SHARED *arg);

#endif
