#ifndef MEMORIES_DECOMP_FUNC_80058938_H
#define MEMORIES_DECOMP_FUNC_80058938_H

#include "../types.h"
#include "model.h"

/* Fills a model slot's tint request block. `bits` selects which of the
   per-request fields are written; func_80058838 builds it from its varargs
   and func_80059AF8 passes 0. */
void func_80058938(s32 a0, s32 a1, ModelTintColor a2, ModelTintColor a3,
                   s32 a4, s32 bits);

#endif
