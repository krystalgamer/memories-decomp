#ifndef GAME_SHARED_PRIMITIVE_RENDERERS_H
#define GAME_SHARED_PRIMITIVE_RENDERERS_H

#include "../types.h"

/* Preserve the existing unspecified-parameter address-use declarations. */
int Model_PrepareSharedPrimitiveCaches();
int Model_DrawSharedGt3Primitives();
int Model_DrawSharedGt4Primitives();
int Model_DrawSharedGt3WindowRecordPrimitives();
int Model_DrawSharedGt4WindowRecordPrimitives();
int Model_DrawSharedTwoSidedGt3Primitives();
int Model_DrawSharedTwoSidedGt4Primitives();
int Model_DrawSharedTwoSidedGt3WindowRecordPrimitives();
int Model_DrawSharedTwoSidedGt4WindowRecordPrimitives();
int Model_DrawSharedSemiGt3Primitives();
int Model_DrawSharedSemiGt4Primitives();
int Model_DrawSharedSemiGt3WindowRecordPrimitives();
int Model_DrawSharedSemiGt4WindowRecordPrimitives();
int Model_DrawSharedTwoSidedSemiGt3Primitives();
int Model_DrawSharedTwoSidedSemiGt4Primitives();
int Model_DrawSharedTwoSidedSemiGt3WindowRecordPrimitives();
int Model_DrawSharedTwoSidedSemiGt4WindowRecordPrimitives();

#endif
