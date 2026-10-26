#ifndef MEMORIES_DECOMP_DIRECT_POLYGON_RENDERERS_H
#define MEMORIES_DECOMP_DIRECT_POLYGON_RENDERERS_H

#include "../types.h"
#include "func_8006151C.h"
#include "func_80061A84.h"

/*
 * HMD primitive drivers: each consumes one primitive block and returns the
 * next. The Gt4 drawers stay hand-written assembly with address-only shapes.
 */
u32 *Model_DrawFt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawGt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawFt4Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawGt4Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
void Model_DrawGt4WindowRecordPrimitives(void);
u32 *Model_DrawTwoSidedFt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedGt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedFt4Primitives(GsARGUNIT_NORMAL *arg);
void Model_DrawTwoSidedGt4Primitives(void);
u32 *Model_DrawTwoSidedFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
void Model_DrawTwoSidedGt4WindowRecordPrimitives(void);
u32 *Model_DrawSemiFt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawSemiGt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawSemiFt4Primitives(GsARGUNIT_NORMAL *arg);
void Model_DrawSemiGt4Primitives(void);
u32 *Model_DrawSemiFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawSemiGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawSemiFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
void Model_DrawSemiGt4WindowRecordPrimitives(void);
u32 *Model_DrawTwoSidedSemiFt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedSemiGt3Primitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedSemiFt4Primitives(GsARGUNIT_NORMAL *arg);
void Model_DrawTwoSidedSemiGt4Primitives(void);
u32 *Model_DrawTwoSidedSemiFt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedSemiGt3WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
u32 *Model_DrawTwoSidedSemiFt4WindowRecordPrimitives(GsARGUNIT_NORMAL *arg);
void Model_DrawTwoSidedSemiGt4WindowRecordPrimitives(void);

#endif
