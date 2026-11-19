#ifndef GAME_SHARED_PRIMITIVE_RENDERERS_H
#define GAME_SHARED_PRIMITIVE_RENDERERS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* POLY_GT3 as GPU command words (colour+code, XY, UV+CLUT, colour, XY,
 * UV+TPAGE, ...). */
typedef struct {
    u32 tag;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
} SharedGt3Packet;

/* POLY_GT4 as GPU command words (colour+code, XY, UV+CLUT, colour, XY,
 * UV+TPAGE, ...). */
typedef struct {
    u32 tag;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
    u32 rgb3;
    DVECTOR xy3;
    u32 uv3;
} SharedGt4Packet;

/* POLY_GT3 as GPU command words (colour+code, XY, UV+CLUT, colour, XY,
 * UV+TPAGE, ...) between the texture-window command (from the record) and the
 * E2 reset word. */
typedef struct {
    u32 tag;
    u32 window;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
    u32 window_reset;
} SharedGt3WindowRecordPacket;

/* POLY_GT4 as GPU command words (colour+code, XY, UV+CLUT, colour, XY,
 * UV+TPAGE, ...) between the texture-window command (from the record) and the
 * E2 reset word. */
typedef struct {
    u32 tag;
    u32 window;
    u32 rgb0;
    DVECTOR xy0;
    u32 uv0;
    u32 rgb1;
    DVECTOR xy1;
    u32 uv1;
    u32 rgb2;
    DVECTOR xy2;
    u32 uv2;
    u32 rgb3;
    DVECTOR xy3;
    u32 uv3;
    u32 window_reset;
} SharedGt4WindowRecordPacket;

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
