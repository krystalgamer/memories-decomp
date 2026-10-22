#ifndef MEMORIES_DECOMP_WIREFRAME_RENDERERS_H
#define MEMORIES_DECOMP_WIREFRAME_RENDERERS_H

#include "../types.h"

/* Address-only declarations; the handwritten caller-scratch ABI is unchanged. */
void Model_DrawFt3Wireframes(void);
void Model_DrawGt3Wireframes(void);
void Model_DrawFt4Wireframes(void);
void Model_DrawGt4Wireframes(void);
void Model_DrawFt3WindowRecordWireframes(void);
void Model_DrawGt3WindowRecordWireframes(void);
void Model_DrawFt4WindowRecordWireframes(void);
void Model_DrawGt4WindowRecordWireframes(void);
void Model_DrawSharedGt3Wireframes(void);
void Model_DrawSharedGt4Wireframes(void);
void Model_DrawSharedGt3WindowRecordWireframes(void);
void Model_DrawSharedGt4WindowRecordWireframes(void);

#endif
