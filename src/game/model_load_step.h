#ifndef MEMORIES_DECOMP_MODEL_LOAD_STEP_H
#define MEMORIES_DECOMP_MODEL_LOAD_STEP_H

#include "../types.h"

extern char gModel_szLoadModelTraceFormat[];

/* Executes one phase of an already-started model-slot load and publishes its
 * next phase. Phase 0 and the 0xFF idle/finished marker do not advance. A set
 * is_active ends the phase sequence; callers poll readiness separately. */
void Model_AdvanceSlotLoad(s32 index);

#endif
