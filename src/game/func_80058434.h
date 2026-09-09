#ifndef MEMORIES_DECOMP_FUNC_80058434_H
#define MEMORIES_DECOMP_FUNC_80058434_H

#include "../types.h"

/* Retail callers store an unused fifth argument on the stack. Keep that
 * call-site contract without adding a parameter the callee never reads. */
#ifdef FUNC_80058434_CALL_WITH_UNUSED_ARG
void func_80058434(s32 direction, s32 yaw, s32 pitch, s32 distance, s32 unused);
#else
void func_80058434(s32 direction, s32 yaw, s32 pitch, s32 distance);
#endif

#endif
