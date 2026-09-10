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
 * unmatched_asm, once its consumers are known to agree. There are 52 such
 * functions today. Thirty-nine have caller-visible contracts here. Three
 * remain local at seven sites, and all three are the deliberate disagreements
 * listed below rather than duplication waiting to be moved. The remaining ten
 * have no executable reference from matching C, so this header does not invent
 * signatures for them.
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
 *   func_80042188   first parameter spelled s32 and SpritePrim *.
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
 *                   The u8 * spelling is gone: func_80016784.c held the last
 *                   one and now builds a SpritePrim, like the other two
 *                   sprite callers, so only the two real arms remain.
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

/* Three consumers, identical spelling in all three: model_scene_setup.c,
 * model_cleanup.c and model_slot_support.c. The last of those is the site
 * described above, which used to declare no parameters; once it was given the
 * true one-parameter signature all three agreed, which is what made this
 * safe to move.
 *
 * func_800164FC.c also calls it, but had no declaration at all -- it was
 * relying on an implicit one, which is worse than a duplicate because there
 * is no spelling to disagree with. It takes this header now.
 *
 * src/overlays/overworld/set_location.c spells the parameter int rather than
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
void func_8002ACA4(u8 *);           /* library_runtime.c */

/* One consumer, duel_effect_tables.c, as a DuelEffectHandler table entry. */
void func_800262D4(void);

/* A buffer base address rather than a byte array anyone indexes: every user
 * either passes it to func_800428A8 or stores it into an object field, and
 * none of them read through it. func_80020D4C sized it [16], but nothing
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
 * rather than retyped, which is a separate question from centralizing it.
 *
 * Six consumers do NOT take the plain arm, and both reasons are load bearing.
 * Five carry section(".data"), which is the -G8 lever that keeps the address
 * being rebuilt per access instead of resolved gp-relative, and
 * duel_load_package_stage.c additionally spells it u8 * because it does
 * pointer arithmetic on it. Those two arms are selected by
 * D_8009B118_IN_DATA and D_8009B118_IS_POINTER_IN_DATA, the same shape
 * input.h and sound.h use, rather than by re-declaring the symbol locally. */
#ifdef D_8009B118_IS_POINTER_IN_DATA
extern u8 *D_8009B118 __attribute__((section(".data")));
#elif defined(D_8009B118_IN_DATA)
extern s32 D_8009B118 __attribute__((section(".data")));
#else
extern s32 D_8009B118;
#endif

/* Nine more that nobody calls, found by re-measuring the note above rather
 * than by a scan. Every one is installed as data -- seven into the duel scene
 * callback table, func_80056D7C as a transfer completion callback, and
 * func_80029EC4 into D_800E9DB0[3] (library_runtime.c:318) -- and none is
 * invoked from C.
 *
 * Seven of them were declared in duel_scene_callbacks.c, which is a file I
 * added when that table moved out of its blob. The argument there was that
 * taking a function's address does not depend on its signature, so a local
 * declaration was self-contained. That is true but beside the point: this
 * header already holds exactly this class, in func_80042C08, func_80035E20
 * and func_80067220, and the issue asks for one declaration site rather than
 * a defensible second one.
 *
 * void (void) is the form all nine consumers already used. As with the
 * entries above, it is safe precisely because there are no call sites for it
 * to be wrong at, and it is what has to change if a caller is ever matched
 * and passes an argument. */
void func_8001BD88(void);
void func_8001D670(void);
void func_80019D18(void);
void func_8001B170(void);
void func_8001F55C(void);
void func_800218F0(void);
void func_80018FEC(void);
void func_80056D7C(void);
void func_80029EC4(void);

/* One byte at 0x8009B363, written by four files that share nothing else.
 *
 * frontend_scene_states.c clears it, and duel_effect_basic_commands.c,
 * func_8002EB48.c and main_run_selection_menus.c each store a value into it;
 * the last also reads it back to pass to func_8016866C. Four subsystems with
 * no header above them, which is what makes this genuinely homeless rather
 * than merely undeclared.
 *
 * The array spelling is a lever, not a size. Every one of the four is
 * compiled and assembled at -G8, where a byte-sized global would otherwise be
 * reached %gp_rel, and an array is what escapes that -- the table in
 * notes/build.md, second row. All four already write the same incomplete-array
 * form, so this declaration reproduces it exactly and no arm is needed.
 *
 * It is only ever indexed at [0], in all four files, so nothing here claims
 * the object is longer than one byte. c_symbols.ld agrees: gDuel_bTerrain
 * begins at 0x8009B364, immediately after it. */
extern u8 D_8009B363[];

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

/* Three caller-visible contracts that were outside the central inventory.
 *
 * func_800482B0 had one local declaration in SD_SEPlay. Its wider integer
 * parameters are the caller's measured view and remain distinct from the
 * narrower internal types in the build-integrated candidate.
 *
 * func_80015EF4 and func_80056828 were called implicitly. The former takes the
 * caller's record and scratch buffers; void * preserves the record boundary
 * without importing a subsystem type into this root header. The latter takes
 * the model player/slot index already passed by its sole caller. */
void func_800482B0(s32, s32, u8, s32, s32, s32);
void func_80015EF4(void *, u8 *, u8 *, s32 *);
void func_80056828(s32);

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

/* Two more of the same kind, found by asking which unmatched functions are
 * never called rather than which are declared oddly.
 *
 * func_80035E20 goes into the SAME +0x4C slot, in func_800391E4.c. That slot
 * holds at least two unmatched callbacks, which is the reason none of their
 * arities are established: the code that reads +0x4C back and calls it has
 * not been matched, so nothing in C has ever had to state their arguments.
 *
 * func_80067220 is not a callback at all -- func_800608B8.c returns its
 * address as an s32 -- but it lands in the same place for the same reason.
 *
 * Both were spelled without a prototype by their consumers, which is the
 * honest form for a function nobody calls. They keep a declared return type
 * here because their consumers cast the address, not the result. */
s32 func_80035E20();
int func_80067220();

/* Three more undefined globals, moved together for the same reason as the
 * batch above: this header is a serialisation point, so separate changes for
 * separate symbols conflict with each other without making any of them
 * easier to review.
 *
 * All three are one-byte state bytes that every consumer spells `extern u8`
 * and reads or writes whole. None is ever indexed, so there is no array
 * hiding behind the scalar and no per-consumer addressing form to preserve.
 *
 * D_8009B3F9 is the strongest of the three structurally: c_symbols.ld names
 * gMemCard_wDialogFlags one byte later, so the object is exactly one byte and
 * nothing can carry a second name inside it. It is the memory card slot the
 * MemCard* calls are issued against.
 *
 * D_8009B3EB and D_8009B174 have larger gaps to the next name -- two bytes
 * and eight -- but those are upper bounds rather than sizes, the way
 * D_8009B23A's was. Nothing is named inside either gap, and no consumer of
 * either reads past the byte, so the u8 all five consumers agree on is what
 * is declared and the bytes above stay unclaimed.
 *
 * Both of the latter two are packed state bytes rather than plain counters,
 * which is why the byte width matters to every reader: D_8009B3EB is switched
 * on through `& 0xF` with MEM_CARD_DIALOG_FLAG_* bits set above it, and
 * D_8009B174 carries a step in its low nibble with 0x20, 0x40 and 0x80 used
 * as independent flags. */
extern u8 D_8009B3F9;   /* five declarers */
extern u8 D_8009B3EB;   /* five declarers */
extern u8 D_8009B174;   /* five declarers */

/* Five more undefined scalars, batched for the reason the two batches above
 * give: this header serialises, so one change per symbol buys nothing and
 * conflicts with every other one in flight.
 *
 * None of the five is ever indexed anywhere in the tree, so each is a plain
 * scalar rather than an array wearing a scalar's spelling, and every consumer
 * already writes the same declaration. Neither the width nor the addressing
 * form is being changed here; the spellings below are copied from the
 * consumers that had them.
 *
 * Three are pinned exactly, with the next name sitting at precisely the end
 * of the declared width, so no element can hide inside them: D_8009B3C2 and
 * D_8009B3C4 are two bytes each with a name two bytes on, and D_8009B1D0 is
 * two bytes with D_8009B1D2 immediately after it.
 *
 * Two have a larger gap than their width and are treated the way D_8009B23A
 * was, as an upper bound rather than a size: D_8009B3D0's next name is eight
 * bytes on and D_8009B3F4's is five. Nothing is named inside either gap and
 * no consumer reads past the declared width, so the agreed type is what is
 * declared and the bytes above stay unclaimed.
 *
 * The first four are memory card state, shared by the create, load, save and
 * dialog paths together with data_transfer_request.c. D_8009B1D0 is
 * unrelated to them and belongs to the duel side; it is here because it
 * passed the same checks, not because it is part of that group. */
extern u16 D_8009B3C2;   /* four declarers */
extern u16 D_8009B3C4;   /* four declarers */
extern u32 D_8009B3D0;   /* four declarers */
extern s32 D_8009B3F4;   /* four declarers */
extern u16 D_8009B1D0;   /* four declarers */

/* The text engine's control byte, shared with the dialog and duel-effect
 * screens. Text_HandleChoiceCommand sets it from a command nibble
 * (`c & 0xF0`), text_stream_commands.c reads it whole, and the other two test
 * it by mask -- 0x30 for the layout arm and 0x40 for the choice arm. No C
 * source defines it and the four that use it share no subsystem header, so it
 * is homeless by the rule at the top of this file.
 *
 * func_8002EE94.c also clears it, spelled with a .data section attribute
 * because it addresses the byte outside small data. It does not include this
 * header, so the two never meet and no guarded arm is needed here; if it ever
 * does, that is what would go in. */
extern u8 D_8009B34C;

/* Linker-resolved data whose matching-C consumers already share this header.
 *
 * These declarations are copied from the unanimous local spellings they
 * replace. Bounds are retained only where consumers already agreed on them;
 * unsized arrays remain address/range views rather than guessed object sizes.
 * D_8009B26E is deliberately separate from D_8009B26C: the latter still has
 * incompatible scalar, array, and absolute-address views across its users. */
extern u8 D_80010074[];
extern u8 D_80010090[];
extern u8 D_800100A8[];
extern u8 D_8009AF2A;
extern u8 D_8009AF2C[2];
extern u8 D_8009AF2D;
extern s32 D_8009B0FC;
extern u8 D_8009B108;
extern u8 D_8009B110;
extern s32 D_8009B12C;
extern void (*D_8009B128)(void);
extern u8 D_8009B1B8;
extern u8 D_8009B26E;
extern u8 D_8009B2B4;
extern u8 D_8009B2B5;
extern u8 D_8009B2B6;
extern u8 D_8009B2B8;
extern u8 D_8009B2C0;
extern u8 D_8009B2E0;
extern u8 D_8009B2E8;
extern u8 D_8009B2EA;
extern void *D_8009B2EC;
extern u8 D_8009B2F0;
extern u16 D_8009B33A;
extern s32 D_8009B378;
extern s32 D_8009B3BC;
extern u8 D_8009B48E[2];
extern u8 D_8009B490[2];
extern u16 D_800F5678[];
extern s16 D_800EFE3C;

/* An eight-byte zeroed block with no common subsystem owner. Main_Init reads
 * its first byte as the initial sound-output state, while file transfer
 * control reads the two words as an address adjustment. The consumers share
 * no narrower state contract, so the raw byte-array view stays here. */
extern u8 D_800E9EC0[];

#endif
