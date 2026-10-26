#ifndef MEMORIES_DECOMP_WIREFRAME_RENDERERS_H
#define MEMORIES_DECOMP_WIREFRAME_RENDERERS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

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
