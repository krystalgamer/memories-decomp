#ifndef MEMORIES_DECOMP_MODEL_GET_SLOT_READINESS_H
#define MEMORIES_DECOMP_MODEL_GET_SLOT_READINESS_H

#include "../types.h"

/* Returns 2 while the load phase differs from 0xFF, otherwise the active
 * byte's nonzero test. Load requests, transfer completion and
 * Model_AdvanceSlotLoad establish the phase sequence; this is not a Boolean
 * predicate. Slot lookup, int ABI and the phase-before-active read stay exact. */
int Model_GetSlotReadiness(int index);

#endif
