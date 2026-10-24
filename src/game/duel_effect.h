#ifndef YUGIOH_GAME_DUEL_EFFECT_H
#define YUGIOH_GAME_DUEL_EFFECT_H

#include "../ygo_types.h"

#define DUEL_EFFECT_CHANNEL_COUNT 4
#define DUEL_EFFECT_ENTRY_COUNT 620
#define DUEL_EFFECT_OCCUPANCY_COUNT 240
#define DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE 0x4000
#define DUEL_EVENT_SCRIPT_FLAG_STARTED 0x8000
#define TEXT_BOX_FLAG_BUILD_REQUESTED 0x800
#define TEXT_BOX_FLAG_DONE 0x2000
#define TEXT_BOX_COMPLETION_MASK 0x2008
#define TEXT_BOX_FLAG_BUILD_ACTIVE 0x4000
#define DUEL_EFFECT_CHANNEL_FLAG_ACTIVE 0x8000
#define DUEL_EFFECT_STATE_FLAG_INITIALIZED 0x80
#define DUEL_EFFECT_STATE_INDEX_MASK 0x1F
#define DUEL_EFFECT_ENTRY_FLAG_ACTIVE 0x80
#define DUEL_EFFECT_ENTRY_HANDLER_INDEX_MASK 0x1F

/* One text/effect entry, 0x1C bytes, the element type of tent_DuelEffectEntries. The
   leading words are unnamed but must stay four-byte aligned: retail copies a
   whole entry with aligned lw/sw pairs during the compaction in
   DuelEffect_ProcessEntries, and a byte-aligned struct turns that into
   lwl/lwr. The password overlay reads two of the fields: code_00 is the
   entry's Shift-JIS glyph code, and the 0x0C pair is its local pixel
   position, which TextBox_GetGlyphAt searches to find the node under a
   coordinate. */
/* DuelEffectEntry is defined in ygo_types.h. DuelEffect_AppendEntry writes
   0x16 on its 0x100 path and 0x17 on its 0x80 path. The North American
   TextBox_DrawEntries reads both for glyph-kind-dependent palette/icon selection;
   these are not universal meanings for the shared effect-node fields.
   See notes/text-glyph-renderer.md. */

/* The entry occupancy table: one byte per DUEL_EFFECT_OCCUPANCY_COUNT entry,
 * declared with that bound because both consumers already wrote it and the
 * constant is defined above. DuelEffect_ResetOccupancy walks it from the base,
 * DuelEffect_FindFreeOccupancySlot scans for the first zero, and
 * DisplayObjectFade_ReleaseChannel clears the slot an object names at its
 * offset 0x10.
 *
 * Two hundred and forty bytes is well over the -G8 threshold, so unlike the
 * small arrays elsewhere in this tree the bound here costs nothing: it does
 * not move the object between sections. */
extern u8 tent_DuelEffectOccupancy[DUEL_EFFECT_OCCUPANCY_COUNT];

extern DuelEffectChannel D_800EB0F8[DUEL_EFFECT_CHANNEL_COUNT];
extern DuelEffectChannel D_800F04F8;
extern DuelEffectChannel D_800F055C;
/* PAL views of pool slots 2/3; retain the password caller's native names. */
#ifdef VERSION_EUROPE
#define gText_Channel2View D_800F04F8
#define gText_Channel3View D_800F055C
#endif
/* D_800EB15C is D_800EB0F8[1] under its own name: 0x800EB0F8 + 0x64, one
 * DuelEffectChannel in. It stays a second name because how this address is
 * spelled is a codegen input, and two matched functions record the shape:
 * debug_menu_update.c has to hold the array base in a local, since writing
 * `&D_800EB0F8[1]` inline folds the record offset into the address and
 * drops the `addiu` retail keeps for the call argument; and
 * FreeDuel_UpdateScreen has to hold this symbol in a local, since writing
 * `D_800EB15C + 0x34` inline folds the offset into the relocation and the
 * base instruction disappears (free_duel_functions.csv,
 * notes/overlays/matching-patterns.md). Neither of those measures the name
 * against `&D_800EB0F8[1]` -- Main_RunTwoPlayerDuelSetup passes that form directly and
 * matches -- so what is established is that the four files using this name
 * match as written; whether they would also match through the array index
 * is not measured. */
extern DuelEffectChannel D_800EB15C;
/* D_800EB12C is the word at D_800EB0F8 + 0x34: the flags_34 (:82) and
 * field_36 (:83) halfwords of record 0 under one name. Script_RunTick
 * (script_run_tick.c:23) and Password_UpdateShopScreen
 * (overlays/password/shop.c) each load it as a
 * word and test it through TEXT_BOX_COMPLETION_MASK against
 * TEXT_BOX_FLAG_DONE; retail loads it lui/lw (func_8002FA54.s:15-16).
 * Script_OpSavePrompt (script_op_save_prompt.c) reads the low halfword
 * as `D_800EB0F8[0].flags_34`, and password README.md:146-147 calls the word
 * the slot-0 text flags. Both units also matched when the read was spelled
 * `*(u32 *)&D_800EB0F8[0].flags_34` (measured, one build each), so the name
 * is kept for the listings, the notes and the generated symbol lists that
 * carry it, not for codegen. The two units used to declare it `s32 []` and
 * `u32`; the `[]` was the -G8 placement device, which the .data arm does
 * now. */
#ifdef D_800EB12C_IN_DATA
extern u32 D_800EB12C __attribute__((section(".data")));
#else
extern u32 D_800EB12C;
#endif
extern DuelEffectEntry tent_DuelEffectEntries[DUEL_EFFECT_ENTRY_COUNT];

/* The selected card's encoded guardian glyph. DuelField_CreateCardInfoTextBox stores guardian
 * star 1 or 2 plus 0x17 while it prepares the field-card display, and
 * Text_EmitGuardianStarGlyph forwards the byte to the shared text-command renderer.
 * Every access is sb or lbu. Retail addresses the candidate writer through
 * $at, while the matching reader is gp-relative, so the writer selects the
 * .data declaration and the reader takes the plain declaration. */
#ifdef D_8009B344_IN_DATA
extern u8 gText_GuardianGlyphCode __attribute__((section(".data")));
#else
extern u8 gText_GuardianGlyphCode;
#endif

/* Generic text jump selector A; the field-info interpretation below is not
 * universal. Results use multiplayer mode, and the trade builder stores its
 * mode argument. Text_JumpBySelectorA indexes a u16 target table with it.
 *
 * DuelField_CreateCardInfoTextBox clears it on entry, sets 1
 * for an occupied record, 2 or 3 (3 when the record is face-down) for a
 * record in its side's second row (index within the side at or past
 * DUEL_FIELD_ROW_SIZE) and 0 again for a flagged first-row one;
 * func_800610E0 stores its mode argument into it before building a text
 * box; Text_JumpBySelectorA adds twice its value to the word the object's +0x58
 * index selects; duel_result_runtime.c stores the result-screen mode.
 * notes/duel-card-record.md calls it a text-selector override. Values
 * 0..3, every access sb or lbu.
 *
 * Retail reaches it through $at in DuelField_CreateCardInfoTextBox (six stores) with a
 * lui/lbu read in the same function, and gp-relative in Text_JumpBySelectorA, so
 * duel_field_display_objects.c defines the .data arm below;
 * duel_effect_object_commands.c takes the plain byte, and so does
 * func_800610E0.c, whose -G0 unit stores through $at either way. */
#ifdef D_8009B34E_IN_DATA
extern u8 gText_BranchSelectorA __attribute__((section(".data")));
#else
extern u8 gText_BranchSelectorA;
#endif

/* Independent generic selector B. Results use winner + 2 * multiplayer;
 * this is not universally Swords or guardian flags.
 * The field-info constructor sets 1 when the rank it computed
 * is non-zero, or-ed with 2 when gText_BranchSelectorA is non-zero, and read by
 * Text_JumpBySelectorB, which adds twice its value to the word the object's +0x58
 * index selects.
 * Same two units, same forms (four $at stores and one lui/lbu read in
 * DuelField_CreateCardInfoTextBox, gp-relative in Text_JumpBySelectorB), same arms. */
#ifdef D_8009B355_IN_DATA
extern u8 gText_BranchSelectorB __attribute__((section(".data")));
#else
extern u8 gText_BranchSelectorB;
#endif

/* Stored by two functions and read by one. DuelField_CreateCardInfoTextBox stores
 * DuelField_GetGuardianStarDisplayCode's result -- 4, 1 or 6 -- under
 * `if (gText_BranchSelectorA != 0)` in duel_field_display_objects.c;
 * Library_RefreshSelectedCardText stores the returned text channel's
 * color_index_54 byte at +0x54, then 4 when
 * `*(p + (n << 2) + 0x56) & 1` (src/game/library_card_grid.c:246-249);
 * Text_ExpandSelectedCardInfo stores it into `object[0x54]` when the opcode byte has bit
 * 0x10 (src/candidates/func_80037DA4.c). notes/duel-card-record.md:173-176
 * glosses the three results. No unit defines it; the address comes from the
 * generated tmp/splat/undefined_syms_auto.txt, and the nearest named symbol
 * above it is gText_WaitCounter, at +2 (nothing is named at +1 there, in symbols.txt
 * or in c_symbols.ld).
 *
 * u8 because the one function that loads it, Text_ExpandSelectedCardInfo, already declared
 * it u8 and matched, and the load is lbu (func_80037DA4.s:20, gp-relative).
 * Retail stores it through $at in DuelField_CreateCardInfoTextBox (DuelField_CreateCardInfoTextBox.s:162-163)
 * and Library_RefreshSelectedCardText (func_8002A2F4.s:39-40, :48-49), both
 * gcc_2_8_1_g8_split, so duel_field_display_objects.c and library_card_grid.c
 * define the .data arm; Text_ExpandSelectedCardInfo's candidate takes the plain byte.
 * Initial value not read. */
#ifdef D_8009B320_IN_DATA
extern u8 gText_SelectedCardColorIndex __attribute__((section(".data")));
#else
extern u8 gText_SelectedCardColorIndex;
#endif

/* Stored by five C functions and read by one. CardList_CreateSlotTextBox
 * stores the list entry's id (card_list_text_boxes.c:19); DuelField_CreateCardInfoTextBox stores
 * `id = (s16)record->card_id;` under `if (record->flags & 0x8000)`,
 * immediately after `gText_BranchSelectorA = 1;` (duel_field_display_objects.c);
 * DuelEffect_UpdateCardViewerState stores `id = gDuel_wViewerCardID;` (its candidate, :111);
 * Library_RefreshSelectedCardText stores Library_GetGridCursorCardId's result
 * (src/game/library_card_grid.c:232) and then, under `if (n != 0)`, stores
 * 0 when Library_GetCardFlags' result `r` (:238) has `(r & 0x80) == 0`
 * (:240-241);
 * func_80060E70 stores `id` (func_80060E70.c). Text_ExpandSelectedCardInfo reads it,
 * plain and as the index in `gDuel_adwCardStats[gDuel_wSelectedCardID - 1]`
 * (in its candidate). Library_UpdateCardView (src/game/library_runtime.c) stores the
 * Library's selected card and reads it straight back as that index. Three
 * functions still in assembly also store it: DuelScene_UpdateCardMove.s:140-141,
 * DuelScene_UpdateResultRewards.s:202-203 and :235-236,
 * DuelEffect_ApplyRitual assembly:379-380.
 *
 * s16 because Text_ExpandSelectedCardInfo, the only function that loads it, declared it s16
 * when it matched; the five loads are lh (func_80037DA4.s:27/:34/:56/:68/
 * :87). Two bytes at 0x8009B338 (symbols.txt:27), bounded above by
 * gText_CurrentStreamCode at +2.
 *
 * Those five loads are %gp_rel and every C writer's sh goes through $at
 * (CardList_CreateSlotTextBox at 0x80031CD4, DuelField_CreateCardInfoTextBox.s:36-37,
 * DuelEffect_UpdateCardViewerState.s:102-103,
 * func_8002A2F4.s:13-14 and :26-27, func_80060E70.s:61-62), so the five
 * units whose profiles are -G8 at both the compiler and maspsx --
 * duel_field_display_objects.c (DuelField_CreateCardInfoTextBox), func_80060E70.c,
 * library_card_grid.c, library_runtime.c (Library_UpdateCardView) and the
 * DuelEffect_UpdateCardViewerState candidate --
 * define the .data arm, and card_list_text_boxes.c, whose unit assembles
 * at -G0, takes the plain declaration.
 * Initial value not read. */
#ifdef GDUEL_WSELECTEDCARDID_IN_DATA
extern s16 gDuel_wSelectedCardID __attribute__((section(".data")));
#else
extern s16 gDuel_wSelectedCardID;
#endif

/* The text command disposition. TextBox_BuildStep is the only reader: it
 * clears the flag to 0, calls the opcode handler, and then tests the result
 * twice -- `>= 0` and then `== 1`. Everything else only ever writes it, and
 * almost always writes 1. Text_ExtendGlyphCode writes -1 to continue through
 * the glyph path; other nonnegative values continue command parsing.
 *
 * Signed, because that `>= 0` test is only meaningful on a signed value.
 *
 * volatile, because the two tests have no intervening call, so without it
 * GCC folds them onto a single load: dropping the qualifier shrinks the
 * executable by 12 bytes. It is a property of the object, so it belongs on
 * the shared declaration rather than on the one file that happens to read it.
 */
extern volatile s32 gText_CommandDisposition;

/* The shared compaction-request word. DuelEffect_ProcessEntries clears it
 * before invoking entry handlers; DisplayObjectFade_ReleaseChannel raises it.
 * A nonzero request after that walk triggers stable active-entry compaction,
 * not a handler-walk restart or a queued next-frame guarantee.
 * See notes/entry-lifecycle-state.md. */
extern s32 D_8009B330;

/* The CD-sound command's second word, consumed as a phase-two poll budget.
 * A nonzero value arms state bit 0x40; it does not clear a preexisting bit.
 * The wait decrements it only under its original transfer-step bit test
 * and continues for a positive stored result. It never reissues playback.
 * Keep signed halfword narrowing/wrap and the distinct !=0 / >0 tests;
 * the separate byte watchdog remains active even without a new arm. */
extern s16 gText_CdSoundPollBudget;

/* Per-scene state flags. Bit 0x80 is the run-once latch: every scene entry
 * point in this subsystem opens with the same
 * `if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0)` test and
 * sets the bit inside, so the body runs on the first tick of a scene only.
 * dialog_transition.c's handlers additionally raise 0x20 and 0x40,
 * and the byte is cleared back to 0 when a scene is torn down. */
extern u8 D_8009B3C1;

/* Raw legacy work-operation mode, also consumed by the combined dialog
 * opener. MemCardWork_InitializeOperation stores its entire u8 parameter.
 * MemCardWork_UpdateOperation tests bit 0 to select read-header versus
 * write-header work; dialog_transition.c uses that same bit in
 * `DuelEffect_CreateChannel((a & 1) | 0xD0, 0)`. The other seven stored
 * bits have no reader in matched code; this is not a Boolean declaration.
 *
 * Plain, with no .data arm: the low-level card controller, MemCardWork_UpdateOperation,
 * reaches it as `lbu %gp_rel(gMemCardWork_OperationMode)($gp)`, so retail addresses it through
 * $gp like the small-data neighbours here. */
extern u8 gMemCardWork_OperationMode;

/* A 0x1C-byte partial effect prefix, historically merged from
 * DuelEffect_ApplyStatPenalty and DuelEffect_ApplyCursebreaker.
 * Both files described the same layout: the first named the halfword
 * at 0x12 and padded 0x06..0x11, the second padded straight across
 * 0x06..0x13. This retains the merged historical layout.
 * Its main-game consumers now use DuelEffectRequest directly. */
typedef struct {
    u16 x;             /* 0x00 */
    u16 y;             /* 0x02 */
    u16 field_04;      /* 0x04 */
    u8 pad_06[0xC];    /* 0x06 */
    s16 field_12;      /* 0x12 */
    s32 field_14;      /* 0x14 */
    u8 pad_18[2];      /* 0x18 */
    s16 field_1A;      /* 0x1A */
} DuelEffectObject;

typedef char DuelEffectObject_size_must_be_0x1C[
    sizeof(DuelEffectObject) == 0x1C ? 1 : -1
];

#define DUEL_EFFECT_STATE_CARD_VIEWER 2
#define DUEL_EFFECT_STATE_NOOP_3 3
#define DUEL_EFFECT_STATE_NOOP_4 4

#define DUEL_EFFECT_STATE_FLAG_COMPLETE 0x40
#define DUEL_EFFECT_STATE_FLAG_INITIALIZED 0x80
#define DUEL_EFFECT_DIALOG_FLAG_CHOICE_OPEN 0x40
#define DUEL_EFFECT_DIALOG_FLAG_CREATED 0x80

/* Legacy aliases for the dialog request, handler flags, and active handler
 * index. func_800283F4's candidate contract uses D_8009B248; D_8009B244 and
 * D_8009B24A remain layout aliases for other address-oriented consumers. */
extern u16 D_8009B244;
extern u8 D_8009B248;
extern u8 D_8009B24A;

extern u8 gDuel_bEffectHandlerFlags;
extern u8 gDuel_bActiveEffectState;

/* The pending duel-effect request. DuelEffect_UpdateState reads it each
 * tick: zero is idle; otherwise the low bits are the effect id, which it
 * copies into gDuel_bActiveEffectState and then marks with 0x80 as started.
 * State handlers raise 0x40 when the effect is finished, and the next tick
 * clears the byte back to 0. Stored 2 by build_deck_pane_input.c,
 * duel_update_card_pick_cursor.c and the main_menu overlay's
 * MainMenu_UpdateTradeScreen (src/overlays/main_menu/trade_update.c), 3 and
 * 4 by
 * frontend_scene_states.c,
 * and cleared by
 * Main_ResetFrontendRuntime and Main_RunCampaign. One byte, read lbu; the
 * next named byte is gCardGrid_bCursorColumn at 0x8009B258.
 * Retail reaches it through $gp in DuelEffect_UpdateState,
 * DuelEffect_UpdateCardViewerState and DuelEffect_UpdateDialogState, and
 * through %hi/%lo everywhere else, including DuelScene_UpdateFieldActions
 * (still assembly). frontend_scene_states.c, main_run_campaign.c,
 * duel_scene_hand_actions.c and duel_update_card_pick_cursor.c define the
 * .data arm below for that;
 * build_deck_pane_input.c, src/game/duel_effect_states.c,
 * duel_effect_dialog_state.c, main_reset_frontend_runtime.c and
 * MainMenu_UpdateTradeScreen take the plain arm. */
#ifdef GDUEL_BEFFECTSTATE_IN_DATA
extern u8 gDuel_bEffectState __attribute__((section(".data")));
#else
extern u8 gDuel_bEffectState;
#endif

s32 DuelEffect_UpdateState(void);

#endif
