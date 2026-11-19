#ifndef GAME_SHARED_PRIMITIVE_RENDERERS_H
#define GAME_SHARED_PRIMITIVE_RENDERERS_H

#include "../types.h"
#include "../psyq/libhmd.h"
#include "model_polygon_packets.h"

/* The shared-vertex preparation block Model_PrepareSharedPrimitiveCaches
 * consumes: after its type word, a vertex range (count, index into vertop,
 * index into the vertop2 cache) and a normal range (count, index into
 * nortop, index into the nortop2 colour cache). */
typedef struct {
    u32 type;
    u32 vertex_count;
    u32 vertex_index;
    u32 vertex_cache_index;
    u32 normal_count;
    u32 normal_index;
    u32 colour_cache_index;
} ModelSharedCacheBlock;

/* A vertop2 cache entry as the two words the preparation writes: SXY2,
 * then the depth word (otz = SZ3 in the low half, p = IR0 in the high
 * half), all ones for a vertex whose projection overflowed. */
typedef union {
    GsWORKUNIT unit;
    u32 words[2];
} ModelSharedCacheEntry;

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
