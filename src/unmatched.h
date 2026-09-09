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
 *   func_80042188   first parameter spelled s32, u8 * and SpritePrim *.
 *                   Measured, and it is not a spelling difference: the high
 *                   half of the fourth argument selects a six-entry jump
 *                   table, and the arms disagree about what the first
 *                   argument is. The sprite arms pass it straight to
 *                   GsSortFastSprite/GsSortFlipSprite/GsSortSprite, where it
 *                   is a GsSPRITE *; the four-vertex arm never dereferences
 *                   it and only tests `and $s2, 0x4000000`, which is the
 *                   `v | 0x4000000` display_object_list_renderers.c builds.
 *                   One flat prototype would have to be wrong for one caller
 *                   or the other, so it stays out until the arms are split.
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

/* Two rules for anything added here.
 *
 * A symbol belongs here only while it is genuinely homeless. Once its owning
 * subsystem is understood, move the declaration to that subsystem's header
 * rather than leaving it in this one.
 *
 * When a consumer cannot take the shared declaration because its spelling is
 * load-bearing -- a section attribute deciding gp-relative versus absolute
 * addressing under -G8, a volatile qualifier deciding whether a redundant
 * access survives, or an asm() alias keeping GCC from holding an address
 * across a call -- note the exception beside the declaration so it is not
 * quietly "fixed" later. */

/* Two consumers, identical spelling in both: func_80049138.c and
 * sound_init.c. */
void func_80046294(void);

/* Two consumers. frontend_scene_states.c spelled the result `int` and
 * func_800307B8.c spelled it `s32`; types.h defines s32 as signed int, so the
 * two agree and the difference was only spelling. */
s32 func_80030294(void);

/* Two consumers. main_run_duel_and_library.c spells the first and final
 * parameters with project aliases, while main_run_selection_menus.c uses the
 * equivalent unsigned int and int spellings. */
void func_800323F8(u32, void *, s32, s32);

/* One consumer, duel_scene_update.c, which calls it without arguments. */
void func_800235C0(void);

/* Three consumers, identical spelling in all three: func_800528AC.c,
 * model_cleanup.c and model_slot_support.c. The last of those is the site
 * described above, which used to declare no parameters; once it was given the
 * true one-parameter signature all three agreed, which is what made this
 * safe to move.
 *
 * func_800164FC.c also calls it, but had no declaration at all -- it was
 * relying on an implicit one, which is worse than a duplicate because there
 * is no spelling to disagree with. It takes this header now.
 *
 * src/overlays/overworld/camera_state.c spells the parameter int rather than
 * s32. The two are the same type, but that site is outside this change. */
void func_800540B4(s32);

/* The five below each have exactly one consumer today, so there is no second
 * spelling to reconcile and nothing was measured away to move them. They are
 * here because the issue asks for every unmatched prototype to live in one
 * place, not only the ones that had already drifted: a declaration with one
 * consumer is simply a duplicate that has not happened yet. Each names the
 * file that used to declare it. */
void func_80045514(void);           /* sound_runtime.c */
void func_800559D4(s32);            /* model_cleanup.c */
void func_8005E808(u8 *);           /* func_8005F91C.c, and the candidate
                                       source src/candidates/func_8005E808.c
                                       defines it with the same u8 * */
void func_8004ADE8(s32, s32, s32);  /* sound_sequence_events.c */
void func_8002ACA4(u8 *);           /* func_8002BAB4.c */

/* One consumer, duel_effect_tables.c, as a DuelEffectHandler table entry. */
void func_800262D4(void);

/* A buffer base address rather than a byte array anyone indexes: every user
 * either passes it to func_800428A8 or stores it into an object field, and
 * none of them read through it. func_80020D4C.c sized it [16], but nothing
 * takes its sizeof, so the bound was decorative. */
extern u8 D_801AF000[];

/* Six consumers use this second buffer base with the same unsized-byte-array
 * spelling: three resident transfer paths and three main-menu display paths.
 * Nothing takes its sizeof. */
extern u8 D_801AF800[];

/* A single word at 0x8009B118, four bytes: c_symbols.ld names D_8009B11C
 * immediately after it, so no element can hide inside. Fifteen files declared
 * it identically as `extern s32 D_8009B118;`, which is why it can move here
 * unchanged - there is no per-consumer spelling to preserve.
 *
 * It holds an address despite the s32 spelling: func_8003BF00.c passes it to
 * LoadImage2 as (u32 *) and also adds 0x800 to it. The s32 is left as-is
 * because every consumer already agrees on it; retyping it is a separate
 * question from centralizing it. */
extern s32 D_8009B118;

/* One consumer each, model_primitive_handler.c, which does not call either
 * one: its dispatcher returns them as function pointers for primitive codes
 * 0xD and 0x15. So the declaration fixes the pointer type rather than a call's
 * argument setup, and `void (void)` is what the consumer already spelled. */
void func_80033DB0(void);
void func_80034830(void);

/* Eleven unmatched_asm functions, each with a single consumer that declared
 * it locally. Moved together rather than one per change: this header is a
 * serialisation point, and separate PRs for separate symbols conflict with
 * each other without making any of them easier to check.
 *
 * All eleven clear the bar this header sets. Each has one spelling across the
 * tree, so there is no disagreement to resolve first. None is declared with
 * an empty parameter list, so none is a call site relying on whatever the
 * argument register happened to hold. None has an overlay consumer.
 *
 * Eight are void (void), called with no arguments against a no-argument
 * declaration, so declaration and call already agree. The other three are
 * not, and are worth naming because assuming otherwise is exactly the
 * mistake this batch nearly shipped: func_800534B8 returns s32,
 * func_80051350 returns s32 and takes three, and func_8004158C takes three.
 * Each prototype below is copied from the consumer that had it, not
 * restated. */
void func_8004DE24(void);
void func_8004EB00(void);
void func_8004FE2C(void);
void func_800507D0(void);
void func_80051A48(void);
void func_8005DBA4(void);
s32 func_80051350(s32 arg0, s32 arg1, s32 arg2);
s32 func_800534B8(void);
void func_8005C7BC(void);
void func_8005D378(void);
void func_8004158C(u8 *, s32, s32);

/* Three more undefined globals, each declared identically by every consumer
 * and only ever read or written as a scalar, so none of them has a
 * per-consumer addressing form to preserve.
 *
 * D_8009B162 and D_8009B2EB are pinned by their neighbours: c_symbols.ld
 * names D_8009B164 two bytes after the first and D_8009B2EC one byte after
 * the second, so neither has room for an element to carry its own name.
 * D_8009B23A's next name, D_8009B244, is ten bytes on, so that gap is an
 * upper bound rather than the size; nothing is named inside it and no
 * consumer reads past the halfword, so the u16 every consumer agrees on is
 * what is declared here and the bytes above it stay unclaimed. */
extern u16 D_8009B162;   /* nine declarers  */
extern u16 D_8009B23A;   /* eight declarers */
extern u8 D_8009B2EB;    /* six declarers   */

/* Nothing in the tree calls this one. Both consumers only take its address,
 * to install it in a display object's +0x4C slot: dialog_transition.c stores
 * it twice, once through (s32) and once through (u8 *), and
 * overworld/set_location.c stores it as a void *. No C source invokes +0x4C,
 * so whatever reads that slot back is still generated assembly.
 *
 * That means its arity is NOT established, and the (void) here is the form
 * set_location.c already used rather than a claim. It is safe precisely
 * because there are no call sites for it to be wrong at -- taking a
 * function's address does not depend on its signature. If a caller of +0x4C
 * is ever matched and passes an argument, this declaration is what has to
 * change, and dialog_transition.c's unprototyped `extern void
 * func_80042C08();` was quietly saying the same thing. */
void func_80042C08(void);

#endif
