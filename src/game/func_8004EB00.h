#ifndef MEMORIES_DECOMP_FUNC_8004EB00_H
#define MEMORIES_DECOMP_FUNC_8004EB00_H

#include "../types.h"

/* The model effect handler table at 0x800114E8: four functions of (s32, s32)
 * returning s32.
 *
 * func_8004EB00, the model scene mode 15 controller
 * (src/game/model_intro_controller.c), is the only source in the tree that
 * names it, and as a candidate it declared the symbol for itself because the
 * unit had no header to take it from. No C source defines the table; it is
 * data the listing reaches and nothing here initialises.
 *
 * The tree already treats it as a known entity even though nothing declared
 * it: config/slus_01411/functions.csv describes three functions as handlers of
 * this table -- func_8006C37C (the 67-point disc effect, matching_c),
 * func_8006CD78 (the burst effect) and func_8006F1B4 (the spark burst, "the
 * fourth handler in D_800114E8") -- and each of those units' headers names the
 * table in its prose without declaring it. The remaining entry is not
 * identified anywhere in this tree, so this comment does not name it.
 *
 * The four-entry bound is corroborated independently of the declaration. The
 * one consumer copies the whole table into a local array in a single
 * assignment through ModelSlotS32Quad (model.h):
 *
 *     *(ModelSlotS32Quad *)handlers = *(ModelSlotS32Quad *)D_800114E8;
 *
 * and that type carries a sizeof == 0x10 assertion, which is four pointers.
 *
 * Unlike the duel scene-state globals, this symbol is not gp-relative: the
 * matched controller reaches it with lui %hi / addiu %lo at its entry and
 * nothing else, zero gp-relative accesses.
 * That is a property of this symbol rather than of the function -- the same
 * body reaches D_8009AF88, D_8009AF96 and D_8009AF9A through %gp_rel -- so the
 * declaration is moved verbatim, array form and all, because the form is what
 * the addressing depends on. */
extern s32 (*D_800114E8[4])(s32, s32);

#endif
