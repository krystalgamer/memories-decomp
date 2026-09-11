#ifndef MEMORIES_DECOMP_UNMATCHED_H
#define MEMORIES_DECOMP_UNMATCHED_H

#include "types.h"
#include "ygo_types.h"

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
 * unmatched_asm, once its consumers are known to agree. There are 205 such
 * functions today. Sixty-nine have their declarations here. One hundred
 * are build-integrated candidates that keep theirs in the header of the unit
 * they came from: src/candidates/ still gives each of them a defining C
 * translation unit, so that header is a home in the sense this one is not
 * (see the #3859 sections at the end). Four remain local at seven sites, and
 * all four are the deliberate disagreements listed below rather than
 * duplication waiting to be moved. The remaining thirty-two have no
 * executable reference from matching C, so this header does not invent
 * signatures for them.
 *
 * Hand-written assembly belongs here on the same terms. The status differs but
 * the reason does not: handwritten_asm functions have no defining C
 * translation unit either, so there is nowhere else for a shared declaration
 * to live, and every consumer otherwise writes its own. The primitive handler
 * entry points at the end of this file arrived that way.
 *
 * WHAT DOES NOT GO HERE, AND WHY THIS FILLS UP SLOWLY
 *
 * A local declaration is not always duplication. Several of these functions
 * are declared incompatibly on purpose, because the declaration is what makes
 * the caller's code generation match:
 *
 *   func_8004CB0C   void (s32, s32, s32, s32) in the Model_LoadMonsterMerge
 *                   candidate, src/candidates/func_80056504.c, and
 *                   void (void) in model_slot_setup.c
 *   func_80013C28   void (u8, u8 *, u32 *) in file_transfer_runtime.c,
 *                   void (s32) elsewhere
 *   func_80042188   first parameter spelled s32 and SpritePrim *.
 *                   Measured, and it is not a spelling difference: the high
 *                   half of the fourth argument selects a six-entry jump
 *                   table, and the arms disagree about what the first
 *                   argument is. The sprite arms pass it straight to
 *                   GsSortFastSprite/GsSortFlipSprite/GsSortSprite, where it
 *                   is a GsSPRITE *; the four-vertex arm never dereferences
 *                   it and only tests `and $s2, 0x4000000`, which is the
 *                   `v | 0x4000000` the func_80040DD8 and func_80041068
 *                   candidates build. Since #3859 moved those two out of
 *                   matching C, only the sprite arms remain there.
 *                   One flat prototype would have to be wrong for one caller
 *                   or the other, so it stays out until the arms are split.
 *                   The u8 * spelling is gone: func_80016784.c held the last
 *                   one and now builds a SpritePrim, like the other two
 *                   sprite callers, so only the two real arms remain.
 *   SD_SEPlay       (u32, s32, s32), (s32, s32, s32) and (u16, u8, s8)
 *   Ai_GetHandSize  s32 (void) in ai_card_ranges.c, whose code needs the
 *                   widened return, and the definition's s8 (void) in
 *                   ai_fusion.c
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
 * model_scene_states.c and model_slot_support.c. The last of those is the site
 * described above, which used to declare no parameters; once it was given the
 * true one-parameter signature all three agreed, which is what made this
 * safe to move.
 *
 * func_800164FC also calls it, but had no declaration at all -- it was
 * relying on an implicit one, which is worse than a duplicate because there
 * is no spelling to disagree with. Its candidate takes this header now.
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
void func_800559D4(s32);            /* model_scene_states.c */
void func_8005E808(u8 *);           /* model_effect_state.c, and the candidate
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

/* One consumer, func_8004E9A0.c. The four contiguous halfwords at
 * 0x8009B468..0x8009B46E are filled as a RECT before MoveImage, while
 * D_800F569F is only indexed at zero as the animation gate. Their linker
 * assignments establish storage but no subsystem header owns them, so their
 * existing declarations move here without assigning broader semantics. */
extern s16 D_8009B468;
extern s16 D_8009B46A;
extern s16 D_8009B46C;
extern s16 D_8009B46E;
extern u8 D_800F569F[];

/* One consumer, func_800582C0.c. These four contiguous halfwords are filled
 * as a RECT for StoreImage2, LoadImage2, and MoveImage. The linker assignments
 * establish the scratch storage, but no subsystem header owns it. */
extern s16 D_8009B470;
extern s16 D_8009B472;
extern s16 D_8009B474;
extern s16 D_8009B476;

/* One consumer, func_8005C374.c, which writes its three parameters to these
 * contiguous bytes. Their individual roles remain unknown, and no movie
 * subsystem header owns the storage. */
extern u8 D_8009B4A0;
extern u8 D_8009B4A1;
extern u8 D_8009B4A2;

/* One consumer, AiScript_Print. Both addresses are passed to printf as format
 * strings; the second also receives the script checkpoint byte. Their
 * contents remain unnamed, and no AI subsystem header owns them. */
extern const char D_80011908[];
extern const char D_80011918[];

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

/* Two more undefined globals, each declared identically by every consumer
 * and only ever read or written as a scalar, so none of them has a
 * per-consumer addressing form to preserve.
 *
 * D_8009B162 is pinned by its neighbour: c_symbols.ld names D_8009B164
 * two bytes later, so it has no room for an element to carry its own name.
 * D_8009B23A's next name, D_8009B244, is ten bytes on, so that gap is an
 * upper bound rather than the size; nothing is named inside it and no
 * consumer reads past the halfword, so the u16 every consumer agrees on is
 * what is declared here and the bytes above it stay unclaimed. */
extern u16 D_8009B162;   /* nine declarers  */
extern u16 D_8009B23A;   /* eight declarers */

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
 * func_80067220 is not a callback at all -- model_primitive_handler.c returns its
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
 * dialog paths together with mem_card_dialog_runtime.c. D_8009B1D0 is
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

/* The thirty-two primitive handler entry points, in retail's own arm order in
 * memory.
 *
 * These are hand-written assembly rather than the unmatched assembly the rest
 * of this header covers, so they have no defining C translation unit either.
 * model_primitive_handler.c and model_handler_registry.c both only ever take
 * their addresses, never call them, so nothing in the tree checked one copy
 * against the other; the two files used to declare the set locally, then
 * shared it through a model_primitive_handler_entries.h that existed for no
 * other purpose. It folds in here.
 *
 * The reason that header could not simply be model_primitive_handler.h still
 * holds and is worth keeping written down: that header declares func_800603DC
 * as void *(u32), while model_handler_registry.c declares the same function
 * as `extern s32 (*func_800603DC())()` and uses it sixty-seven times. Those
 * spellings collide, so the registry cannot include model_primitive_handler.h
 * at all. This header does not declare func_800603DC, so both files can take
 * the entry points from here.
 */
void func_800612C0(void);
void func_8006151C(void);
void func_800617E0(void);
void func_80061A84(void);
void func_80061DDC(void);
void func_80062058(void);
void func_8006233C(void);
void func_80062600(void);
void func_80062978(void);
void func_80062BC0(void);
void func_80062E70(void);
void func_80063100(void);
void func_80063444(void);
void func_800636AC(void);
void func_8006397C(void);
void func_80063C2C(void);
void func_80063F90(void);
void func_80064248(void);
void func_80064568(void);
void func_80064868(void);
void func_80064C1C(void);
void func_80064EF4(void);
void func_80065234(void);
void func_80065554(void);
void func_80065928(void);
void func_80065BCC(void);
void func_80065ED8(void);
void func_800661C4(void);
void func_80066564(void);
void func_80066828(void);
void func_80066B54(void);
void func_80066E60(void);

/* The sixteen object handler entry points func_800608B8 dispatches to.
 *
 * Hand-written assembly like the primitive handlers above, and here too both
 * consumers -- model_primitive_handler.c and model_handler_registry.c -- only ever take
 * their addresses, as `return (s32)func_...`, so nothing in the tree checked
 * one declaration against the other. They had already drifted once:
 * model_primitive_handler.c spelled them `int X()` while model_handler_registry.c
 * spelled eight of them `void X(void)`.
 *
 * The retail image settled that, and the resolution is preserved here. Every
 * one of the sixteen reads its first argument through $a0 -- func_80067354
 * begins `lw $a3, 0x0($a0)` and saves $s0..$s3 into 0x28($a0)..0x34($a0) --
 * and every one leaves a value in $v0 immediately before `jr $ra`;
 * func_80067354 ends `lw $v0, 0x0($a0)` / `addiu $v0, $v0, 0x8`. So
 * `void X(void)` was wrong in both return type and arity, and the
 * unprototyped form below is the one the image does not contradict.
 *
 * What this does NOT claim: the parameter and return types are deliberately
 * left unspecified. The evidence shows that an argument is taken and a value
 * returned, not what either is. $a0 is dereferenced as a pointer and the
 * result derived from it, but nothing establishes the pointee's layout, so a
 * fuller prototype would assert more than is known.
 */
int func_80067354();
int func_8006759C();
int func_80067858();
int func_80067ABC();
int func_80067D94();
int func_80067FD0();
int func_8006825C();
int func_800684B4();
int func_8006875C();
int func_80068A00();
int func_80068D18();
int func_80068FD8();
int func_8006930C();
int func_800695A4();
int func_8006988C();
int func_80069B40();

/* A function #3859 moved back to generated assembly that has nowhere else
 * to be declared. The other pin and inline-asm functions it moved became
 * build-integrated candidates and keep their declarations in the header of
 * the unit they came from, because src/candidates/ still gives them a
 * defining C translation unit and the contract check accepts that home for a
 * candidate. The mixed -G share that follows took the other accepted home.
 *
 * func_800291E0 builds the display objects for slot `index` of the
 * D_800EA0E8 effect-resource records and returns the first of them. Its
 * source was only ever the target's own words in an asm block, so there is no
 * candidate, and u8 * is what its three callers agreed on rather than a
 * recovered type; func_800283F4.c casts the result to DisplayObject *, which
 * is the closest thing to evidence there is. The password overlay's shop.c
 * casts it to PasswordCardPreviewView *, a fourth view of the same object. */
u8 *func_800291E0(s32 index, s32 arg1, s32 arg2);

/* Also reclassified from matching_c by #3859: the mixed -G share. Each of
 * these matched only under a compiler profile whose GCC and MASPSX
 * small-data thresholds disagree, or only with register pins #3867 left
 * load-bearing, so its source now lives in src/candidates/ and the function
 * is assembly again. Their declarations moved here from the unit headers
 * that held them, comments included, and headers that held nothing else are
 * gone. The contract check accepts this home or the former unit header, but
 * not both.
 * func_8004A43C takes sound.h's SDSecondaryObject, which cannot be
 * forward-declared here, so its declaration stays in sound.h for the one
 * caller, the func_8004AAFC candidate. func_800476B4's one caller, the
 * func_80045514 candidate, declares it with an explicit extern that the
 * contract fingerprint records. func_80048768 and func_8004A6F8 have no
 * caller in C and get no declaration. Ai_GetHandSize is one of the deliberate
 * disagreements listed at the top. */

struct CardList;
struct DuelEffectChannel;
struct DuelRitualResult;

/* Fills the file position table. Its one caller is the boot-time start-up
 * func_80013154, a candidate since #3859 (src/candidates/func_80013154.c). */
/* The duel screen's per-frame view callback. It reads D_800F2848, programs
 * the geometry engine from its projection field -- SetGeomScreen,
 * SetGeomOffset, SetFarColor and SetFogNearFar -- and then walks the field
 * records.
 *
 * func_800179F4 installs it rather than calling it, as `D_800E9DB0[3] =
 * func_800164FC;` (src/candidates/func_800179F4.c:170), so the declaration
 * has to match the definition exactly for the address to be taken. */
void func_800164FC(void);

/* Builds the unique card id list for the combined deck and starts the fetch
 * for it.
 *
 * It copies the combined deck ids down into the sort buffer, sorts them with
 * qsort through Util_CompareS16, then walks the sorted run writing each id
 * that differs from the previous one into a second buffer, so the result is
 * the deck's ids deduplicated and in order, closed with a sentinel. It then
 * asks File_TryRequestAsyncTransfer for the block spanning the first id to
 * the last, with Duel_StepCardDataTransfer as the step callback, and records
 * the returned transfer's state with the primary-active bit set.
 *
 * The dedupe relies on the sort: it compares only against the previous
 * element, so it removes runs of equal ids rather than duplicates in
 * general. Duel_PopulateCombinedDeckData, still matched in
 * duel_card_record_lifecycle.c, depends on that. */
void Duel_RequestCombinedDeckData(void);

/* gDuelEffect_apfnGroupHandler entry: the terrain effect step. It reads
 * gDuel_bTerrain back after storing it and decrements in the same
 * expression. */
void func_80024E58(void);

/* Starts the async read of one card's effect artwork into slot `slot` of the
 * D_800EA0E8 record array. `value` is the card id: it is stored at +0x30 of
 * the record and turned into the disc position (value - 1) * 7 + CARD_COUNT,
 * seven sectors long, so a card's art is seven sectors and the table begins
 * one card's worth past the card data.
 *
 * It hands back the transfer descriptor it started, with `slot` already
 * stored in the descriptor's callback_data for func_800289BC to pick up, and
 * it has published the descriptor's status through D_8009B0F4 with
 * FILE_TRANSFER_STATE_PRIMARY_ACTIVE set. Every caller ignores the value,
 * which is why they can: the descriptor is reachable without it. The password
 * overlay's shop.c calls it too. */
FileTransferDescriptor *func_80029164(s32 slot, s32 value);

void func_8002A2F4(u8 *state);

/* Allocates and fills one effect request entry, storing arg0 as the id at
 * +0x18, and returns the entry or 0 when none was free. func_8002C68C.c used
 * to call it with no argument, leaving its own caller's id in $a0; it now
 * forwards that id, at no cost in instructions, so one spelling serves all
 * seven callers. */
u8 *func_8002C604(s32 arg0);

/* Returns the u16 result card id widened to s32. Declaring a narrow return at
 * the callers adds a zero-extension instruction that retail does not have.
 * duel_check_ritual.h keeps the result record and the recipe table. */
s32 Duel_CheckRitual(struct DuelRitualResult *out, s32 ritual_id);

/* D_80090C50 handler: the two-axis smooth scroll stepper Script_OpViewportTween
 * hands the scene over to. On its first frame it derives the per-frame 16.16
 * deltas from the distance to the target over the remaining frame count, then
 * advances both accumulators, publishes their high halves as the camera
 * position, and snaps to the target when the counter runs out. */
void Script_UpdateViewportTween(void);

/* Entry 5 of the frontend step table D_80090D84 (frontend_step_tables.c):
 * the debug sound test. It steps gDebug_nSceneOrSoundID from the pad, plays
 * the selected sound effect or BGM, and stops all sound on START. */
void func_800307B8(void);

/* Four-phase callback for the 0x2189, 0x4C-sector duel reward request. */
void func_80032184(FileTransferDescriptor *descriptor, s32 mode);

/* One step of the card list's cursor and paging input.
 *
 * It first places the scroll box, deriving a y position from the combined
 * first and cursor rows scaled across 152 pixels by sort_row_count. It then
 * settles any outstanding scroll: while first differs from first_target it
 * moves first one row towards it, refreshes eight rows through
 * func_80031E04 and returns immediately, so a page scroll is animated a row
 * per call rather than jumped. Only once the two agree does it read the pad
 * for paging and sorting.
 *
 * The return value is a handled flag: build_deck_pane_input.c, the only
 * consumer, tests it against zero at both call sites and does no more work
 * when it is set. */
s32 func_800330BC(struct CardList *list);

/* One frame of choice-cursor input on a dialog's text-box record. Returns 1
 * when the repeat pad held a direction or R1 -- whether or not the cursor
 * actually moved, because a clamped edge still counts as handled and returns
 * before the cursor sound -- and 0 when it held none, which is the caller's
 * signal to look at the other buttons.
 *
 * R1 wraps past the last choice to the first; up and down clamp. The record
 * parameter is only forwarded to Dialog_HighlightChoice, which takes the same
 * `u8 *record` view in dialog_highlight_choice.h; func_8002EE94
 * (src/candidates/func_8002EE94.c) holds the same object as
 * DuelEffectChannel * and casts. */
s32 Dialog_ReadChoiceInput(u8 *record);

/* The per-frame half of Dialog_OpenChoice's state: that builds the choice
 * list once, and this runs it, reading the cursor input and repainting the
 * entries. text_box_state_callbacks.c installs it in two adjacent D_80090E64
 * slots. */
void Dialog_UpdateChoice(struct DuelEffectChannel *object);

/* D_80090EAC entry: the fade command. Bit 6 of its operand sets D_8009B140 from
 * the D_8009AF74 pair, bit 5 sets it to 4, and bit 4 starts a fade -- white
 * through Fade_InitOutColor when bit 0 is set. The bits are tested in that
 * order and are not exclusive. */
void func_800388D8(u8 *object);

/* Two entries of D_80090FB0, the pair that builds packets in the scratchpad
 * rather than only running callbacks. func_80040DD8 takes the list at
 * D_800EFE38[4] and is 8 wide; func_80041068 takes D_800EFE38[5] and is 12
 * wide by 0x3C high. Both are reached only through that table. */
void func_80040DD8(void);
void func_80041068(void);

void func_80047480(void);

/* Three arguments, and no result: sound_spatialization.c already declared it
   this way and matched, while two other files carried `extern int
   SD_SetVoiceVolume()`. The int was never read anywhere in the tree. */
void SD_SetVoiceVolume(s32 voice, s32 left, s32 right);

/* Rebuilds the voice tables. func_80049BAC.c calls it right after
 * SD_ResetSequenceTracks, and func_8004A6D8 is a one-call wrapper for it. */
void func_8004A518(void);

/* The definition in src/candidates/func_8004B374.c takes two parameters.
 * sound_sequence_parser.c, the one caller, passes a third, and that call
 * is what this spells. */
void func_8004B374(s32 channel, s32 value, s32 unused);

void func_8004B854(void);

s32 SD_FindMidiTrackChunk(s32 offset);

/* Starts the asynchronous transfer that fills one model slot with a monster
 * merge record, and records the slot's display properties while the request
 * is in flight.
 *
 * Bit 0x80 of slot is a flag rather than part of the index. A negative model
 * id reuses the id already in the slot, and each of p2 to p5 follows the same
 * "negative means leave alone" convention.
 *
 * Returns zero once a transfer has been requested and one when the model id
 * has no record to request. Both callers discard it. The candidate source
 * carries the full account of which slots load from which file, and of the
 * three model-id ranges that have no record. */
s32 Model_LoadMonsterMerge(
    s32 slot, s32 model, s32 p2, s32 p3, s32 p4, s32 p5, s32 arg6
);

/* An AI script opcode, installed by ai_script_commands.c's table. */
void AiScript_CountCards(void);

#endif
