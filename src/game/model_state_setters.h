#ifndef MEMORIES_DECOMP_MODEL_STATE_SETTERS_H
#define MEMORIES_DECOMP_MODEL_STATE_SETTERS_H

#include "../types.h"

/* The unit's two one-line setters, each storing its argument into a single
 * model state global and doing nothing else: func_80059AE0 writes the
 * halfword at D_8009AF92, func_80059AEC the byte at D_8009AFA4.
 *
 * Both are declared s32 here, which is the type the definitions already use --
 * src/types.h defines s32 as `signed int`. The store narrows to the global's
 * own width, which is what the matched bytes do: functions.csv records these
 * as the "straight unsigned-halfword G8 setter" and "straight unsigned-byte G8
 * setter", each matching with its GPREL16 relocation.
 *
 * func_80059AEC has no caller in matched C yet; it is declared here because
 * this is the unit's header, not because a consumer needs it today. */
void func_80059AE0(s32 value);
void func_80059AEC(s32 value);

#endif
