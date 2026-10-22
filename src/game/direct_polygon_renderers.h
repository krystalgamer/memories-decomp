#ifndef MEMORIES_DECOMP_DIRECT_POLYGON_RENDERERS_H
#define MEMORIES_DECOMP_DIRECT_POLYGON_RENDERERS_H

#include "../types.h"
#include "func_8006151C.h"
#include "func_80061A84.h"

/* Retain the two existing typed interfaces and thirty address-only shapes. */
void Model_DrawFt3Primitives(void);
u32 *Model_DrawGt3Primitives(GsARGUNIT_NORMAL *arg);
void Model_DrawFt4Primitives(void);
u32 *Model_DrawGt4Primitives(GsARGUNIT_NORMAL *arg);
void Model_DrawFt3WindowRecordPrimitives(void);
void Model_DrawGt3WindowRecordPrimitives(void);
void Model_DrawFt4WindowRecordPrimitives(void);
void Model_DrawGt4WindowRecordPrimitives(void);
void Model_DrawTwoSidedFt3Primitives(void);
void Model_DrawTwoSidedGt3Primitives(void);
void Model_DrawTwoSidedFt4Primitives(void);
void Model_DrawTwoSidedGt4Primitives(void);
void Model_DrawTwoSidedFt3WindowRecordPrimitives(void);
void Model_DrawTwoSidedGt3WindowRecordPrimitives(void);
void Model_DrawTwoSidedFt4WindowRecordPrimitives(void);
void Model_DrawTwoSidedGt4WindowRecordPrimitives(void);
void Model_DrawSemiFt3Primitives(void);
void Model_DrawSemiGt3Primitives(void);
void Model_DrawSemiFt4Primitives(void);
void Model_DrawSemiGt4Primitives(void);
void Model_DrawSemiFt3WindowRecordPrimitives(void);
void Model_DrawSemiGt3WindowRecordPrimitives(void);
void Model_DrawSemiFt4WindowRecordPrimitives(void);
void Model_DrawSemiGt4WindowRecordPrimitives(void);
void Model_DrawTwoSidedSemiFt3Primitives(void);
void Model_DrawTwoSidedSemiGt3Primitives(void);
void Model_DrawTwoSidedSemiFt4Primitives(void);
void Model_DrawTwoSidedSemiGt4Primitives(void);
void Model_DrawTwoSidedSemiFt3WindowRecordPrimitives(void);
void Model_DrawTwoSidedSemiGt3WindowRecordPrimitives(void);
void Model_DrawTwoSidedSemiFt4WindowRecordPrimitives(void);
void Model_DrawTwoSidedSemiGt4WindowRecordPrimitives(void);

#endif
