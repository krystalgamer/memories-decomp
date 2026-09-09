#ifndef MEMORIES_DECOMP_UNMATCHED_H
#define MEMORIES_DECOMP_UNMATCHED_H

#include "types.h"

/* Declarations for functions and data that are still generated assembly.
 *
 * These have no defining C translation unit, so there is nowhere for a
 * per-TU header to live and every consumer has been writing its own extern.
 * That is what lets two files disagree about a function nothing in the tree
 * can check. This header is the single place those declarations belong.
 *
 * WHAT GOES HERE
 *
 * A function or global whose status in config/slus_01411/functions.csv is
 * unmatched_asm, once its consumers are known to agree. There are 53 such
 * functions today; 34 of them are declared locally in .c files, at 48 sites.
 *
 * WHAT DOES NOT GO HERE, AND WHY THIS FILLS UP SLOWLY
 *
 * A local declaration is not always duplication. Several of these functions
 * are declared incompatibly on purpose, because the declaration is what makes
 * the caller's code generation match:
 *
 *   func_8004CB0C   void (s32, s32, s32, s32) in model_load_monster_merge.c,
 *                   void (void) in model_slot_setup.c
 *   func_80013C28   void (u8, u8 *, u32 *) in file_cd_transfer.c,
 *                   void (s32) elsewhere
 *   func_80042188   first parameter spelled s32, u8 * and SpritePrim *
 *   SD_SEPlay       (u32, s32, s32), (s32, s32, s32) and (u16, u8, s8)
 *
 * Where a consumer declares no parameters and calls with none, the argument
 * register is not empty. It holds the CALLER'S OWN incoming parameter, still
 * live because nothing clobbered it. That is worth checking per site rather
 * than assuming, because it decides whether the declaration can be fixed:
 *
 *   - model_slot_support.c called func_800540B4 with no argument while $a0
 *     still held its own `index`. Writing that argument explicitly costs
 *     nothing, because the register already holds the value, so the true
 *     one-parameter signature is used there now.
 *
 *   - model_slot_setup.c calls func_8004CB0C the same way, and that one
 *     cannot be fixed. The callee reads $a0, $a1, $a2 and $a3, but this site
 *     sets only $a0. The other three are whatever the register file happened
 *     to hold, so there is no expression to write for them. Its `void (void)`
 *     declaration stays, and is not a mistake.
 *
 * So a symbol only moves here once every consumer's spelling is accounted
 * for, and a consumer that cannot state its arguments keeps its local
 * declaration. When a symbol needs different spellings per consumer it gets
 * arms selected by guard #defines, the way input.h and sound.h already do,
 * rather than one flat declaration. Entries are added a few at a time, each
 * gated on the full build. */

/* Two consumers, identical spelling in both: func_80049138.c and
 * sound_init.c. */
void func_80046294(void);

/* Two consumers. frontend_scene_states.c spelled the result `int` and
 * func_800307B8.c spelled it `s32`; types.h defines s32 as signed int, so the
 * two agree and the difference was only spelling. */
s32 func_80030294(void);

#endif
