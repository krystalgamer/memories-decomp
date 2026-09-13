#ifndef MEMORIES_DECOMP_MAIN_MODE_STATE_H
#define MEMORIES_DECOMP_MAIN_MODE_STATE_H

#include "../types.h"

/* Cross-subsystem frontend mode state.
 *
 * D_8009B269 is the next/base mode copied back when a mode runner exits.
 * D_8009B26C holds the active mode in its low bits plus lifecycle flags:
 * 0x20 for the animated-battle variant, 0x40 for mode initialization, and
 * 0x80 for the main-loop reset gate.
 *
 * These forms are codegen inputs under the named compiler profiles. Select
 * each byte independently before including this header or unmatched.h.
 * The defaults retain the current unmatched.h contract: scalar NEXT and
 * incomplete-array ACTIVE. AS_SCALAR and AS_ARRAY can also select those
 * defaults explicitly. Legacy address-based selectors remain compatible;
 * .data takes precedence, as it did in unmatched.h. */
#if defined(MAIN_MODE_STATE_NEXT_IN_DATA) || defined(D_8009B269_AS_SCALAR_DATA)
extern u8 D_8009B269 __attribute__((section(".data")));
#elif defined(MAIN_MODE_STATE_NEXT_AS_ARRAY) || defined(D_8009B269_AS_ARRAY)
extern u8 D_8009B269[];
#else
extern u8 D_8009B269;
#endif

#if defined(MAIN_MODE_STATE_ACTIVE_IN_DATA) || defined(D_8009B26C_AS_SCALAR_DATA)
extern u8 D_8009B26C __attribute__((section(".data")));
#elif defined(MAIN_MODE_STATE_ACTIVE_AS_SCALAR) || defined(D_8009B26C_AS_SCALAR)
extern u8 D_8009B26C;
#else
extern u8 D_8009B26C[];
#endif

#endif
