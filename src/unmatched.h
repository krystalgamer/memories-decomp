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
 *   func_800540B4   void (s32) in func_800528AC.c and model_cleanup.c,
 *                   void (void) in model_slot_support.c
 *   func_8004CB0C   void (s32, s32, s32, s32) in model_load_monster_merge.c,
 *                   void (void) in model_slot_setup.c
 *   func_80013C28   void (u8, u8 *, u32 *) in file_cd_transfer.c,
 *                   void (s32) elsewhere
 *   func_80042188   first parameter spelled s32, u8 * and SpritePrim *
 *   SD_SEPlay       (u32, s32, s32), (s32, s32, s32) and (u16, u8, s8)
 *
 * A call site that passes no argument to a function taking one is relying on
 * whatever the previous call left in the register. Giving those files one
 * agreed prototype changes what the compiler emits, so they keep their local
 * declarations until each is measured. When such a symbol does move here it
 * needs per-consumer arms selected by guard #defines, the way input.h and
 * sound.h already do, not one flat declaration.
 *
 * So entries are added a few at a time, each gated on the full build, and a
 * symbol is only added once every consumer's spelling is accounted for. */

/* Two consumers, identical spelling in both: func_80049138.c and
 * sound_init.c. */
void func_80046294(void);

/* Two consumers. frontend_scene_states.c spelled the result `int` and
 * func_800307B8.c spelled it `s32`; types.h defines s32 as signed int, so the
 * two agree and the difference was only spelling. */
s32 func_80030294(void);

#endif
