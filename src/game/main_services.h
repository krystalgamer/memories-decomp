#ifndef MEMORIES_DECOMP_MAIN_SERVICES_H
#define MEMORIES_DECOMP_MAIN_SERVICES_H

#include "../types.h"

/* The per-frame callback registry the resident service pump walks.
 *
 * There are four slots, not one. func_8001306C calls every non-null slot in
 * order once per frame, and func_800134B4 clears all four. The size is not a
 * guess: c_symbols.ld places D_800E9DB0 at 0x800E9DB0 and the next symbol,
 * D_800E9DC0, sixteen bytes later, which is exactly four pointers.
 *
 * The main_menu overlay only ever uses slot 0, so it previously declared the
 * registry as a bare `void (*)(void)` -- and, where it only cleared the slot,
 * as an `s32`. Both spellings write the right address, because slot 0 sits at
 * the base, but they hide that three more slots follow and that something else
 * may own them. Reaching slot 0 by index says what the code is really doing.
 *
 * This is a registry of independent slots, NOT a chain: no slot is called with
 * arguments, none returns a value, and nothing enforces an order beyond the
 * index. Which subsystem owns slots 1 to 3 is still unknown -- every writer
 * found, in C and in the generated assembly for both the resident image and
 * the overlays, materializes %lo(D_800E9DB0) with no displacement, so slot 0
 * is the only one anything is known to write. */
extern void (*D_800E9DB0[4])(void);

/* The single extra callback the pump runs after the four slots, and that
 * func_800134B4 clears alongside them. Only main_services.c refers to it. */
extern void (*D_8009B0B8)(void);

#endif
