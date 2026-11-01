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
 * unmatched_asm, once its consumers are known to agree. The remaining local
 * function declarations are deliberate disagreements listed below rather
 * than duplication waiting to be moved. `make check-unmatched-contracts` prints the current counts and
 * rejects new unaccounted declaration sites.
 *
 * Hand-written assembly belongs here on the same terms. The status differs but
 * the reason does not: handwritten_asm functions have no defining C
 * translation unit either, so there is nowhere else for a shared declaration
 * to live, and every consumer otherwise writes its own. The primitive handler
 * entry points at the end of this file arrived that way.
 *
 * WHAT DOES NOT GO HERE, AND WHY THIS FILLS UP SLOWLY
 *
 * Several functions need incompatible declarations because the caller-visible
 * type is what makes code generation match. Those variants still belong here:
 *
 *   SD_SEPlay       (u32, s32, s32), (s32, s32, s32) and (u16, u8, s8)
 *
 * Where a consumer declares no parameters and calls with none, the argument
 * register is not empty. It holds the CALLER'S OWN incoming parameter, still
 * live because nothing clobbered it. That is worth checking per site rather
 * than assuming, because it decides whether the declaration can be fixed:
 *
 *   - model_slot_support.c called Model_DrawSlot with no argument while $a0
 *     still held its own `index`. Writing that argument explicitly costs
 *     nothing, because the register already holds the value, so the true
 *     one-parameter signature is used there now.
 *   - Model_LoadSlot called Model_InitSlot with no arguments while the
 *     callee reads $a0 to $a3. All four still held Model_LoadSlot's own
 *     incoming arguments, so model_slot_control.c now writes
 *     Model_InitSlot(arg0, arg1, arg2, arg3) against the one four-argument
 *     prototype.
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

/* Load-bearing caller views that cannot share one flat prototype. Consumers
 * select the declaration they measured before including this header. */
/* BuildDeck_InitScreen, which used to lead this list with two consumers, has
 * matched and is declared by game/build_deck_init_screen.h. */

/* Model_DrawSlot, which had six consumers sharing one spelling, has matched
 * and is declared by game/model_draw_slot.h. */

/* The five below each have exactly one consumer today, so there is no second
 * spelling to reconcile and nothing was measured away to move them. They are
 * here because the issue asks for every unmatched prototype to live in one
 * place, not only the ones that had already drifted: a declaration with one
 * consumer is simply a duplicate that has not happened yet. Each names the
 * file that used to declare it. */

/* A buffer base address rather than a byte array anyone indexes: every user
 * either passes it to DisplayObject_ConfigureSpriteAtPositionWithResource or stores it into an object field, and
 * none of them read through it. DuelResult_UpdateOrbitSprite sized it [16], but nothing
 * takes its sizeof, so the bound was decorative. */

/* Six consumers use this second buffer base with the same unsized-byte-array
 * spelling: three resident transfer paths and three main-menu display paths.
 * Nothing takes its sizeof. */
extern u8 D_801AF800[];

/* One consumer, Model_ApplyTextureTint. These four contiguous halfwords are filled
 * as a RECT for StoreImage2, LoadImage2, and MoveImage. The linker assignments
 * establish the scratch storage, but no subsystem header owns it. */
extern s16 gModel_sTextureTintRectX;
extern s16 gModel_sTextureTintRectY;
extern s16 gModel_sTextureTintRectW;
extern s16 gModel_sTextureTintRectH;


/* One consumer, AiScript_Print. Both addresses are passed to printf as format
 * strings; the second also receives the script checkpoint byte. Their
 * contents remain unnamed, and no AI subsystem header owns them. */
extern const char gAiScript_szCheckpoint[];
extern const char gAiScript_szCheckpointFormat[];


/* Address-only declarations found by re-measuring the note above rather
 * than by a scan. Every one is installed as data -- into the duel scene
 * callback table -- and none is invoked from C. func_80029EC4, the fourth of
 * this class (gMain_apFrameServiceCallbacks[3], library_runtime.c), left for
 * game/library_card_grid.h when it matched.
 *
 * Seven of them were declared in duel_scene_callbacks.c, which is a file I
 * added when that table moved out of its blob. The argument there was that
 * taking a function's address does not depend on its signature, so a local
 * declaration was self-contained. That is true but beside the point: this
 * header held exactly this class -- func_80067220 and TextBox_DrawEntries,
 * until each matched -- and the issue asks for one declaration site rather
 * than a defensible second one.
 *
 * void (void) is the form all nine consumers already used. As with the
 * entries above, it is safe precisely because there are no call sites for it
 * to be wrong at, and it is what has to change if a caller is ever matched
 * and passes an argument. */

/* One byte at 0x8009B363, written by four files that share nothing else.
 *
 * frontend_scene_states.c clears it, and duel_effect_sound_commands.c,
 * script_op_open_campaign_map.c and main_run_selection_menus.c each store a value into it;
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
 * mistake this batch nearly shipped: ModelDebug_UpdateController returns s32,
 * Model_SeparateViewFromSlots returns s32 and takes three, and
 * DisplayObject_RenderSpriteSheet takes three. Each prototype below is copied
 * from the consumer that had it, not restated.
 * DisplayObject_RenderSpriteSheet has since matched and is declared by
 * display_object_runtime.h. ModelDebug_UpdateController is owned
 * by model_debug_controller.h. Model_UpdateAnimatedBattle has since matched
 * and is declared by game/model_scene_states.h beside
 * Model_UpdateExodiaPresentation.
 * Model_SeparateViewFromSlots has since
 * matched and is declared by game/model_separate_view.h. */
/* SD_AllocateSoundEffectVoice is now owned by sound_voice_allocator.h, and func_80015EF4
 * (Duel_RenderFieldCard3D) has matched and is declared by
 * game/duel_field_card_render.h. */


/* The text engine's control byte, shared with the dialog and duel-effect
 * screens. Text_HandleChoiceCommand sets it from a command nibble
 * (`c & 0xF0`), text_stream_commands.c reads it whole, and the other two test
 * it by mask -- 0x30 for the layout arm and 0x40 for the choice arm. No C
 * source defines it and the four that use it share no subsystem header, so it
 * is homeless by the rule at the top of this file.
 *
 * Script_OpSavePrompt also clears it, spelled with a .data section
 * attribute because it addresses the byte outside small data. */
#ifdef D_8009B34C_IN_DATA
extern u8 D_8009B34C __attribute__((section(".data")));
#else
extern u8 D_8009B34C;
#endif

/* Linker-resolved data whose matching-C consumers already share this header.
 *
 * These declarations are copied from the unanimous local spellings they
 * replace. Bounds are retained only where consumers already agreed on them;
 * unsized arrays remain address/range views rather than guessed object sizes.
 * gMain_bModeRunnerStep is deliberately separate from the guarded frontend mode
 * bytes owned by main_mode_state.h. */
extern u8 D_80010074[];
extern u8 D_80010090[];
extern u8 D_800100A8[];
extern u8 gMain_bModeRunnerStep;
extern s32 gGameOver_pTitleObject;


/* Also reclassified from matching_c by #3859: the mixed -G share. Each of
 * these matched only under a compiler profile whose GCC and MASPSX
 * small-data thresholds disagree, or only with register pins #3867 left
 * load-bearing, so its source became a build-integrated candidate and the
 * function was assembly again. Their declarations moved here from the unit
 * headers that held them, comments included, and headers that held nothing
 * else are gone.
 * SD_UpdateSecondaryObjectPitch takes sound.h's SDSecondaryObject, which cannot be
 * forward-declared here, so its declaration stays in sound.h for the one
 * caller, the SD_UpdateSecondaryObjectVoices candidate.
 * SD_RegisterSeqBlockNotes takes SDSeqBlock and lives in
 * sound_pending_entries.h for the SD_StepActiveCommand caller.
 * SD_SESetPan has no caller in C; its matching
 * definition uses sound_voice_pan.h. SD_SetVoiceEnvelopeFromTone is the named
 * sound operation and stays in sound.h. Ai_GetHandSearchCount is now matching C; its caller-specific
 * return declarations live in ai.h. */

struct CardList;
struct DuelEffectChannel;
struct DuelRitualResult;

/* The duel screen's per-frame view callback. It reads D_800F2848, programs
 * the geometry engine from its projection field -- SetGeomScreen,
 * SetGeomOffset, SetFarColor and SetFogNearFar -- and then walks the field
 * records.
 *
 * Duel_InitScene installs it rather than calling it, as `gMain_apFrameServiceCallbacks[3] =
 * func_800164FC;` (src/game/duel_init_scene.c:155), so the declaration
 * has to match the definition exactly for the address to be taken. */

/* gScript_apfnCommandHandlers handler: the two-axis smooth scroll stepper Script_OpViewportTween
 * hands the scene over to. On its first frame it derives the per-frame 16.16
 * deltas from the distance to the target over the remaining frame count, then
 * advances both accumulators, publishes their high halves as the camera
 * position, and snaps to the target when the counter runs out. */
/* Entry 5 of the frontend step table gDebugMenu_apfnPrimaryPageSteps (frontend_step_tables.c):
 * the debug sound test. It steps gDebug_nSceneOrSoundID from the pad, plays
 * the selected sound effect or BGM, and stops all sound on START. */
/* One step of the card list's cursor and paging input.
 *
 * It first places the scroll box, deriving a y position from the combined
 * first and cursor rows scaled across 152 pixels by sort_row_count. It then
 * settles any outstanding scroll: while first differs from first_target it
 * moves first one row towards it, refreshes eight rows through
 * CardList_CreateSlotTextBoxes and returns immediately, so a page scroll is animated a row
 * per call rather than jumped. Only once the two agree does it read the pad
 * for paging and sorting.
 *
 * The return value is a handled flag: build_deck_pane_input.c, the only
 * consumer, tests it against zero at both call sites and does no more work
 * when it is set. */
/* One frame of choice-cursor input on a dialog's text-box record. Returns 1
 * when the repeat pad held a direction or R1 -- whether or not the cursor
 * actually moved, because a clamped edge still counts as handled and returns
 * before the cursor sound -- and 0 when it held none, which is the caller's
 * signal to look at the other buttons.
 *
 * R1 wraps past the last choice to the first; up and down clamp. The record
 * parameter is only forwarded to Dialog_HighlightChoice, which takes the same
 * `u8 *record` view in dialog_highlight_choice.h; Script_OpSavePrompt
 * (script_op_save_prompt.c) holds the same object as
 * DuelEffectChannel * and casts. */
/* Unmatched linker data consumed by matching C. These declarations preserve
 * the existing caller types. The guarded arms are measured code-generation
 * differences: scalar small-data access, array/address access, explicit
 * .data placement, and pointer arithmetic are not interchangeable here. */
extern u8 gSD_szSeGroupCountFormat[];
extern u8 gSD_szSmfMixerOutFormat[];
extern u8 gSD_szCdMixerOutFormat[];
extern char gAiScript_szNestOverflowError[];
extern char gAiScript_szSourceFile[];
extern char gAiScript_szReturnUnderflowError[];

#include "game/main_mode_state.h"

#ifdef D_8009B370_AS_BYTE_ARRAY
extern u8 D_8009B370[];
#else
extern u16 D_8009B370;
#endif
extern u16 D_8009B372;
/* DuelScene_UpdateBattle (src/game/duel_scene_battle.c) reads it through a
 * %hi/%lo pair into the load's own register, which is the .data form. */
#ifdef D_8009B374_IN_DATA
extern u16 D_8009B374 __attribute__((section(".data")));
#else
extern u16 D_8009B374;
#endif

#ifdef D_8009B_MODEL_VISIBLE
extern ModelBytes8 gModel_ViewSnapshot;
#endif

extern u8 D_8018C7D8[];
extern u8 D_801B122B[];
extern u8 D_801B1238[];
extern u8 tent_TradeSaveBuffer[];
extern u8 D_801D9174[];
extern u8 D_801D9174_b[];
extern u8 gSD_SeBankVolInfoTag __attribute__((section(".data")));
extern u8 gSD_SmfBankVolInfoTag __attribute__((section(".data")));
#ifdef GCAMPAIGN_SCENE_INDEX_AS_ARRAY
extern u8 gCampaignSceneIndex[];
#elif defined(GCAMPAIGN_SCENE_INDEX_AS_SCALAR)
extern u8 gCampaignSceneIndex;
#else
extern u8 gCampaignSceneIndex __attribute__((section(".data")));
#endif
extern u8 gDuel_bTerrainCodegenAlias[];
extern u8 gFile_szSuMrgPath[];

#endif
