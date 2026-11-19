#ifndef MEMORIES_DECOMP_WIREFRAME_RENDERERS_H
#define MEMORIES_DECOMP_WIREFRAME_RENDERERS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* The line packets the drivers build in the packet work area. */
/* Closed LINE_F4 behind a draw-mode word: the packet both triangle
 * wireframe drivers build. */
typedef struct {
    u32 tag;
    u32 draw_mode;
    u32 line;
    DVECTOR xy[4];
    u32 terminator;
} WireTrianglePacket;

/* Three-point LINE_F3: code word, three XYs, terminator. */
typedef struct {
    u32 line;
    DVECTOR xy[3];
    u32 terminator;
} WirePolyline3;

/* The quad wireframe packet: a draw-mode word and the polylines 0-1-3 and
 * 3-2-0 under one tag. */
typedef struct {
    u32 tag;
    u32 draw_mode;
    WirePolyline3 edge[2];
} WireQuadPacket;

/* HMD primitive drivers: each consumes one primitive block and returns the next. */
u32 *Model_DrawFt3Wireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawGt3Wireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawFt4Wireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawGt4Wireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawFt3WindowRecordWireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawGt3WindowRecordWireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawFt4WindowRecordWireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawGt4WindowRecordWireframes(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawSharedGt3Wireframes(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedGt4Wireframes(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedGt3WindowRecordWireframes(GsARGUNIT_SHARED *arg);
u32 *Model_DrawSharedGt4WindowRecordWireframes(GsARGUNIT_SHARED *arg);

#endif
