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

/* One text/effect entry, 0x1C bytes, the element type of gDuelEffect_aEntries. The
   leading words are unnamed but must stay four-byte aligned: retail copies a
   whole entry with aligned lw/sw pairs during the compaction in
   DuelEffect_ProcessEntries, and a byte-aligned struct turns that into
   lwl/lwr. The password overlay reads two of the fields: glyph_code is the
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
extern u8 gDuelEffect_aOccupancyValues[DUEL_EFFECT_OCCUPANCY_COUNT];

extern DuelEffectChannel gText_aChannels[DUEL_EFFECT_CHANNEL_COUNT];
#define TEXT_CHANNEL(index) (&gText_aChannels[index])

/* The selected card's encoded guardian glyph. DuelField_CreateCardInfoTextBox stores guardian
 * star 1 or 2 plus 0x17 while it prepares the field-card display, and
 * Text_EmitGuardianStarGlyph forwards the byte to the shared text-command renderer.
 * Every access is sb or lbu. Retail addresses the candidate writer through
 * $at, while the matching reader is gp-relative, so the writer selects the
 * .data declaration and the reader takes the plain declaration. */
extern u8 gText_GuardianGlyphCode;

/* Generic text jump selector A; the field-info interpretation below is not
 * universal. Results use multiplayer mode, and the trade builder stores its
 * mode argument. Text_JumpBySelectorA indexes a u16 target table with it.
 *
 * DuelField_CreateCardInfoTextBox clears it on entry, sets 1
 * for an occupied record, 2 or 3 (3 when the record is face-down) for a
 * record in its side's second row (index within the side at or past
 * DUEL_FIELD_ROW_SIZE) and 0 again for a flagged first-row one;
 * MainMenu_CreateTradeReadyIndicator stores its mode argument into it
 * before building a text box; Text_JumpBySelectorA adds twice its value to the word the object's +0x58
 * index selects; duel_result_runtime.c stores the result-screen mode.
 * notes/duel-card-record.md calls it a text-selector override. Values
 * 0..3, every access sb or lbu. */
extern u8 gText_BranchSelectorA;

/* Independent generic selector B. Results use winner + 2 * multiplayer;
 * this is not universally Swords or guardian flags.
 * The field-info constructor sets 1 when the rank it computed
 * is non-zero, or-ed with 2 when gText_BranchSelectorA is non-zero, and read by
 * Text_JumpBySelectorB, which adds twice its value to the word the object's +0x58
 * index selects.
 * Same two units, same forms (four $at stores and one lui/lbu read in
 * DuelField_CreateCardInfoTextBox, gp-relative in Text_JumpBySelectorB), same arms. */
extern u8 gText_BranchSelectorB;

/* Stored by two functions and read by one. DuelField_CreateCardInfoTextBox stores
 * DuelField_GetGuardianStarDisplayCode's result -- 4, 1 or 6 -- under
 * `if (gText_BranchSelectorA != 0)` in duel_field_display_objects.c;
 * Library_RefreshSelectedCardText stores the returned text channel's
 * color_index_54 byte at +0x54, then 4 when
 * `*(p + (n << 2) + 0x56) & 1` (src/game/library_card_grid.c:246-249);
 * Text_ExpandSelectedCardInfo stores it into `object[0x54]` when the opcode byte has bit
 * 0x10 (src/game/duel_effect_command.c). notes/duel-card-record.md:173-176
 * glosses the three results. No unit defines it; the address comes from the
 * generated tmp/splat/undefined_syms_auto.txt, and the nearest named symbol
 * above it is gText_WaitCounter, at +2 (nothing is named at +1 there, in symbols.txt
 * or in c_symbols.ld).
 *
 * u8 because the one function that loads it, Text_ExpandSelectedCardInfo,
 * already declared it u8 and matched, and the load is lbu
 * (Text_ExpandSelectedCardInfo.s:20, gp-relative). Initial value not read. */
extern u8 gText_SelectedCardColorIndex;

/* Stored by five C functions and read by one. CardList_CreateSlotTextBox
 * stores the list entry's id (build_deck_card_slots.c:45); DuelField_CreateCardInfoTextBox stores
 * `id = (s16)record->card_id;` under `if (record->flags & 0x8000)`,
 * immediately after `gText_BranchSelectorA = 1;` (duel_field_display_objects.c);
 * DuelEffect_UpdateCardViewerState stores `id = gDuel_wViewerCardID;` (its candidate, :111);
 * Library_RefreshSelectedCardText stores Library_GetGridCursorCardId's result
 * (src/game/library_card_grid.c:232) and then, under `if (n != 0)`, stores
 * 0 when Library_GetCardFlags' result `r` (:238) has `(r & 0x80) == 0`
 * (:240-241);
 * CardList_BuildRowSetTextBoxes stores `id` (card_list_deck_box.c). Text_ExpandSelectedCardInfo reads it,
 * plain and as the index in `gDuel_adwCardStats[gDuel_wSelectedCardID - 1]`
 * (in its candidate). Library_UpdateCardView (src/game/library_runtime.c) stores the
 * Library's selected card and reads it straight back as that index. Three
 * functions still in assembly also store it: DuelScene_UpdateCardMove.s:140-141,
 * DuelScene_UpdateResultRewards.s:202-203 and :235-236,
 * DuelEffect_ApplyRitual assembly:379-380.
 *
 * s16 because Text_ExpandSelectedCardInfo, the only function that loads it, declared it s16
 * when it matched; the five loads are lh (Text_ExpandSelectedCardInfo.s:27/:34/:56/:68/
 * :87). Two bytes at 0x8009B338 (symbols.txt:27), bounded above by
 * gText_CurrentStreamCode at +2.
 *
 * Initial value not read. */
extern s16 gDuel_wSelectedCardID;

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

/* The CD-sound command's second word, consumed as a phase-two poll budget.
 * A nonzero value arms state bit 0x40; it does not clear a preexisting bit.
 * The wait decrements it only under its original transfer-step bit test
 * and continues for a positive stored result. It never reissues playback.
 * Keep signed halfword narrowing/wrap and the distinct !=0 / >0 tests;
 * the separate byte watchdog remains active even without a new arm. */
extern s16 gText_CdSoundPollBudget;

/* Per-scene state flags. Bit 0x80 is the run-once latch: every scene entry
 * point in this subsystem opens with the same
 * `if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0)` test and
 * sets the bit inside, so the body runs on the first tick of a scene only.
 * dialog_transition.c's handlers additionally raise 0x20 and 0x40,
 * and the byte is cleared back to 0 when a scene is torn down. */
extern u8 gDialog_bCallbackState;

/* Raw legacy work-operation mode, also consumed by the combined dialog
 * opener. MemCardWork_InitializeOperation stores its entire u8 parameter.
 * MemCardWork_UpdateOperation tests bit 0 to select read-header versus
 * write-header work; dialog_transition.c uses that same bit in
 * `DuelEffect_CreateChannel((a & 1) | 0xD0, 0)`. The other seven stored
 * bits have no reader in matched code; this is not a Boolean declaration. */
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

extern u8 gDuel_bEffectHandlerFlags;
extern u8 gDuel_bActiveEffectState;

/* The pending duel-effect request. DuelEffect_UpdateState reads it each tick:
 * zero is idle; otherwise the low bits are the effect id, which it copies
 * into gDuel_bActiveEffectState and then marks with 0x80 as started. State
 * handlers raise 0x40 when the effect is finished, and the next tick clears
 * the byte back to 0. Stored 2 by build_deck_input.c,
 * duel_update_card_pick_cursor.c and the main_menu overlay's
 * MainMenu_UpdateTradeScreen (src/overlays/main_menu/trade_update.c), 3 and 4
 * by frontend_scene_states.c, and cleared by Main_ResetFrontendRuntime and
 * Main_RunCampaign. One byte, read lbu; the next named byte is
 * gCardGrid_bCursorColumn at 0x8009B258. */
extern u8 gDuel_bEffectState;

s32 DuelEffect_UpdateState(void);

#endif
