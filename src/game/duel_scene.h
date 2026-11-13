#ifndef MEMORIES_DECOMP_DUEL_SCENE_H
#define MEMORIES_DECOMP_DUEL_SCENE_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "ai.h"
#include "duel_card.h"
#include "display_object.h"
#include "display_object_system.h"
#include "duel_grid.h"
#include "card_constants.h"
#include "duel_effect_object_pool.h"
#include "../ygo_types.h"
#include "file_transfer.h"
#include "color_constants.h"
#include "mem_card.h"
#include "file_staging_buffer.h"

/* libgpu.h uses SVECTOR without declaring it, so libgte.h has to come first
 * for this header to be includable on its own. */

struct DisplayObject;

/* One 0x40-byte entry of the array at gCardPreview_Resources. CardPreview_HandleResourceTransfer uploads four
 * textures from a single source origin, and casts each of the four
 * destination rectangles to RECT before handing it to LoadImage, so the
 * layout is four RECTs followed by that origin. */
typedef struct DuelEffectResourceRecord {
    /* CardPreview_CreateResourceObjects acquires both objects from the display-object pool,
     * publishes the allocated-second primary at +0 and the secondary at +4,
     * and links the primary from the secondary. Clear/release helpers keep
     * the same nullable pointer values and order. */
    struct DisplayObject *primary_object;
    struct DisplayObject *secondary_object;
    RECT rects[4];
    u16 src_x;
    u16 src_y;
    /* The VRAM position of the preview's 256-entry CLUT.
       CardPreview_HandleResourceTransfer uploads the 0x100 x 1 palette image
       to rects[1] at (clut_x, clut_y), and CardPreview_CreateResourceObjects
       passes the same pair to DisplayObject_ConfigureScreenSprite as its
       clut_x and clut_y. The duel and Library setups seed it per region. */
    u16 clut_x;
    u16 clut_y;
    /* CardPreview_RequestResourceTransfer stores the card ID here, and the store is
     * `sh $a1, 0x30($v0)` at 80029184, which does not discriminate the sign.
     * u16 follows the `*(u16 *)(record + 0x30)` cast that source carried when
     * the field was named. CardPreview_CreateResourceObjects explicitly takes a signed view for
     * its one-based card index, matching that constructor's lh. */
    u16 card_id;
    /* The constructor supplies ATK/DEF overrides or decoded base stats;
     * CardPreview_DrawDetailPanel sends the corresponding sums to its ATK
     * and DEF digit rows. Retail uses lh at 0x32, 0x34, 0x36 and 0x38,
     * and lbu at 0x3A, 0x3B and 0x3C, through the resource base formed at
     * 80028D88. The default stat path clears both adjustments; the battle
     * Guardian-Star ramp supplies the same additive display amount to each.
     * Level and attribute-icon bytes come from the card-ID-indexed metadata
     * table; icon codes 6/7 also cover non-monsters. Palette bits 0x80/0x40
     * select alternate ATK/DEF rows; no visual meaning is inferred.
     * 0x3D..0x3F remain padding: no recovered access names them. */
    s16 attack;
    s16 defense;
    s16 attack_adjustment;
    s16 defense_adjustment;
    u8 level;
    u8 attribute_icon;
    u8 stat_palette_flags;
    u8 pad_3D[3];
} DuelEffectResourceRecord;

typedef char DuelEffectResourceRecord_size_must_be_0x40[
    sizeof(DuelEffectResourceRecord) == 0x40 ? 1 : -1
];

#define DUEL_EFFECT_RESOURCE_RECORD_OFFSET(member) \
    ((u32)&(((DuelEffectResourceRecord *)0)->member))

typedef char DuelEffectResourceRecord_primary_object_offset_must_be_0x0[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(primary_object) == 0 ? 1 : -1
];
typedef char DuelEffectResourceRecord_secondary_object_offset_must_be_0x4[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(secondary_object) == 4 ? 1 : -1
];
typedef char DuelEffectResourceRecord_rects_offset_must_be_0x8[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(rects) == 8 ? 1 : -1
];
typedef char DuelEffectResourceRecord_card_id_offset_must_be_0x30[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(card_id) == 0x30 ? 1 : -1
];
typedef char DuelEffectResourceRecord_attack_offset_must_be_0x32[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(attack) == 0x32 ? 1 : -1
];
typedef char DuelEffectResourceRecord_defense_offset_must_be_0x34[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(defense) == 0x34 ? 1 : -1
];
typedef char DuelEffectResourceRecord_attack_adjustment_offset_must_be_0x36[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(attack_adjustment) == 0x36 ? 1 : -1
];
typedef char DuelEffectResourceRecord_defense_adjustment_offset_must_be_0x38[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(defense_adjustment) == 0x38 ? 1 : -1
];
typedef char DuelEffectResourceRecord_level_offset_must_be_0x3A[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(level) == 0x3A ? 1 : -1
];
typedef char DuelEffectResourceRecord_attribute_icon_offset_must_be_0x3B[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(attribute_icon) == 0x3B ? 1 : -1
];
typedef char DuelEffectResourceRecord_stat_palette_flags_offset_must_be_0x3C[
    DUEL_EFFECT_RESOURCE_RECORD_OFFSET(stat_palette_flags) == 0x3C ? 1 : -1
];

/* The array itself. Seven sources declared it identically and all seven
 * already include this header, which exists to describe one of its entries,
 * so this is where it belongs.
 *
 * src/overlays/password/shop.c includes it too. It never indexes the array:
 * Password_InitShopScreen parks the first record in gPassword_pCardCache and sets its
 * source rectangle through that pointer, which module_state.h declares by
 * this header's struct tag. */
extern DuelEffectResourceRecord gCardPreview_Resources[];

/* The developer effect-preview screen's state: the index of the operand being
 * edited, the two tuned operands and the page. The operand pair is a byte
 * array, which GCC word-aligns, so the gap after the index is one byte in the
 * North American, Japanese and European builds and three in the four language
 * builds. */
extern u8 gDebugEffect_bEditedOperand;
extern u8 gDebugEffect_abOperands[2];
extern u8 gDebugEffect_bPage;

/* The two card-record indices the battle removes, -1 when unused. The
 * phase entry walks them by presentation cursor; DUEL_BATTLE_REMOVAL_SLOT
 * indexes the pair starting at the first. */
extern s8 gDuel_bBattleRemovalSlot0;
extern s8 gDuel_bBattleRemovalSlot1;

#define DUEL_BATTLE_REMOVAL_SLOT(index) ((&gDuel_bBattleRemovalSlot0)[index])

/* Duel small data that duel_scene.c defines (#2602), for the
 * units that reach it without the render prototypes. */
/* Active duel display-transition request/status flags. Every consumer uses
 * this same scalar view; it is not the separate view-tween frame counter.
 *
 * gDuel_wDisplayTransitionFlags is pinned by its neighbour: gDuel_bQuitDialogState sits
 * two bytes later, so it has no room for an element to carry its own name.
 * gDuel_wSceneStateFlags has moved to its owner, duel_scene.h. */
extern u16 gDuel_wDisplayTransitionFlags;   /* nine declarers  */
/* Three more undefined globals, moved together for the same reason as the
 * batch above: this header is a serialisation point, so separate changes for
 * separate symbols conflict with each other without making any of them
 * easier to review.
 *
 * All three are one-byte state bytes that every consumer spells `extern u8`
 * and reads or writes whole. None is ever indexed, so there is no array
 * hiding behind the scalar and no per-consumer addressing form to preserve.
 *
 * gMemCardDialog_Port is the strongest of the three structurally: c_symbols.ld names
 * gMemCard_wDialogFlags one byte later, so the object is exactly one byte and
 * nothing can carry a second name inside it. It is the memory card slot the
 * MemCard* calls are issued against.
 *
 * gMemCardDialog_OperationState and gDuel_bSceneSubStep have larger gaps to the next name -- two bytes
 * and eight -- but those are upper bounds rather than sizes, like the gap
 * after gDuel_wSceneStateFlags. Nothing is named inside either gap, and no
 * consumer of either reads past the byte, so the u8 all five consumers agree
 * on is what is declared and the bytes above stay unclaimed.
 *
 * Both of the latter two are packed state bytes rather than plain counters,
 * which is why the byte width matters to every reader: gMemCardDialog_OperationState is switched
 * on through `& 0xF` with MEM_CARD_DIALOG_FLAG_* bits set above it, and
 * gDuel_bSceneSubStep carries a step in its low nibble with 0x20, 0x40 and 0x80 used
 * as independent flags. The modal port and internal-state declarations live in
 * game/mem_card.h; the width/addressing evidence above remains historical. */
extern u8 gDuel_bSceneSubStep;   /* five declarers */
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
 * of the declared width, so no element can hide inside them: gMemCardDialog_TransferSize and
 * gMemCardDialog_TransferOffset are two bytes each with a name two bytes on, and gDuel_wStepCounter is
 * two bytes with gDuel_wEffectCardID immediately after it.
 *
 * gMemCardDialog_SyncResult has a larger gap than its width and is treated as an upper
 * bound rather than a size. Nothing is named
 * inside that gap and no consumer reads past the declared width, so the
 * agreed type is what is declared and the bytes above stay unclaimed.
 *
 * The first four are memory card state, shared by the create, load, save and
 * dialog paths together with mem_card_dialog.c. gDuel_wStepCounter is
 * unrelated to them and belongs to the duel side; it is here because it
 * passed the same checks, not because it is part of that group. The modal
 * transfer halfwords and sync result live in game/mem_card.h with the same types. */
extern u16 gDuel_wStepCounter;   /* four declarers */
extern u8 gDuel_bSelectedTrapSlot;
/* Duel_UpdateCardUseConfirmation's step flags: bit 7 once it has created the
 * two flip markers, bit 6 while a left/right flip of gDuel_pActionCard runs.
 * DuelScene_UpdateFieldActions clears the byte before the step starts. */
extern u8 gDuel_bConfirmFlags;
/* The two sprites placed beside the card being flipped, 8 pixels left of it
 * and 0x3C right of it, 0x1E down. The confirmation step and the hand step
 * create them; both release them once the choice is made. */
extern DisplayObject *gDuel_pFlipLeftMarker;
extern DisplayObject *gDuel_pFlipRightMarker;
/* The field-action card: DuelScene_UpdateFieldActions moves it to (0x86, 0x5A)
 * and Duel_UpdateCardUseConfirmation flips it face up or down. */
extern DisplayObject *gDuel_pActionCard;
extern DisplayObject *gDuel_pActionReadyMarker;

struct DisplayObject;

#define DUEL_HAND_SLOT_OFFSET(member) ((u32)&(((DuelHandSlot *)0)->member))

/* One of the HAND_SIZE hand slots at gDuelHand_aSlots.
 *
 * `object` is the card object the draw spawns for the slot; `order_marker` is the
 * sprite DuelHand_CreateSelectionOrderMarker stacks on top of it. Both refer to display-pool slots;
 * consumers retain their context-specific views where needed. `selection_order`
 * is zero for an unmarked slot and holds its one-based order while selecting.
 * Duel_ClearHandSlots zeroes it alongside the two pointers; marker creation
 * publishes the running count, and cancellation renumbers later selections.
 */
typedef struct DuelHandSlot {
    struct DisplayObject *object;
    struct DisplayObject *order_marker;
    u8 pad_08;
    u8 selection_order;
    u8 pad_0A[2];
} DuelHandSlot;

typedef char DuelHandSlot_object_offset_must_be_0x00[
    DUEL_HAND_SLOT_OFFSET(object) == 0x00 ? 1 : -1
];
typedef char DuelHandSlot_size_must_be_0xC[
    sizeof(DuelHandSlot) == 0xC ? 1 : -1
];
typedef char DuelHandSlot_order_marker_offset_must_be_0x04[
    DUEL_HAND_SLOT_OFFSET(order_marker) == 0x04 ? 1 : -1
];
typedef char DuelHandSlot_selection_order_offset_must_be_0x09[
    DUEL_HAND_SLOT_OFFSET(selection_order) == 0x09 ? 1 : -1
];

#undef DUEL_HAND_SLOT_OFFSET

extern DuelHandSlot gDuelHand_aSlots[HAND_SIZE];

/* How many cards are still to be drawn into those slots. duel_scene.c
 * arms it -- 5 for a full opening hand, and HAND_SIZE - n to top an existing
 * hand back up -- and duel_scene.c spends it, taking one off per
 * card as the draw animation retires each one and raising bit 0x4000 in the
 * scene state gDuel_wSceneStateFlags once it reaches zero.
 *
 * Declared u8, which is what both sources say, with the one signed read left
 * spelled as a cast where it is:
 *
 *     if (*(s8 *)&gDuel_bPendingDrawCount == 0)
 *
 * That is the same arrangement duel_effect_text.h records for
 * gDialog_bInputState: the declaration stays unsigned and the cast at the use
 * is what keeps retail's signed load, so resolving one into the other would
 * be a codegen change rather than a tidy-up. */
extern u8 gDuel_bPendingDrawCount;

void Duel_ClearHandSlots(void);

#define DUEL_MONSTER_REMOVAL_CARD_ID_BASE 600
#define DUEL_MONSTER_REMOVAL_RULE_SIZE 2
#define DUEL_MONSTER_REMOVAL_SELECTOR_OFFSET 1
#define DUEL_MONSTER_REMOVAL_RULE_COUNT 7

extern u8 gDuel_abMonsterRemovalRules[];

extern s8 gDuel_bPresentationCursor;

extern s8 gDuel_bPlacementNeedsCardMoveSetup;
extern u8 gDuel_bFieldActionAutoCommit;

#define DUEL_SCENE_PHASE_MASK 0xF
#define DUEL_SCENE_FLAG_INITIALIZED 0x8000

/* Scene phase and update flags. All measured accesses use this halfword;
 * the gap up to gDuel_wEffectDialogTextID (0x8009B244) is not evidence of a wider object. */
extern u16 gDuel_wSceneStateFlags;

#define DUEL_SELECTION_RECORD_SIZE 0x1C
#define DUEL_SELECTION_RECORDS_PER_SIDE 4
#define DUEL_SELECTION_SIDE_SIZE \
    (DUEL_SELECTION_RECORDS_PER_SIDE * DUEL_SELECTION_RECORD_SIZE)

/* One whole selection record, DUEL_SELECTION_RECORD_SIZE bytes, four to a
 * side. Duel_InitSelectionRecords resets the three leading words and bytes
 * at 0x13, 0x14, 0x17 and 0x18. Cursor and mode code supply the other named
 * fields; this is not a claim that initialization clears the whole record.
 *
 * Cursor input/status APIs use this actual record and its real stride. */
typedef struct DuelSelectionRecord {
    /* The record's sliding field-card information panel, stored by the
     * field-display transition; its text box and linked display objects
     * are positioned from it. */
    struct DisplayObject *info_panel;
    /* The cursor sprite DuelScene_UpdateHandActions creates for the side and
     * flags through its halfword at +8; DuelScene_UpdateExodiaResult releases
     * it. */
    struct DisplayObject *cursor_object;
    DuelHandSlot *hand;
    /* Target y of the record's information panel: the field-display
     * transition stores it as the panel's slide-to y; the cursor paths
     * write regional screen constants. */
    s16 panel_target_y;
    /* The hand slot the side's cursor is on: DuelHand_RefreshSelectionColors indexes
     * gDuelHand_aSlots with it to pick the card it leaves undimmed, and
     * DuelScene_UpdateHandActions clears it beside ordered_pick_count. Read signed at
     * that index. */
    u8 hand_cursor_slot;
    /* Signed grid coordinates, also consumed by the input/status APIs. */
    s8 col;
    s8 row;
    /* Allowed rows are [row_min, row_end) through the existing signed views. */
    u8 row_min;
    u8 row_end;
    /* Nonzero: resource variant applied to the information panel, whose
     * text box then moves up. */
    u8 panel_variant;
    /* Text-box channel of the record's information panel. TextBox_Create
     * receives this raw numeric index; only TextBox_SetPos and
     * TextBox_Destroy convert it to &gText_aChannels[index] through
     * DUEL_FIELD_DISPLAY_CHANNEL. Japanese keeps the record index; North
     * America maps index 3 to 1. */
    u8 text_channel;
    /* Number of hand cards marked in pick order on the hand record:
     * DuelHand_CreateSelectionOrderMarker numbers each new mark from it,
     * cancelling a mark decrements it, and zero means a single-card play. */
    u8 ordered_pick_count;
    s8 text_y_offset;
    /* Ordinal within the side's four records; also tags linked objects and
     * selects the field-cursor graphic and information-box mode. */
    u8 record_index;
    /* Nonzero only while DuelScene_UpdateFieldActions browses the field
     * with this record; init, pick, placement and targeting clear it. It
     * selects the field-browse page of gDuel_awFieldGridRowViewTargetZ and,
     * on record 2, the Swords turn count in the information panel. */
    u8 field_actions_mode;
    /* Cursor input/status byte. */
    u8 status;
    u8 pad_1A[2];
} DuelSelectionRecord;

typedef char DuelSelectionRecord_info_panel_offset_must_be_0[
    (u32)&(((DuelSelectionRecord *)0)->info_panel) == 0 ? 1 : -1
];
typedef char DuelSelectionRecord_cursor_object_offset_must_be_4[
    (u32)&(((DuelSelectionRecord *)0)->cursor_object) == 4 ? 1 : -1
];
typedef char DuelSelectionRecord_hand_offset_must_be_8[
    (u32)&(((DuelSelectionRecord *)0)->hand) == 8 ? 1 : -1
];
typedef char DuelSelectionRecord_hand_cursor_slot_offset_must_be_0xE[
    (u32)&(((DuelSelectionRecord *)0)->hand_cursor_slot) == 0xE ? 1 : -1
];
typedef char DuelSelectionRecord_col_offset_must_be_0xF[
    (u32)&(((DuelSelectionRecord *)0)->col) == 0xF ? 1 : -1
];
typedef char DuelSelectionRecord_row_offset_must_be_0x10[
    (u32)&(((DuelSelectionRecord *)0)->row) == 0x10 ? 1 : -1
];
typedef char DuelSelectionRecord_text_y_offset_must_be_0x16[
    (u32)&(((DuelSelectionRecord *)0)->text_y_offset) == 0x16 ? 1 : -1
];
typedef char DuelSelectionRecord_record_index_offset_must_be_0x17[
    (u32)&(((DuelSelectionRecord *)0)->record_index) == 0x17 ? 1 : -1
];
typedef char DuelSelectionRecord_status_offset_must_be_0x19[
    (u32)&(((DuelSelectionRecord *)0)->status) == 0x19 ? 1 : -1
];
typedef char DuelSelectionRecord_ordered_pick_count_offset_must_be_0x15[
    (u32)&(((DuelSelectionRecord *)0)->ordered_pick_count) == 0x15 ? 1 : -1
];
typedef char DuelSelectionRecord_panel_variant_offset_must_be_0x13[
    (u32)&(((DuelSelectionRecord *)0)->panel_variant) == 0x13 ? 1 : -1
];
typedef char DuelSelectionRecord_field_actions_mode_offset_must_be_0x18[
    (u32)&(((DuelSelectionRecord *)0)->field_actions_mode) == 0x18 ? 1 : -1
];
typedef char DuelSelectionRecord_size_must_be_record_size[
    sizeof(DuelSelectionRecord) == DUEL_SELECTION_RECORD_SIZE ? 1 : -1
];

/* Both hand and field controllers keep record 2's 0x38 (and record 3's 0x54)
 * inside their load displacements through this actual four-record side. */
typedef struct {
    DuelSelectionRecord records[DUEL_SELECTION_RECORDS_PER_SIDE];
} DuelSelectionSide;

typedef char DuelSelectionSide_size_must_be_side_size[
    sizeof(DuelSelectionSide) == DUEL_SELECTION_SIDE_SIZE ? 1 : -1
];

/* The selection table itself: DUEL_SELECTION_SIDE_SIZE bytes per side.
 * Callers that start from record N of the active side form
 * gDuel_abSelectionSides + N * DUEL_SELECTION_RECORD_SIZE and add the side
 * stride. */
extern u8 gDuel_abSelectionSides[];

/* Active record in the selection table. Every writer selects a whole
 * DuelSelectionRecord of gDuel_abSelectionSides with the side-stride
 * arithmetic above. Phase entry writes
 * the hand pointer at +8; battle and Exodia paths access cursor_object at +4;
 * the cursor/viewer paths read the shared coordinates and status.
 *
 * Cursor, pick and field-display APIs use this same actual record; no
 * standalone prefix allocation or alternate active-pointer declaration
 * exists. Record-array views retain the measured interior displacements.
 *
 * The global itself is still one four-byte pointer, accessed gp-relative;
 * gDuel_bSelectedTrapSlot follows at +4. No new initial-value or nullability claim is made. */
extern DuelSelectionRecord *gDuel_pActiveSelectionRecord;

/* Resets that table. For both sides it walks the four records, zeroing the
 * first three words and the byte at 0x18, then writing 1 to 0x13, the record
 * index to 0x17, and to 0x14 the index again except for record 3, which gets
 * 1. Afterwards it clears 0x13 on the first record of each side, so the value
 * written in the loop survives only for records 1 to 3.
 *
 * Declared here because this header owns everything it touches: gDuel_abSelectionSides
 * above, DuelSelectionRecord, which the walk now names those fields through,
 * and the three size constants it is written in terms of.
 * Duel_InitScene (src/game/duel_scene.c) is the only caller, and
 * its old duel_scene.c held the only declaration. */
void Duel_InitSelectionRecords(void);

/* Opposing view headings, indexed by duel side. */
extern u16 gDuel_awSideViewHeadings[2];

/* The byte these two report on. They run a cursor update and then hand back
 * the status byte at 0x19, so the return is that byte widened rather than a
 * result of their own.
 *
 * The callers use different checked views of DuelCursor_UpdateDirection --
 * see the note in duel_scene.c. */

/* Direction selected by the cursor path plus the target column and row used
 * by DuelCursor_StepTowardTarget, which compares them as signed bytes. */
extern s8 gDuelCursor_Direction;
extern s8 gDuelCursor_TargetColumn;
extern s8 gDuelCursor_TargetRow;

s32 DuelCursor_UpdateFromInput(DuelSelectionRecord *object);

s32 DuelCursor_UpdateDirection(DuelSelectionRecord *object);
/* Callers that also load the direction into $a1 for the ambient
 * DuelCursor_ProcessGridDirection argument call through this type. */
typedef s32 (*DuelCursorUpdateDirectionWithDir)(DuelSelectionRecord *cursor, s8 dir);

#define DUEL_CURSOR_MOTION_OFFSET(member) \
    ((u32)&(((DisplayObject *)0)->member))

typedef char DuelCursorMotion_position_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(position.h.field_28) == 0x28 &&
    DUEL_CURSOR_MOTION_OFFSET(position.h.field_2A) == 0x2A ? 1 : -1
];
typedef char DuelCursorMotion_target_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(field_2C.h.field_2C) == 0x2C &&
    DUEL_CURSOR_MOTION_OFFSET(field_2C.h.field_2E) == 0x2E ? 1 : -1
];
typedef char DuelCursorMotion_step_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(field_34.h.field_36) == 0x36 &&
    DUEL_CURSOR_MOTION_OFFSET(field_38.h.field_3A) == 0x3A ? 1 : -1
];
typedef char DuelCursorMotion_control_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(field_60) == 0x60 &&
    DUEL_CURSOR_MOTION_OFFSET(field_6C) == 0x6C ? 1 : -1
];

#undef DUEL_CURSOR_MOTION_OFFSET

#define DUEL_CURSOR_RECORD_OFFSET(member) \
    ((u32)&(((DuelSelectionRecord *)0)->member))

typedef char DuelCursor_record_offsets_must_match[
    DUEL_CURSOR_RECORD_OFFSET(cursor_object) == 0x04 &&
    DUEL_CURSOR_RECORD_OFFSET(col) == 0x0F &&
    DUEL_CURSOR_RECORD_OFFSET(row) == 0x10 &&
    DUEL_CURSOR_RECORD_OFFSET(row_min) == 0x11 &&
    DUEL_CURSOR_RECORD_OFFSET(row_end) == 0x12 &&
    DUEL_CURSOR_RECORD_OFFSET(field_actions_mode) == 0x18 &&
    DUEL_CURSOR_RECORD_OFFSET(status) == 0x19 ? 1 : -1
];

#undef DUEL_CURSOR_RECORD_OFFSET

/* One step of the duel field cursor. `dir` is the direction index
 * DuelCursor_ProcessHeldGridDirection priority-encodes out of the held pad bits: negative for no
 * direction, bit 0 picking the axis and bit 1 the sign. A set 0x80 flag means
 * a move is still settling, and that arm ignores `dir` entirely.
 *
 * DuelCursor_UpdateDirection reaches it with the object alone. Its prologue
 * assigns only $a0, so `dir` arrives as whatever that function's own caller
 * left in $a1, and there is no expression duel_scene.c could write for
 * it -- the call site does not compute one. The arm below preserves that
 * one-argument call through DuelCursorProcessGridDirectionAmbient, while
 * DuelCursor_ProcessHeldGridDirection, which does compute the argument, uses
 * the real prototype. */
void DuelCursor_ProcessGridDirection(DuelSelectionRecord *o, s32 dir);
typedef void (*DuelCursorProcessGridDirectionAmbient)(DuelSelectionRecord *o);

/* The duel scene's phase callback table at 0x80090998.
 *
 * Deliberately left unsized. DuelScene_Update reaches it through
 * `gDuel_apfnSceneStateHandler[gDuel_wSceneStateFlags &
 * DUEL_SCENE_PHASE_MASK]`, and the mask permits an index the fifteen
 * defined entries do not cover, so a bound here would assert something the
 * call site does not. */
extern void (*gDuel_apfnSceneStateHandler[])(void);

/* Card-move presentation, position choice, and deferred stat adjustment. */
void DuelScene_UpdateCardMove(void);

/* Interior alias of gDuel_aActiveCards[AI_SLOT_OWN_MONSTER_FIRST], not a
 * separate allocation. Retain the alias as the refresh routine's base. */
extern AiActiveCard gAiSnapshot_aOwnCards[];

void Ai_ExportSideSnapshot(AiActiveCard *out, s32 side);
void Ai_RefreshBoardSnapshot(void);

int Ai_SelectMagicPlay(void);
s32 Ai_SelectFusionPlay(void);
s32 Ai_SelectHandPlay(void);

/* First positive comparison in grid order, not a maximum search. If no
 * occupied card exists, return the row's middle grid entry; otherwise -1. */
int Ai_FindFavorableFieldTarget(DuelCardRecord *source);
s32 Ai_FindFavorableFaceUpFieldTarget(DuelCardRecord *source);

s32 Ai_SelectFieldPlay(void);

void DebugEffect_UpdateViewControls(void);
void DebugEffect_UpdatePreview(void);

extern u8 *gDebugEffect_pPreviewCard0;
extern u8 *gDebugEffect_pPreviewCard1;
extern DuelCardRecord gDebugEffect_aPreviewCardRecords[];
/* DuelScene_UpdateEffectPreview's FntPrint strings stay in generated
 * .rodata: retail keeps them, after gDuel_szComSelCardError, ahead of every
 * duel_scene jump table, while GCC would emit them as literals after the
 * tables of earlier functions (Duel_LoadPackageStage's is the first). */
extern u8 gDebugEffect_szOperandsFormat[];
extern u8 gDebugEffect_szSecondOperandMarker[];
extern u8 gDebugEffect_szFirstOperandMarker[];

/* The developer effect-preview screen. DuelScene_UpdateEffectPreview is the gDuel_apfnSceneStateHandler duel
 * phase entry for it: it runs the preview builder and then prints the
 * "EFFECT = %2d %2d" line with one of two divider strings, chosen by the
 * edited-operand index gDebugEffect_bEditedOperand.
 *
 * The other two functions this unit defines are reached only from within it. */
void DuelScene_UpdateEffectPreview(void);

void Duel_UpdateCardMarkerRemovalTurn(DisplayObject *object);
void DisplayObject_UpdateCosineSlideWithRollReset(DisplayObject *object);

/* Callers that install this as a DisplayObjectCallback (u8 *) cast it; the
 * callback receives the same display-record address. */
void DisplayObject_UpdateCosineSlide(DisplayObject *object);

/* These are installed in the update slot by the field-action controller,
 * not called directly. Each clears the callback and active latch on completion. */
/* Steps three colour bytes toward the halfword targets by field_60. */
void DisplayObject_UpdateColorTransition(DisplayObject *object);
/* Cosine-interpolates the saved position and advances rotation over the
 * configured duration, then snaps both to their targets. */
void DisplayObject_UpdatePositionRotationTransition(DisplayObject *o);

/* Facing-result callback installed by the card and icon factories at +0x10.
   DisplayObject_ProjectRotationFacing supplies the projected-area word.
   It takes the full u32 bits and stores only their sign into the object's
   resource_variant. */
void DisplayObject_SetResourceVariantFromSign(
    DisplayObject *object,
    u32 value
);

/* Sets field_21 to start, stores target at 0x28 and the signed step at 0x2A,
   sets field_6C and the clip-test flag, and installs
   DisplayObject_UpdateAlphaTransition as the update callback. */
void DisplayObject_StartAlphaTransition(
    DisplayObject *object, u8 start, s16 target, s16 step
);

void DisplayObject_UpdateAlphaTransition(DisplayObject *object);

void DuelSelection_LinkDisplayObject(
    DuelSelectionRecord *parent,
    volatile DisplayObject *object
);
void DuelSelection_LinkDisplayObjects(DuelSelectionRecord *parent, s32 clear);

/* The card-effect flag word. duel_scene.c owns the initialization bit:
 * DuelEffect_MarkInitialized sets bit 0x80 the first time it is asked and
 * DuelEffect_ClearCardEffect clears the whole word.
 * DuelEffect_StartCardEffect starts an effect by writing
 * DUEL_CARD_EFFECT_FLAG_ACTIVE, or ACTIVE | SECOND_HANDLER
 * (card_constants.h). Effect handlers set and test the bits below 0x80 as
 * they step. Retail reaches it gp-relative at every site, 23 lhu and 34 sh,
 * so u16 is the width and sign the accesses give it. */
extern u16 gDuel_wCardEffectFlags;

/* The two values DuelEffect_StartCardEffect stores before the flag word.
 * gDuel_wEffectCardID is its first parameter, stored unmodified once it falls
 * in one of the two DUEL_EFFECT_*_CARD_ID blocks or equals
 * DUEL_DARK_MAGIC_RITUAL_CARD_ID. The handlers compare it against
 * DUEL_DRAGON_CAPTURE_JAR_CARD_ID and DUEL_SPELLBINDING_CIRCLE_CARD_ID,
 * subtract DUEL_DIRECT_DAMAGE_FIRST_CARD_ID from it, and pass it to
 * Duel_CheckRitual as ritualId. gDuel_sCardEffectIndex is the normalized
 * table index consumed by gDuelEffect_abGroupByEffectId.
 *
 * Retail reads gDuel_wEffectCardID with lh at seven sites and
 * gDuel_sCardEffectIndex with lh at one, all gp-relative; the one lhu of the
 * card ID is in the matching DuelEffect_ApplyRitual source.
 * DuelEffect_ApplyTerrain reads its low byte explicitly, which is retail's
 * lbu. */
extern s16 gDuel_sCardEffectIndex;
extern s16 gDuel_wEffectCardID;

/* Two adjacent halfwords in the North American and Japanese small data,
 * separate variables in PAL (whose action substate sits elsewhere).
 *
 * gDuel_wActionSubstate is the packed action substate of
 * DuelScene_UpdateHandActions and DuelScene_UpdateFieldActions.
 * gDuel_sEffectWorkCounter is the field-sweep and countdown counter: retail
 * reaches it gp-relative at 23 sites (7 lh, 5 lhu, 11 sh, three in
 * DuelEffect_ApplyRitual); the lhu is where the source writes
 * `*(u16 *)&gDuel_sEffectWorkCounter` for the +1 or -1. DuelEffect_ApplyStatPenalty
 * runs it down from 0x10; the other three reset it to 0 or -1, advance it and
 * index gDuel_abFieldGridCardRecordIndices by it. Both were once one
 * `s16 [2]` array plus linker aliases; the scalars build identically. */
extern u16 gDuel_wActionSubstate;
extern s16 gDuel_sEffectWorkCounter;

/* The halfword immediately past the counter.
 *
 * It is the trap presentation's mode: Duel_UpdateTrapPresentation in
 * duel_scene.c sequences on `gDuel_wTrapPresentationMode & 0xF` through four modes
 * and stores 1, 2 and 3 itself, and duel_scene.c clears it to 0 at the
 * two points the effect finishes. Four further writers are still assembly,
 * all `sh $zero`. */
extern u16 gDuel_wTrapPresentationMode;

s32 DuelEffect_MarkInitialized(void);
void DuelEffect_ClearCardEffect(void);

/* Returns the cached action-ready marker, or creates one linked to source.
 * The caller checks eligibility and publishes the result to gDuel_pActionReadyMarker.
 * A cached object is returned unchanged, without rebinding its source. */
DisplayObject *Duel_GetOrCreateActionReadyMarker(DisplayObject *source);

/* Returns the left card's position-selected, guardian-adjusted battle stat
 * minus the right card's position-selected effective stat. Equal stats yield
 * 0 if either card defends, otherwise -1; negative values are not normalized.
 * With `right` null it returns `left`'s effective attack instead.
 *
 * Both operands are duel card display objects, and the only offset either is
 * reached at is 0x6A -- the gDuel_aCardRecords index. That is the same use
 * Duel_DrawCardFrame makes of the byte through this record's field_6A, and the
 * evidence for calling these DisplayObject rather than something narrower.
 *
 * The field selectors now take canonical DuelCardRecord pointers and use
 * this same DisplayObject view, rather than separate one-pointer and
 * 0x6B-byte prefix records. DuelScene_UpdateBattle also consumes the result
 * as a damage magnitude, tie marker, or direct-attack value. */
s32 Duel_CompareBattleStrength(DisplayObject *left, DisplayObject *right);

typedef struct DuelFieldEffectObject DuelFieldEffectObject;

/* The display/effect object shared by the field-wide transition handlers. */
struct DuelFieldEffectObject {
    s16 x;
    s16 y;
    u8 pad_04[0x16];
    s16 field_1A;
    u8 flags;
    u8 count;
    u8 pad_1E[4];
    u8 timer;
    u8 pad_23;
    void (*callback)(DuelFieldEffectObject *);
    u8 pad_28[0x3F];
    u8 mark;
    u8 pad_68[2];
    u8 index;
    u8 pad_6B;
    u8 active;
};

#define DUEL_FIELD_EFFECT_OBJECT_OFFSET(member) \
    ((u32)&(((DuelFieldEffectObject *)0)->member))

typedef char DuelFieldEffectObject_size_must_be_0x70[
    sizeof(DuelFieldEffectObject) == 0x70 ? 1 : -1
];
typedef char DuelFieldEffectObject_callback_offset_must_be_0x24[
    DUEL_FIELD_EFFECT_OBJECT_OFFSET(callback) == 0x24 ? 1 : -1
];
typedef char DuelFieldEffectObject_mark_offset_must_be_0x67[
    DUEL_FIELD_EFFECT_OBJECT_OFFSET(mark) == 0x67 ? 1 : -1
];
typedef char DuelFieldEffectObject_active_offset_must_be_0x6C[
    DUEL_FIELD_EFFECT_OBJECT_OFFSET(active) == 0x6C ? 1 : -1
];

#undef DUEL_FIELD_EFFECT_OBJECT_OFFSET

/* Adjacent application handlers from the magic-effect group table plus the
 * per-card reveal callback. They share the gDuel_aCardRecords field walk,
 * gDuel_wCardEffectFlags phase state, effect-object requests and completion gates. */
extern s16 gDuel_wMonsterRemovalCriterion;
extern s16 gDuel_wMonsterRemovalZone;

void DuelEffect_ApplyLifePointRecovery(void);
void DuelEffect_ApplyDirectDamage(void);
void DuelEffect_ApplyMonsterRemoval(void);
void DuelEffect_ApplyBoardDestruction(void);
void DuelEffect_ApplyStopDefense(void);
void DuelEffect_ApplyRaigeki(void);
void DuelEffect_UpdateRevealCard(DuelFieldEffectObject *object);
void DuelEffect_ApplyDarkPiercingLight(void);
void DuelEffect_ApplyStatPenalty(void);

/* The two 0x20-byte per-side duel records at gDuel_aSideStates, one per duellist.
 * gDuel_bActiveSide selects the side, and code that switches turns writes
 * gDuel_pActiveSideState = (u8 *)gDuel_aSideStates + gDuel_bActiveSide * 0x20, so gDuel_pActiveSideState always
 * points at one of these two records.
 */

/* Offset 0x14 is read as unsigned by the life-point code, which clamps it at
 * zero and against the maximum, and as signed by the rank scorer, which
 * copies it into a signed slot. Both loads exist in the retail image, so the
 * field is offered under both spellings rather than forcing a cast at one of
 * them.
 */
union DuelSideLifePoints {
    u16 unsigned_value;
    s16 signed_value;
};

/* The rank statistics stored at the front of each side record. The named
 * bytes are the counters consumed by Duel_CalcRankScore; face_down_plays
 * counts normal single-card face-down commitments, not fusion results.
 * combo_plays, card_destructions and defensive_losses are shown on the
 * result screen (COMBO PLAYS, CARD DESTRUCTION, DEFENSIVE LOSSES) but not
 * scored: plays committing more than one card, attacker wins over a
 * monster, and the defender's defence-position monsters beaten. */
typedef struct {
    s8 result_adjustment;
    u8 turns_taken;
    u8 effective_attacks;
    u8 defensive_wins;
    u8 face_down_plays;
    u8 pure_magic_used;
    u8 traps_triggered;
    u8 combo_plays;
    u8 fusions_initiated;
    u8 equips_used;
    /* Field magic cards played: DuelEffect_ApplyTerrain counts each one. */
    u8 field_magic_used;
    u8 card_destructions;
    u8 defensive_losses;
} DuelRankStatistics;

typedef struct DuelSideState {
    DuelRankStatistics rank;
    u8 field_0D;
    /* Mean ATK and DEF of the side's 40-card deck, computed by duel phase
     * entry and shown on the result screen; not part of the rank score. */
    s16 deck_average_attack;
    s16 deck_average_defense;
    /* The life-point value actually drawn. Duel_UpdateLifePointDisplay steps
     * it towards life_points a little each frame, and
     * Duel_DrawLifePointsAndDeckCounts draws this field, not life_points, as
     * four digits. */
    s16 displayed_life_points;
    union DuelSideLifePoints life_points;
    s16 max_life_points;
    /* Index of the next deck card to draw. The result screen labels this
     * statistic "cards used", but refill draws advance it before play. */
    s8 deck_draw_cursor;
    s8 swords_turns_remaining;
    /* duel_scene.c's own view of this record calls +0x1A
     * hand[HAND_SIZE] and Duel_HasAllExodiaPieces copies five entries out of
     * it, so the six bytes are five hand slots and one separate byte. */
    s8 hand[HAND_SIZE];
    /* Per-side card presentation mode copied from the two-player setup
     * option; positive and negative values select distinct card markers. */
    s8 card_view_mode;
} DuelSideState;

#define DUEL_SIDE_STATE_OFFSET(member) ((u32)&(((DuelSideState *)0)->member))

typedef char DuelSideState_size_must_be_0x20[
    sizeof(DuelSideState) == 0x20 ? 1 : -1
];
typedef char DuelSideState_hand_must_be_at_0x1A[
    DUEL_SIDE_STATE_OFFSET(hand) == 0x1A ? 1 : -1
];
typedef char DuelRankStatistics_size_must_be_0x0D[
    sizeof(DuelRankStatistics) == 0x0D ? 1 : -1
];
typedef char DuelSideState_deck_draw_cursor_must_be_at_0x18[
    DUEL_SIDE_STATE_OFFSET(deck_draw_cursor) == 0x18 ? 1 : -1
];
typedef char DuelSideState_swords_turns_remaining_must_be_at_0x19[
    DUEL_SIDE_STATE_OFFSET(swords_turns_remaining) == 0x19 ? 1 : -1
];
typedef char DuelSideState_card_view_mode_must_be_at_0x1F[
    DUEL_SIDE_STATE_OFFSET(card_view_mode) == 0x1F ? 1 : -1
];

/* The side selector: 0 or 1, and the index behind both cursors this header
 * and duel_grid.h describe. Thirty-nine private declarations before this. */
extern u8 gDuel_bActiveSide;

/* A volatile view of the same byte. Swords of Revealing Light and
 * Cursebreaker (duel_scene.c) reload it at every use, while the rest
 * of that unit reads the plain declaration above. */
#define DUEL_SIDE_SELECTOR_VOLATILE (*(volatile u8 *)&gDuel_bActiveSide)

/* Destination slot and signed stat adjustment used by the card-move
 * presentation sequence (DuelScene_UpdateCardMove). The adjustment is the
 * pending equip bonus: DuelScene_UpdateCardPlacement adds 500 per equip
 * (a second 500 for card 0x291) and clears it for a fusion. Both are reached
 * through small data. */
extern u8 gDuel_bCardMoveSlot;
extern s16 gDuel_wPendingEquipBonus;

extern DuelSideState gDuel_aSideStates[DUEL_SIDE_COUNT];
/* Always &gDuel_aSideStates[gDuel_bActiveSide]: four translation units assign it exactly
 * that on a turn change. */
extern DuelSideState *gDuel_pActiveSideState;

/* Per-side request-pool entries for Swords of Revealing Light. Apply and
 * replay reconstruction publish NONBLOCKING effect 0x15 requests; draw entry
 * writes the current side's removal command before clearing its pointer. */
extern DuelEffectRequest
    *gDuel_apSwordsEffectObjects[DUEL_SIDE_COUNT];

/* The flag the field-action step raises for the battle step: set when Square
 * (PAD_BUTTON_SQUARE) is among the buttons that commit the field-action confirm,
 * cleared otherwise and by two battle-step paths, one of them a selected
 * attack trap (Duel_SelectAttackTrap). When it is set, the
 * battle step moves to sub-step 5, which fades out and enters the animated
 * battle main mode, instead of sub-step 7.
 * DuelScene_UpdateFieldActions stores 0 twice and 1 once
 * (src/game/duel_scene.c), DuelScene_UpdateBattle stores 0 twice and
 * is the only reader, testing it against 0
 * (src/game/duel_scene.c). That single load is lbu and the five
 * stores are sb and cannot say, so the byte is unsigned -- which settles a
 * disagreement the two units carried, one declaring it u8 and the other s8
 * while every use is a store of 0 or 1 or a test against 0, so neither
 * spelling showed in the object code. No other C source in this tree mentions
 * the symbol. Every access in both listings is gp-relative, so this is the
 * plain declaration. Initial value not read. */
extern u8 gDuel_bAnimatedBattleRequested;

/* The card id the last search or trap selection left behind.
 * Duel_SelectTrapByCardId stores 0 before its slot loop and its argument on a
 * hit, and its own comment says what a hit and a miss leave;
 * Duel_SelectAttackTrap stores `sel + DUEL_ATTACK_TRAP_FIRST_CARD_ID` in one
 * arm and `v` under its `hit:` label;
 * DuelEffect_ApplyLifePointRecovery and DuelEffect_ApplyDirectDamage test it
 * against 0. DuelScene_UpdateBattle (src/game/duel_scene.c) stores 0
 * once and loads it eight times. Every retail load is lh
 * (DuelEffect_ApplyLifePointRecovery, DuelEffect_ApplyDirectDamage, and the
 * eight in DuelScene_UpdateBattle), so the halfword is signed; the stores
 * are sh and cannot
 * say. Every access in all five listings is gp-relative, so this is the
 * plain declaration for duel_scene.c, duel_scene.c and
 * duel_scene.c alike, and the `u16` one of them used to write was the same
 * halfword under the other sign. Initial value not read. */
extern s16 gDuel_wSelectedTrapCardId;

/* Assigned in two units and read by no C statement: Duel_InitScene writes
 * `gDuel_pActiveFieldGrid = &gDuel_abFieldGridCardRecordIndices[gDuel_bActiveSide * 20];`
 * (src/game/duel_scene.c:119-120) and
 * DuelScene_UpdateTurnSwitch writes `gDuel_pActiveFieldGrid = gDuel_abFieldGridCardRecordIndices + gDuel_bActiveSide *
 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;` (duel_scene.c); no other listing
 * mentions the symbol. u8 * because gDuel_abFieldGridCardRecordIndices is `extern u8 gDuel_abFieldGridCardRecordIndices[]`
 * under the arm both units take (duel_grid.h:63) and 20 is
 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT (duel_grid.h:9); DuelScene_UpdateTurnSwitch's source used to
 * write `void *` for the same address, and no prototype takes &gDuel_pActiveFieldGrid.
 * Both stores are gp-relative sw (Duel_InitScene.s:128, DuelScene_UpdateTurnSwitch.s:46),
 * so this is the plain declaration. Initial value not read. */
extern u8 *gDuel_pActiveFieldGrid;

/* The byte Main_RunTwoPlayerDuelSetup passes as `&gDuel_bTwoPlayerCardViewMode` to
 * MainMenu_StartValueSetup's `u8 *toggle` parameter, beside the two halfwords
 * below; Duel_InitSideStates, when both gDuel_bSide0ControllerSelector[0] and
 * gDuel_bOpponentID are negative, copies it into card_view_mode of both
 * gDuel_aSideStates records; Main_Init stores 1 into it. Every retail access is
 * a byte or an address (lbu Duel_InitSideStates.s:65, sb Main_Init.s:39,
 * the lui/addiu pair at 0x8002DC60), and the PAL images place it at an odd
 * address (0x8009C0F1 in SLES-03947), so it is a u8. The following halfword is
 * separate storage. Initial value not read. */
extern u8 gDuel_bTwoPlayerCardViewMode;

/* The two halfwords Main_RunTwoPlayerDuelSetup stores DUEL_STARTING_LIFE_POINTS into
 * (gDuel_wTwoPlayerSide1LifePoints first, then gDuel_wTwoPlayerSide0LifePoints) and passes to MainMenu_StartValueSetup
 * as `first` and `second`, both declared `u16 *` in value_setup.h -- that
 * signature is
 * what fixes the type, and Duel_InitSideStates's lhu of each
 * (Duel_InitSideStates.s:11-12) agrees. Duel_InitSideStates copies them into `sp[0]`
 * and `sp[1]` of its `u16 sp[DUEL_SIDE_COUNT]` when gDuel_bOpponentID is
 * negative. No other C
 * unit touches them. Initial value not read. */
extern u16 gDuel_wTwoPlayerSide0LifePoints;
extern u16 gDuel_wTwoPlayerSide1LifePoints;

/* The signed byte just below gDuel_bOpponentID, 0x8009B360, is declared once,
 * as gDuel_bSide0ControllerSelector in ai_controller_handoff.h. */

/* Which side won, and therefore which of the two records above the result
 * code applies to. duel_scene.c sets it as `gDuel_bActiveSide ^ 1`, the
 * side that is not the one the selector documented above points at, and both
 * it and duel_scene.c immediately use it to index gDuel_aSideStates. That is what
 * puts it in this header rather than beside the screen that displays the
 * outcome: it is a side selector, and it is read as one. */
extern u8 gDuel_bWinnerSide;

/* The outro's own copy of the winning side, and the only one of these that
 * carries a "not set" state. Duel_InitScene clears it to -1 when the duel
 * starts, DuelScene_UpdateResultOutro writes the winner alongside gDuel_bPlayerLost when the
 * result sequence begins, and duel_scene.c reads it as an override:
 * it takes gDuel_bActiveSide and replaces it with this value only when the test
 * `gDuel_bOutroWinnerSide >= 0` passes. That signed test is why it is s8 and not u8 --
 * the sentinel is the whole point of the field. */
extern s8 gDuel_bOutroWinnerSide;

/* The duel's outcome as a plain 0/1 byte. DuelScene_UpdateResultOutro sets it
 * in the same statement group as gDuel_bOutroWinnerSide: 0 when
 * gDuel_bWinnerSide is 0, 1 otherwise. Main_RunDuel indexes its two campaign
 * continuations with it (`table[gDuel_bPlayerLost * 2]`), and the free_duel
 * overlay's FreeDuel_Init steps from the wins halfword to the losses halfword
 * of the duelist record when it is 1. The `u8 [9]`
 * Main_RunDuel (main_mode_runners.c) used to declare was, as the note on
 * gDuel_bSide0ControllerSelector in ai_controller_handoff.h says, a
 * threshold, not a length. */
extern u8 gDuel_bPlayerLost;

/* A byte the duel setup clears and Duel_InitScene tests against 1.
 * Main_StartDuel stores 0 beside gDuel_bOpponentID, gDuel_awCampaignSceneByOutcome[0], gDuel_awCampaignSceneByOutcome[1]
 * and gDuel_bTerrain; Text_StartCampaignDuel stores 0 after its own copy
 * of that setup; matching DuelScene_UpdateExodiaResult stores 1;
 * DuelScene_UpdateBattle stores it too. Duel_InitScene skips two
 * blocks when it is 1, and Main_RunDuel clears gMain_bModeRunnerStep only when it is
 * 0 and gDuel_bOpponentID is not negative. Every retail access is sb or
 * lbu and every declarer said u8. Initial value not read. */
/* Declared once, as gDuel_bResumeFromAnimatedBattle, in duel_session_controls.h. */

/* The halfword the duel hands to SD_BGMPlay: DuelScene_UpdateResume and
 * DuelScene_UpdateStartup read it for that call, Main_StartDuel stores
 * 0x7270, DebugMenu_EnterDuel stores 0x71D0 and Text_StartCampaignDuel stores
 * TextStream_ReadU16LE's result. lhu/sh everywhere, two bytes wide
 * (gFreeDuel_bTargetColumn is at 0x8009B36C). */
extern u16 gDuel_wBgmId;

/* Clears the per-duel state -- roughly thirty scalars, several of them
 * written more than once -- and then, as its last statement, assigns
 * `gDuel_pActiveSideState = &gDuel_aSideStates[gDuel_bActiveSide];` (duel_scene.c:62;
 * Duel_InitSideStates.s:85). It does not touch gDuel_pActiveFieldGrid: that store is in its
 * only caller, src/game/duel_scene.c:119, after the call at :95.
 *
 * Declared here because that last statement is the assignment this header
 * already describes: the note on gDuel_pActiveSideState says four translation units
 * assign it exactly `&gDuel_aSideStates[gDuel_bActiveSide]` on a turn change, and this is
 * one of them. Duel_InitScene (src/game/duel_scene.c) is the only
 * caller, and its old duel_scene.c had the only declaration. */
void Duel_InitSideStates(void);

void Duel_DrawDecimalDigits(
    DisplayObject *style, GsSPRITE *digit, s32 value, s32 count
);

void Duel_UpdateLifePointDisplay(DuelSideState *side);

void Duel_DrawLifePointsAndDeckCounts(DisplayObject *widget);

/* True only for an occupied duel-card record that is not marked used this
 * turn: the body tests DUEL_CARD_FLAG_OCCUPIED and then
 * DUEL_CARD_FLAG_USED_THIS_TURN on the record's flags
 * (duel_scene.c).
 *
 * DuelScene_UpdateFieldActions (src/game/duel_scene.c) is the only
 * source outside the unit that names it, and it declared the prototype for
 * itself because the defining source had no header. The spelling here is the
 * definition's own, int return included. */
int DuelCard_CanActThisTurn(DuelCardRecord *object);

void DuelCard_CaptureRoundedTexture(DisplayObject *object);

/* The two table lookups that answer "may these two cards combine?". Both take
 * card ids and both return 0 for no, so a caller can test them as a
 * condition -- and every one of the five does.
 *
 * What a prototype cannot say is what the non-zero value is, and the two
 * differ. Duel_CheckEquip hands back `equipment` unchanged, so its result
 * carries no information the caller did not already have; ai_script.c and
 * duel_scene.c use it as a plain predicate. Duel_CheckFusion
 * returns the id of the card the pair fuses INTO, which ai_script.c reads as a
 * value and duel_scene.c only tests.
 *
 * The tables are also asymmetric in opposite ways. gDuel_awEquipTable is
 * keyed on the equipment card and searched for the monster, so the argument
 * order matters. Duel_CheckFusion sorts its pair first, so it does not. */
/* The two tables those lookups read, which the comment above already names.
 * Duel_CheckEquip walks gDuel_awEquipTable as u16 pairs; Duel_CheckFusion
 * takes gDuel_aFusionTable's base as a u8 * and strides it itself.
 *
 * Neither is defined in C. Duel_LoadPackageStage
 * (src/game/duel_scene.c) fills both from the disc, handing their
 * addresses to a transfer descriptor as
 * `d->sector_destination = d->alternate_sector_destination = (u32)gDuel_awEquipTable`, which is why that
 * source declared them too and now includes this header instead.
 *
 * Both keep the incomplete-array spelling both consumers already used; the
 * loader is the only thing that knows how long they are and it learns that
 * from the package. */
extern u16 gDuel_awEquipTable[];
extern u16 gDuel_aFusionTable[];

s32 Duel_CheckEquip(s32 equipment, s32 monster);
s32 Duel_CheckFusion(s32 card_a, s32 card_b);

void Duel_StepCardDataTransfer(FileTransferDescriptor *, s32);

/* Rotates a displayed duel card between attack and defense orientation while
   keeping its DuelCardRecord defense-position flag synchronized. */
void DuelCard_UpdateDefenseRotation(DisplayObject *object);

#define DUEL_CARD_DISPLAY_FACE_DOWN_VALUE 0x80
#define DUEL_CARD_DISPLAY_DEFENSE_VALUE 0xC0

/* The fields these helpers touch on a duel card's display object. This is not
 * the whole display record: the callers hold wider views of the same memory
 * under their own names. */
/* Every field below is one Duel_CreateCardDisplayObject, Duel_UpdateCardDisplayMarker, Duel_UpdateCardDisplayState or
   Duel_CreateHandCardDisplayObject writes or reads on the object DisplayObject_AcquireSlot hands back. The
   offsets are unchanged; the fields added for Duel_CreateCardDisplayObject replace padding
   this struct already carried, so no existing user's codegen can move.

   Names are taken from records that already name the offset rather than
   invented here: attribute at 0x04 from the canonical DisplayObject in
   display_object.h, and out_x/out_y at 0x30/0x32 from DisplayObjectPosition
   in display_object_interpolation.h, where the interpolation helpers leave
   their eased result. Duel_CreateCardDisplayObject writes that pair as the object's initial
   position. The card-rendering metadata names the original-deck number
   marker at 0x67, cached card type at 0x68 and deck-entry index at 0x6B.
   Other views of these polymorphic bytes keep their own names.

   back_face_handler at 0x10 is DisplayObject's name for the same word:
   both card constructors store DisplayObject_SetResourceVariantFromSign
   there, which DisplayObject_ProjectRotationFacing calls when the card
   faces away. z at 0x34 is DisplayObject's field_34.h.z: Duel_CreateFieldCardObject
   stores its third argument there beside x and a fixed -24 y, and
   Duel_RenderFieldCard3D loads it as the translation's t[2]. draw_callback
   at 0x4C is the secondary callback DisplayObject_RunSecondaryCallbackList
   calls with the object's ordering table once it is renderable;
   Duel_CreateCardDisplayObject stores Duel_DrawCardAtOutputPosition into it.
   texture_v at 0x5D is the high byte of DisplayObject's texture_uv:
   Duel_CreateFieldCardObject sets it to 0xC0 and Duel_RenderFieldCard3D
   copies it into the POLY_GT4's v0/v1. */
/* The +0x21/+0x22 bytes are the common display rotation_y/rotation_z.
   The two card-state helpers deliberately map card flags to them differently. */
typedef struct {
    u8 pad_00[0x04];
    u32 attribute;
    u16 flags;
    u8 pad_0A[0x02];
    u32 color;
    union {
        u32 word;
        void (*callback)(DisplayObject *, u32);
    } back_face_handler;
    u8 pad_14[0x0D];
    u8 rotation_y;
    u8 rotation_z;
    u8 pad_23[0x0D];
    s16 out_x;
    s16 out_y;
    s16 z;
    u8 pad_36[0x0C];
    u16 icon_variant;
    u8 pad_44[0x08];
    void *draw_callback;
    u8 pad_50[0x0C];
    u8 icon_state;
    u8 texture_v;
    u8 pad_5E[0x09];
    u8 deck_number_marker;
    u8 card_type;
    u8 is_back_facing;
    u8 card_index;
    u8 deck_index;
} DuelCardDisplayObject;

void Duel_UpdateCardDisplayMarker(DuelCardDisplayObject *object);
void Duel_UpdateCardDisplayState(DuelCardDisplayObject *object);

/* Duel_CreateHandCardDisplayObject calls this through an unprototyped cast
 * with the record alone, so its incoming a1/a2 values pass through. */
DuelCardDisplayObject *Duel_CreateCardDisplayObject(DuelCardRecord *record, s32 x, s32 y);

/* Creates a hand-card display object and marks it if the duel is
 * showing card markers: nonzero gDuel_pActiveSideState->card_view_mode sets
 * DUEL_CARD_FLAG_DISPLAY_MARKER on the record and copies data byte +0x04
 * plus one into deck_number_marker, or 0xFF when the view mode is negative.
 *
 * The declaration is the interesting part. The definition takes the record
 * alone, but all three callers pass three arguments -- record, x, y -- and
 * that is not a mistake in any of them: Duel_CreateHandCardDisplayObject hands the record to
 * Duel_CreateCardDisplayObject through an unprototyped cast in
 * duel_scene.c, so the two position words the caller left in
 * $a1/$a2 reach Duel_CreateCardDisplayObject untouched and become the object's initial
 * position. Narrowing a call site to one argument would delete its argument
 * setup and change the image.
 *
 * So a caller that makes the three-argument call goes through
 * DuelCreateHandCardDisplayObjectAt below, while every unit sees the real
 * prototype and the definition is checked against it.
 *
 * The return type is a card-rendering view of the shared display-pool slot.
 * Callers storing it in hand slots cast to the canonical DisplayObject;
 * duel_scene.c also uses that wider record for +0x6C. The narrow
 * return view and both measured argument-count arms below remain unchanged. */
DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(DuelCardRecord *card);
/* Callers that also pass the hand position in $a1/$a2 call through this. */
typedef DuelCardDisplayObject *(*DuelCreateHandCardDisplayObjectAt)(
    DuelCardRecord *card, s32 x, s32 y
);

/* Re-derives a duel card object's visual state from its own card record:
 * gDuel_aCardRecords[object->card_index] is the record, and every field this writes
 * comes from that record's flags. Face-down sets field_22, defense-position
 * sets field_21, DUEL_CARD_FLAG_USED_THIS_TURN dims the colour, and deck_number_marker
 * is cleared unless DUEL_CARD_FLAG_DISPLAY_MARKER asks to keep it.
 *
 * The thing a prototype cannot say is why every caller calls it: the object
 * carries no state of its own here, so a caller that has just changed the
 * record's flags must call this or the object keeps showing the old ones.
 * All four do exactly that -- DuelScene_UpdateDrawPhase after clearing
 * DUEL_CARD_FLAG_USED_THIS_TURN on every occupied record, DuelScene_UpdateResume
 * after restoring two ranges of records it re-set up,
 * DuelEffect_UpdateRevealCard after
 * clearing the field-effect bits, and Duel_UpdateCardToFieldView in the shared reset block
 * of the card flip animation.
 *
 * `DuelCardDisplayObject *` is the definition's own view. The callers hold
 * three others of the same memory -- DuelCardTurnObject, a local struct Obj,
 * and a raw u8 * read out of the record's first word -- which is why the four
 * local declarations this header replaces disagreed: two said u8 *, one u8 *
 * with a named argument, one Obj *. The pointer value is identical in every
 * case, so the call sites cast and the resident image is unchanged. */
void Duel_ApplyCardObjectFlags(DuelCardDisplayObject *object);

/* The two fields Duel_GetCardEffectVariant reads out of a card's display object: the
   rotation_z byte at 0x22 and the type byte at 0x68. It is a view of those two
   offsets, not a claim about the rest of the record, which is why the gaps
   are still padding. */
typedef struct {
    char pad_00[0x22];
    u8 rotation_z;
    char pad_23[0x45];
    u8 type;
} CardObject;

/* Returns the card's display value. The result is s32, not the s16 two of
   the three callers used to declare: the definition returns int, and every
   call site stores the result straight into a 16-bit field, so the narrowing
   they were describing happens at the store rather than at the return. */
s32 Duel_GetCardEffectVariant(CardObject *object);

/* Allocates a type 2 display object, passes x/y to DisplayObject_ConfigureSpriteAtPosition as
   its x/y, and marks it screen space. Defined in
   duel_scene.c; Duel_UpdateDisplayTransition creates the field-card panel
   through it. */
DisplayObject *Duel_CreateFieldCardPanel(int x, int y);

/* The mode byte for the "pick a card off the field" cursor, at 0x2CC($gp).
 * Duel_UpdateCardPickCursor owns it, and its own comment gives the bits:
 *
 *   0x80  the cursor has been initialised for this activation
 *   0x40  a pick has just been committed; hold until the display request clears
 *   0x20  the whole cursor mode should be torn down once 0x40 clears
 *
 * Zero therefore means the cursor is not active at all, which is how
 * Duel_InitScene uses it: it clears this byte in the run of assignments that
 * resets the duel, alongside gDuel_wDisplayTransitionFlags, the request/status
 * word the 0x40 hold waits on rather than a frame countdown.
 *
 * DuelScene_UpdateHandActions is the third accessor; it reads and writes the
 * byte gp-relative like everything else here. */
extern u8 gDuelCursor_PickFlags;

void Duel_UpdateCardPickCursor(DuelSelectionRecord *cursor);

/* Two GPU transfer rectangles per live card slot, including hand cards. */
extern RECT gDuel_aCardUploadRects[];

/* Releases the record's display object and clears DUEL_CARD_FLAG_OCCUPIED,
   leaving the rest of the flags alone. */
void DuelCard_DeactivateRecord(DuelCardRecord *object);

/* DuelCard_DeactivateRecord followed by clearing the whole flags halfword. */
void DuelCard_RemoveFromField(DuelCardRecord *object);

/* Builds the world-space field object; the position words become X/Z and
   world Y starts at -24. The caller publishes the record's object pointer. */
DuelCardDisplayObject *Duel_CreateFieldCardObject(s32 card_id, s32 x, s32 y);
/* Initializes the record from an encoded deck entry and attaches its field
   object. Callers handle retained flags/stat modifiers and old-object release. */
void Duel_SetupFieldCardFromDeck(s32 slot, s32 deck_index);

/* The fields the two card turn/flip animation steps read and write on a duel
 * card display object.
 *
 * This is NOT a full display-object description: it covers only the offsets
 * these two functions touch, and the padding names record the gaps rather
 * than claiming the memory is unused. Other translation units describe the
 * same object through their own, differently shaped views.
 *
 * The three bytes at 0x20..0x22 are written both individually and as one
 * 32-bit store, so they are modelled as a union. The 32-bit stores are the
 * retail code's way of setting all three channels at once: little-endian,
 * 0x8000 sets channel[1], 0xC000C0 sets channel[0] and channel[2], and
 * 0x800000 sets channel[2]. The same bytes are separately assigned the high
 * byte of the animation angle, which is what drives the tint across a flip.
 *
 * The angle at 0x2A is likewise a union. Every read and every stepping store
 * treats it as unsigned, and shifting its high byte out relies on that. The
 * two settle constants are stored signed, which is not interchangeable: a
 * negative 16-bit immediate is materialised with addiu, while the same
 * bit pattern written as unsigned becomes ori. Both spellings are therefore
 * kept rather than picking one and casting. */
typedef struct {
    u8 pad_00[0x08];
    u16 flags;
    u8 pad_0A[0x16];
    union {
        s32 packed;
        u8 channel[4];
    } tint;
    s32 field_24;
    u16 angle_step;
    union {
        u16 unsigned_value;
        s16 signed_value;
    } angle;
    u8 pad_2C[0x02];
    s16 mode;
    u8 pad_30[0x30];
    u16 counter;
    u8 pad_62[0x08];
    u8 record_index;
    u8 pad_6B[0x01];
    u8 phase;
} DuelCardTurnObject;

void Duel_UpdateCardToSelectionView(DuelCardTurnObject *p);
void Duel_UpdateCardToFieldView(DuelCardTurnObject *p);

/* Checks Select in the two-player duel path and arms the quit dialog. It has
 * no caller in matched C; retail reaches it from assembly. */
s32 Duel_CheckQuitInput(void);

/* The state that function arms, declared here because arming it is the whole
 * of what that function does. Two bits are established:
 *
 *   0x01  a quit was requested -- Duel_CheckQuitInput stores 1 when Select is
 *         pressed and gDuel_bOpponentID is negative.
 *   0x80  the dialog is already up. DuelScene_Update
 *         (src/game/duel_scene.c) is the consumer: on a nonzero
 *         value with 0x80 clear it sets that bit, plays SE 0x30 and raises
 *         the box through TextBox_CreateFlagged;
 *         with 0x80 set it waits for the box, destroys it, and clears the
 *         byte back to 0.
 *
 * Duel_InitScene clears it when a duel starts, in the same run of assignments
 * that resets the rest of the duel state.
 *
 * These three are the only accessors in the image, assembly included.
 * Duel_CheckQuitInput is matching C; the other two were candidates. */
extern u8 gDuel_bQuitDialogState;

/* The duel outcome flags the quit flow reports into, as the two small-data
 * users spell it.
 *
 * DuelScene_Update raises 0x2000 here at the point it clears
 * gDuel_bQuitDialogState above -- that is the quit result -- and Main_RunDuel
 * reads exactly that bit back. Duel_InitScene clears the word when the duel
 * starts, in the same run of assignments that clears the quit state, and
 * raises 0x1000 there for its own reason.
 *
 * The other two users take the plain scalar. */
extern u16 gDuel_wOutcomeFlags;

/* duel_scene.c's placement counter: DebugEffect_UpdatePreview zeroes
   it on first entry and steps it modulo 8 on each placement; nothing else reads
   it. It follows gDuel_wOutcomeFlags in the US and Japanese images, but the
   European ones place it elsewhere, so it is its own variable rather than a
   byte of the flags word. */
extern u8 gDebugEffect_bPlacementCounter;

DisplayObject *Duel_CreateCardEffectOverlay(DisplayObject *source);

/* Reads the held pad bits, priority-encodes them into a direction index and
 * hands the cursor and that index to DuelCursor_ProcessGridDirection. It computes the second
 * argument the cursor step wants, which is what separates it from
 * DuelCursor_UpdateDirection's one-argument call.
 *
 * It touches no field of the selection record, only forwards the pointer. */
void DuelCursor_ProcessHeldGridDirection(DuelSelectionRecord *cursor);

/* Advances toward the signed target row/column globals, returning 1 while
 * blocked, moving or off target, and 0 when idle at the target. It schedules
 * row movement before column movement. The checked direction-forwarding
 * call and signed-byte views remain part of the matching contract. */
s32 DuelCursor_StepTowardTarget(DuelSelectionRecord *cursor);

typedef struct {
    u8 pixels[0x500];
    u16 clut[64];
} DuelCardThumbnailBlock;

typedef char DuelCardThumbnailBlock_size_must_be_0x580[
    sizeof(DuelCardThumbnailBlock) == DUEL_CARD_DATA_BLOCK_SIZE ? 1 : -1
];
typedef char DuelCardThumbnailBlock_clut_offset_must_be_0x500[
    (u32)&(((DuelCardThumbnailBlock *)0)->clut) == 0x500 ? 1 : -1
];

typedef struct {
    s16 id;
    /* Source slot in the combined deck. Draw and placement paths consume it
       with signed byte loads before passing it to Duel_SetupCardRecord. */
    s8 deck_index;
    u8 data_block_index;
    /* Slot before shuffle, kept paired with the ID; the marker stores this + 1. */
    u8 pre_shuffle_index;
    u8 unk_05;
} DuelDeckCardRecord;

typedef char DuelDeckCardRecord_size_must_be_6[
    sizeof(DuelDeckCardRecord) == 6 ? 1 : -1
];
#define DUEL_DECK_CARD_OFFSET(member) ((u32)&(((DuelDeckCardRecord *)0)->member))
typedef char DuelDeckCardRecord_id_offset_must_be_0[
    DUEL_DECK_CARD_OFFSET(id) == 0 ? 1 : -1
];
typedef char DuelDeckCardRecord_index_offset_must_be_2[
    DUEL_DECK_CARD_OFFSET(deck_index) == 2 ? 1 : -1
];
typedef char DuelDeckCardRecord_data_block_offset_must_be_3[
    DUEL_DECK_CARD_OFFSET(data_block_index) == 3 ? 1 : -1
];
typedef char DuelDeckCardRecord_pre_shuffle_index_offset_must_be_4[
    DUEL_DECK_CARD_OFFSET(pre_shuffle_index) == 4 ? 1 : -1
];
typedef char DuelDeckCardRecord_unknown_offset_must_be_5[
    DUEL_DECK_CARD_OFFSET(unk_05) == 5 ? 1 : -1
];
#undef DUEL_DECK_CARD_OFFSET

extern DuelDeckCardRecord gDuel_aDeckCardRecords[];

/* Combined-deck thumbnail blocks, indexed by data_block_index. Population
 * copies each complete block; preview readback can replace it as 8x88 GPU
 * words. Setup uploads its 40x32 8-bpp pixels and 64-entry CLUT separately. */
extern DuelCardThumbnailBlock gDuel_aCardThumbnailBlocks[];
/* Returns the initialized live record; the slot/deck words keep their
 * existing full-width tagged-index conventions. */
DuelCardRecord *Duel_SetupCardRecord(s32, s32);

/* gDuel_aCardThumbnailBlocks[0].clut, bound by the linker as the block array
 * plus 0x500; block n's CLUT row starts n * DUEL_CARD_DATA_BLOCK_SIZE bytes on.
 * Duel_SetupCardRecord keeps this separate label because retail materializes
 * the CLUT address on its own instead of offsetting the shared block base. */
extern u8 gDuel_CardThumbnailClutBase[];

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
 * general. Duel_PopulateCombinedDeckData depends on that. */
void Duel_RequestCombinedDeckData(void);

/* Fills gDuel_aDeckCardRecords once the transfer Duel_RequestCombinedDeckData
 * starts has landed.
 *
 * For each combined-deck entry it takes the id and the pre-shuffle index byte
 * from their two tables, records the entry's own index twice, and then finds
 * that id in the unique id list to locate its data block, copying the block
 * to the running destination.
 *
 * That search is unbounded: it walks the unique id list until it finds a
 * match, with no end test, so it assumes every combined-deck id is present in
 * the list Duel_RequestCombinedDeckData built. That is a property of the pair
 * rather than of this function alone. */
void Duel_PopulateCombinedDeckData(void);

#define DUEL_DISPLAY_COLOR_NORMAL COLOR_RGB24_NEUTRAL_GREY
#define DUEL_DISPLAY_COLOR_DIMMED COLOR_RGB24_DIM_GREY

#define DUEL_CARD_READBACK_WIDTH_WORDS 0x8C
/* 0xD4 in the European build, whose wrapper defines it. */
#ifndef DUEL_CARD_READBACK_HEIGHT
#define DUEL_CARD_READBACK_HEIGHT 0xC4
#endif
#define DUEL_CARD_READBACK_WORD_COUNT \
    (DUEL_CARD_READBACK_WIDTH_WORDS * DUEL_CARD_READBACK_HEIGHT)

void Duel_DrawFieldCards(void);

/* The scratch record at 0x1F8003E0. DisplayObject_ProjectRotationFacing
   projects into its head and DisplayObject_SubmitPacket reads that head back as
   the packet origin;
   Duel_DrawCardFrame keeps the card's screen position in the word after it. */
typedef struct {
    DisplayObjectPacketOrigin origin;
    SpritePos pos;
} CardFrameScratch;

/* Draws one duel card's frame from its display object.
 *
 * The record is the canonical DisplayObject: this function reads the
 * attribute at 0x04, the flags at 0x08, the colour word at 0x0C, 0x14, the
 * word at 0x20 and the four tail bytes 0x67 to 0x6A, and every one of those
 * offsets is already named on that record. 0x6A is the index into
 * gDuel_aCardRecords that Duel_CreateCardDisplayObject (duel_scene.c) writes there, which is what
 * says this object is a duel card's.
 *
 * The second argument is the selected GsOT descriptor, passed straight to
 * the typed packet dispatcher. The projected-card caller supplies its
 * gGraphics_apOrderingTables[2] directly; the secondary-callback wrapper keeps
 * its word-shaped API and converts only at this boundary. Position widths
 * and the wrapper's partial first-object view remain unchanged. */
void Duel_DrawCardFrame(DisplayObject *object, GsOT *ordering_table, s32 screen_x, s32 screen_y);

/* Draws one duel card using the output coordinates stored on its object. */
void Duel_DrawCardAtOutputPosition(
    DisplayObjectPosition *object, s32 ordering_table
);

/* Returns nonzero when all five Exodia pieces are in hand. It consumes a local
 * copy of the hand and blanks each match, so a single hand entry cannot
 * satisfy two pieces. */
s32 Duel_HasAllExodiaPieces(void);

extern u8 gDuel_bDrawStepCountdown;

void DuelScene_UpdateDrawResolution(void);

/* gDuelEffect_apfnGroupHandler entry: the terrain effect step. It reads
 * gDuel_bTerrain back after storing it and decrements in the same
 * expression. */
void DuelEffect_ApplyTerrain(void);

/* Draws one 3D field card: `prim` is the lit POLY_GT4 card face, `sprite` the
 * POLY_FT4 projected under it, and `color` a scratch word that receives the
 * object's colour for RotColorDpq. The table index is deliberately read as
 * s8 for the half-turn test, matching retail even for values >= 0x80.
 * One caller: Duel_DrawFieldCards. */
void Duel_RenderFieldCard3D(DuelCardRecord *record, POLY_GT4 *prim, POLY_FT4 *sprite, s32 *color);

/* Compares the guardian-star matchup between two selection records and returns
 * display code 4 for neutral, 1 when the first loses, or 6 when it wins.
 * The second record keeps its independently matched interior-table alias. */
s32 DuelField_GetGuardianStarDisplayCode(
    DuelSelectionRecord *cursor_a, DuelSelectionRecord *cursor_b);

/* Rebuilds the card information box for the record's grid cell. */
void DuelField_RefreshCursorCardInfo(DuelSelectionRecord *source);

/* Stages the selected live card and creates its information text box. */
void DuelField_CreateCardInfoTextBox(
    DuelSelectionRecord *source, s32 card_record_index);

/* Creates the field cursor at record +0x04, selecting its resource from
 * record_index and the active side, positioning it on the grid and installing
 * ground projection. Target-selection entry and transition completion call
 * it. The sliding information panel is the separate pointer at +0x00. */
void Duel_CreateFieldCursorObject(DuelSelectionRecord *source);

/* Advances the active duel display transition: request entry, linked panel
 * objects, cursor/field objects and view tween. Clears the request once the
 * pending work is finished. The coordinator ignores its nonzero-busy return
 * and retains its existing request/effect/scene gates. */
s32 Duel_UpdateDisplayTransition(void);

/* Field-wide stat-restoration step driven once per 16 frames. It sweeps the
 * third grid row, staggering a type-8 effect object per slot and clearing a
 * negative stat modifier where it finds one. */
void DuelEffect_ApplyCursebreaker(void);

void DuelEffect_StartRitual(void);
void DuelEffect_ApplyRitual(void);

/* Card-effect activation, dispatch, and one handler the dispatcher can reach.
 *
 * DuelEffect_UpdateCardEffect is the dispatcher: while
 * DUEL_CARD_EFFECT_FLAG_ACTIVE is set in gDuel_wCardEffectFlags, it indexes
 * gDuelEffect_abGroupByEffectId by the current effect id and picks the second
 * of the group's two handlers when
 * DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER is set, and calls it -- then returns
 * gDuel_wCardEffectFlags. A caller therefore reads "still running" directly
 * from the flags word the handler just updated. Candidate DuelScene_Update tests
 * it exactly that way.
 *
 * DuelEffect_StartCardEffect maps the presented card ID to the effect ID the
 * dispatcher consumes, records the original card ID, and raises the
 * active/handler flags. DuelEffect_ApplyHarpiesFeatherDuster is its own group
 * handler. */
void DuelEffect_ApplyHarpiesFeatherDuster(void);
s32 DuelEffect_UpdateCardEffect(void);
void DuelEffect_StartCardEffect(s32 value, s32 flag);

/* Returns the first unoccupied slot at or after `start` on the active side,
 * or -1 when the five-slot row is full. */
s32 Duel_FindFreeFieldSlot(s32 start);

/* Collecting occupied field records into a caller-supplied array.
 *
 * Both collectors terminate the array with a null, so the caller's array must
 * hold one more entry than the row it scans. They return how many were
 * written, which is the same count the null terminator implies.
 *
 * The element type is DuelCardRecord * because that is what both collectors
 * walk and store: each writes `*out++ = r` from a DuelCardRecord * cursor
 * over gDuel_aCardRecords. This used to be u8 **, on the grounds that a byte pointer
 * was what they stored and that callers holding DuelCardRecord * should
 * convert at the call site. Neither half held up: the cast was a consequence
 * of the parameter type rather than a fact about the data, and the callers
 * did the opposite -- duel_scene.c passes DuelCardRecord *list0[6]
 * uncast, which warned, while casting a stale local type for another call in
 * the same file. */
s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **out, s32 start, s32 type_limit);
s32 Duel_CollectFieldCardsByType(DuelCardRecord **out, s32 start, s32 type);

/* Returns 0 when it has written an equip pairing into gAi_Selection, 1 when
 * there is nothing to pair. */
s32 Duel_SelectEquipPair(void);

/* The same convention, for a trap rather than an equip: 0 once it has written
 * a play into gAi_Selection, 1 when it found no trap on the field or no free
 * slot. duel_scene.c calls the two in adjacent lines behind the same
 * `(rand() & 1) == 0 && ... == 0` guard, so they are alternatives in one
 * chain rather than unrelated helpers. */
s32 Duel_SelectTrapPlay(void);

/* Initializes the active side's placement cursor, restricts its row, chooses
 * a free column when available and requests the selection display. */
void Duel_BeginFieldPlacementSelection(DuelSelectionRecord *selection);

/* Applies the script's card picks to the hand through record swaps, display
 * replacement and conversion to hand-slot selection codes. */
void Duel_ApplyAiHandSelection(void);

s32 Duel_GetCardViewerRequestId(DuelCardRecord *card);

/* Positions the existing hand-selection cursor from the signed slot index. */
void DuelHand_PositionSelectionCursor(DuelSelectionRecord *state);

/* Dims only unmarked slots. With no ordered picks, restores the cursor's
 * card; selected cards otherwise retain their existing colors. */
void DuelHand_RefreshSelectionColors(DuelSelectionRecord *side);

/* Creates the current hand slot's order-marker sprite, lifts the card and
 * records the incremented count in selection_order. The depth calculation reads
 * depth_offset as u8 before adding one and narrowing to s8. */
void DuelHand_CreateSelectionOrderMarker(DuelSelectionRecord *state);

/* Initializes a center-relative projection of the fixed ground point
 * (1000, 0, 1000) under the reference view. The projected screen x and y,
 * less 0xA0 and 0x6C, are stored in
 * gDuel_sGroundReferenceScreenX and gDuel_sGroundReferenceScreenY; no
 * matched source reads them back.
 *
 * ViewState_ResetDefaults runs before and after sampling; this
 * does not save and restore the caller's prior view. Duel_InitScene then
 * applies the active side's angle separately.
 *
 * Duel_InitScene (src/game/duel_scene.c) is the observed consumer and
 * reached it through a local extern that already agreed with this. */
void Duel_InitGroundProjectionReference(void);
extern s16 gDuel_sGroundReferenceScreenX;
extern s16 gDuel_sGroundReferenceScreenY;

/* Runs the two model setup steps Model_ResetSceneView and
 * Model_ResetSceneState, then calls Model_LoadSlot with selector 2,
 * gRuntime_pSharedWorkBase, 0x63000, and 4. The callee uses the pointer
 * only as a non-null gate; this call does not load or read the payload
 * (notes/duel-package-unused-data.md).
 *
 * The name is kept address-based: the call sequence is legible but what the
 * selector and the trailing 4 choose is not established here, and nothing in
 * this unit names the buffer's contents.
 *
 * Duel_InitScene (src/game/duel_scene.c) is the only consumer and
 * reached it through a local extern that already agreed with this. */
void Duel_InitModelScene(void);

/* The duel entry point, and the two banner display objects it builds.

   Duel_InitScene allocates both from DisplayObject_AcquireSlot and hands each to
   DisplayObject_SelectOrderingTable1, whose parameter is DisplayObject *, which is what #3176
   established and what DuelScene_UpdateResultOutro's source already declared. The
   other consumers use the typed view. The Exodia candidate takes the guarded
   byte-pointer arm because its offset-based accesses are codegen-sensitive.

   The prototype is the one Main_RunDuel (main_mode_runners.c) held as its own
   extern, which was the only declaration anywhere. */
void Duel_InitScene(void);

/* The two status sprites Duel_InitScene creates: the terrain icon at (12, 24),
   whose variant is gDuel_bTerrain and which DuelEffect_ApplyTerrain re-selects,
   and the panel at (280, 32) that Duel_DrawLifePointsAndDeckCounts positions
   its numbers against. The result outro slides both away. */
extern DisplayObject *gDuel_pTerrainIcon;
extern DisplayObject *gDuel_pLifePointsPanel;

/* Save-data windows selected while the duel scene starts, one per duel side.
 * The no-opponent path points them at the two 0x1000-byte halves of
 * gSaveData_aPairWorkspace; the normal path uses the buffers returned by the
 * duel package transfer. They are two adjacent scalars, each defined once in
 * duel_scene.c: Duel_InitScene stores each through its own
 * GP-relative address, which one two-entry array would fold into a shared
 * base register.
 *
 * DuelResult instead indexes the pair by side, and retail reloads side 0's
 * pointer after each starchip store. GCC 2.8 only reloads a value that a
 * store through a structure field may alias when the read itself is a
 * structure or array reference, so DUEL_SAVE_WINDOW reads the two adjacent
 * pointers as the pair they are. */
extern SaveDataState *gDuel_pSide0SaveData;
extern SaveDataState *gDuel_pSide1SaveData;

typedef struct DuelSaveWindows {
    SaveDataState *side[2];
} DuelSaveWindows;

#define DUEL_SAVE_WINDOW(side_index) \
    (((DuelSaveWindows *)&gDuel_pSide0SaveData)->side[side_index])

/* The staged loader for the duel terrain package. Its two consumers never call
 * it: duel_scene.c and duel_scene.c each pass its
 * address as the
 * per-stage callback of a transfer request, so the transfer machinery supplies
 * both arguments later. That is why both could declare it `(void)` without the
 * build noticing. */
void Duel_LoadPackageStage(FileTransferDescriptor *d, s32 stage);

extern u8 gAiScript_abFieldScript[];

void DuelScene_UpdateStartup(void);
void DuelScene_UpdateDrawPhase(void);

#define DUEL_RANK_SCORE_INITIAL 50
#define DUEL_RANK_SCORE_THRESHOLD_COUNT 5

#define DUEL_RANK_ADJUST_EXODIA_WIN 40
#define DUEL_RANK_ADJUST_DECK_OUT_WIN (-40)

#define DUEL_RESULT_TEXT_SELECTOR_DEFAULT 0x40
#define DUEL_RESULT_TEXT_SELECTOR_DECK_OUT 0x41
#define DUEL_RESULT_TEXT_SELECTOR_EXODIA 0x42

#define DUEL_RANK_RULE_TURNS 0
#define DUEL_RANK_RULE_EFFECTIVE_ATTACKS 1
#define DUEL_RANK_RULE_DEFENSIVE_WINS 2
#define DUEL_RANK_RULE_FACE_DOWN_PLAYS 3
#define DUEL_RANK_RULE_PURE_MAGIC 4
#define DUEL_RANK_RULE_TRAPS_TRIGGERED 5
#define DUEL_RANK_RULE_CARDS_USED 6
#define DUEL_RANK_RULE_REMAINING_LP 7
#define DUEL_RANK_RULE_INITIATE_FUSION 8
#define DUEL_RANK_RULE_EQUIP_MAGIC 9

/* One threshold/change pair in a duel-rank rule row. The first threshold
 * strictly above the measured value supplies the score change. */
typedef struct {
    s16 threshold;
    s16 score_change;
} DuelRankScoreChangeEntry;

extern DuelRankScoreChangeEntry
    gDuel_awRankScoreChange[][DUEL_RANK_SCORE_THRESHOLD_COUNT];

s32 Duel_CalcRankScoreChange(s32 rule, s32 value);

#define DUEL_RESULT_DISPLAY_CHILD_COUNT 10
#define DUEL_RESULT_DISPLAY_OFFSET(member) \
    ((u32)&(((DuelResultDisplayState *)0)->member))

typedef struct DuelResultDisplayState {
    DisplayObject *root;
    DisplayObject *children[DUEL_RESULT_DISPLAY_CHILD_COUNT];
    /* Duel_CalcRankScore seeds both of these with
       DUEL_RANK_SCORE_INITIAL and then accumulates every
       Duel_CalcRankScoreChange result into them, indexed by side. */
    s32 side_scores[2];
    /* Three, not two: Duel_CalcRankScore writes 0x34, 0x35 and 0x36
       as one group -- 0x44, a variant selector, and 0x45. */
    u8 page_text_ids[3];
    u8 page_index;
    u8 rank_tier;
    u8 is_tec_rank;
    u8 starchip_prize;
    u8 pad_3B;
    s16 dropped_card_id;
    u8 pad_3E[2];
} DuelResultDisplayState;

typedef char DuelResultDisplayState_children_offset_must_be_0x4[
    DUEL_RESULT_DISPLAY_OFFSET(children) == 0x4 ? 1 : -1
];
typedef char DuelResultDisplayState_page_text_ids_offset_must_be_0x34[
    DUEL_RESULT_DISPLAY_OFFSET(page_text_ids) == 0x34 ? 1 : -1
];
typedef char DuelResultDisplayState_page_index_offset_must_be_0x37[
    DUEL_RESULT_DISPLAY_OFFSET(page_index) == 0x37 ? 1 : -1
];
typedef char DuelResultDisplayState_rank_tier_offset_must_be_0x38[
    DUEL_RESULT_DISPLAY_OFFSET(rank_tier) == 0x38 ? 1 : -1
];
typedef char DuelResultDisplayState_starchip_prize_offset_must_be_0x3A[
    DUEL_RESULT_DISPLAY_OFFSET(starchip_prize) == 0x3A ? 1 : -1
];
typedef char DuelResultDisplayState_dropped_card_id_offset_must_be_0x3C[
    DUEL_RESULT_DISPLAY_OFFSET(dropped_card_id) == 0x3C ? 1 : -1
];
typedef char DuelResultDisplayState_size_must_be_0x40[
    sizeof(DuelResultDisplayState) == 0x40 ? 1 : -1
];

#undef DUEL_RESULT_DISPLAY_OFFSET

/* The record itself, or rather the pointer the game reaches it through. It
 * belongs beside the layout above because that layout is the whole of what
 * this header knows about it.
 *
 * The display helpers read the pointer: Duel_ShowResultPage walks it as
 * `gDuel_pResultDisplayState->root` and `gDuel_pResultDisplayState->page_text_ids[page]`, and
 * Duel_CalcRankScore takes it into a local before working through the same
 * record.
 *
 * The sole writer, DuelScene_UpdateResultRewards, points it at
 * gDuel_awRitualData's reused
 * image-backed storage, which spans this complete record; it is not a
 * pointer to a short recipe allocation. */
extern DuelResultDisplayState *gDuel_pResultDisplayState;

/* Duel-result and rewards UI controller installed by the scene table. */
void DuelScene_UpdateResultRewards(void);

void DuelResult_LoadOutroPackageStage(FileTransferDescriptor *descriptor, s32 mode);

void DuelResult_UpdateOrbitSprite(DisplayObject *object);
void DuelResult_UpdateDepartingSprite(DuelCardDisplayObject *object);

#define DUEL_RESULT_SPRITE_COUNT 7

/* One row entry of the duel-result sprite tables gDuelResult_aSpriteSpecsOpponent (a real opponent)
   and gDuelResult_aSpriteSpecsNoOpponent (none). Each table is indexed by the winning side and then
   by sprite, DUEL_RESULT_SPRITE_COUNT of them, and DuelScene_UpdateResultOutro spends the
   whole entry in one call: `x` and `y` are the spawn position it hands
   DisplayObject_ConfigureSpriteAtPositionWithResource, `kind` is that call's part selector and doubles as the
   "this slot is used" test, and `tag` goes to the object's field_48. */
typedef struct {
    u8 x;
    u8 y;
    u8 kind;
    u8 tag;
} DuelResultSpriteSpec;

/* One slot of the table DuelScene_UpdateResultOutro keeps in the gDuel_awRitualData scratch
   while the outro runs: the object it spawned for that sprite, so step 3 can
   retarget every one of them. Only the pointer is reached; the rest of the
   0xC-byte stride is what the scratch already holds. */
typedef struct {
    DisplayObject *object;
    u8 pad_04[0xC - 4];
} DuelResultSpriteSlot;

typedef char DuelResultSpriteSpec_size_must_be_4[
    sizeof(DuelResultSpriteSpec) == 4 ? 1 : -1
];
typedef char DuelResultSpriteSlot_size_must_be_0xC[
    sizeof(DuelResultSpriteSlot) == 0xC ? 1 : -1
];

extern u16 gDuelResult_wBgmId;

/* gDuel_apfnSceneStateHandler duel phase entry: the result outro. It spawns one object per row
 * of the winning side's sprite table -- gDuelResult_aSpriteSpecsOpponent with a real opponent,
 * gDuelResult_aSpriteSpecsNoOpponent without -- keeps them in the gDuel_awRitualData scratch as
 * DuelResultSpriteSlot entries, and retargets all of them on its third step. */
void DuelScene_UpdateResultOutro(void);

/* One weighted card-drop row: CARD_COUNT halfword weights followed by the
 * established padding that makes each pool row 1,460 bytes. */
typedef struct {
    u16 weights[CARD_COUNT];
    u8 pad_05A4[DUEL_DROP_TABLE_PADDING_SIZE];
} DuelDropTable;

extern DuelDropTable gDuel_awSaPowCardDrops[];

void Duel_ShowResultPage(s32 page);
void Duel_CalcRankScore(void);
s32 Duel_SelectCardDrop(s32 pool_index);
void Duel_AwardCard(s32 card_id);

/* Shared ritual result id: DuelEffect_StartRitual and DuelEffect_ApplyRitual retain the
 * signed low halfword returned by Duel_CheckRitual. */
extern s16 gDuel_RitualResultCardId;

/* Scratch display object retained across controller phases. Other duel
 * animations reuse this pointer; it does not own a private object pool. */
extern DisplayObject *gDuel_pCardPreviewObject;

/* The ritual caller sign-extends its result; placement passes its masked
 * card ID. The full word supplies the existing library-flag offset, not
 * an object address. The helper preserves modulo-32-bit addition. */
void Duel_MarkCreatedCardKnown(s32 card_id);

/* Duel scene-state 9, the battle resolution phase, and the two side pairs it
 * keeps private. DuelScene_UpdateBattle (src/game/duel_scene.c) is the
 * only source in the tree that names either pair; duel_scene_hand_actions.h
 * and duel_scene_field_actions.h record the same arrangement for scene-states
 * 4 and 5.
 *
 * Both are a two-entry pair indexed by gDuel_bPresentationCursor
 * (duel_scene_card_placement.h), which that source takes through its signed
 * view. Every access to both is gp-relative in the retail listing: four
 * relocations each, none through %hi/%lo. The same listing reaches
 * gMain_aAnimatedBattleModels through %hi/%lo three times and gp-relatively never, which is
 * what tells the two forms apart here.
 *
 * Neither declared bound comes from the address map, and that is worth being
 * explicit about: the next named symbol after both is gDuel_pActiveSelectionRecord, sixteen and
 * four bytes on, so the gaps are larger than the declarations and are no
 * evidence for them. The bound is what the one declarer wrote, and what the
 * use sites exercise -- [0] and [1] written, and the pair read at that one
 * index.
 */

/* Scene-state 9: presents the attacker and the defender, resolves the
 * exchange, shows the damage and the guardian-star bonus, and hands off to
 * the trap, fusion-result and turn-switch states. */
void DuelScene_UpdateBattle(void);

/* gDuel_apfnSceneStateHandler duel phase entry: the side-swap phase. On its first tick it starts
 * the swap view tween through Duel_StartViewTween -- the direction taken from
 * gDuel_bActiveSide -- and sets display request 0x10. When that request clears,
 * it flips gDuel_bActiveSide, repoints gDuel_pActiveSideState and gDuel_pActiveFieldGrid at the new
 * acting side, and hands the scene to phase 2. */
void DuelScene_UpdateTurnSwitch(void);

/* The life-point delta each side takes from the exchange. The resolution step
 * zeroes both entries and then stores Duel_CompareBattleStrength's result into the entry for
 * the side that lost points; case 8 reads it back as the damage to present,
 * sizing the effect from its magnitude -- |damage| / 1000, capped at 2.
 *
 * The listing shows three sh and no load at all: the read is indexed, so it
 * comes out as one addiu forming the base off $gp. That is the array form,
 * and the private declaration's s16 [2] is kept verbatim. */
extern s16 gDuel_awBattleDamage[2];

/* How that exchange went for each side, as a small signed marker rather than a
 * count: the same step writes 0, 1 and -1 into the two entries. Case 8 takes
 * its address into a local and tests the entry against zero to decide whether
 * to raise the damage-number request at all, then tests it again for
 * greater-than-zero to pick the variant.
 *
 * Three sb and, as above, no load: taking the address gives one addiu off
 * $gp. The signed s8 [2] is what the one declarer wrote and what the -1 stores
 * need, and it is kept verbatim. */
extern s8 gDuel_abBattleOutcome[2];

/* The presentation's card/result halfword. Card use also defines and writes
 * gDuel_wPlacementCardId as u16; placement tests its signed value and uses bit 15 to
 * distinguish fusion from equipment. The low twelve bits are the card id. */
extern u16 gDuel_wPlacementCardId;
extern s16 gDuel_wEquipCardId;

/* A signed read of the same two bytes, not another allocation. The aggregate
 * read preserves the result-publication dependency in the combination path. */
typedef struct {
    s16 value;
} DuelPlacementCardIdCell;

typedef char DuelPlacementCardIdCell_size_must_be_2[
    sizeof(DuelPlacementCardIdCell) == 2 ? 1 : -1
];

void DuelScene_UpdateCardPlacement(void);

/* gDuel_apfnSceneStateHandler entry 6, the card-use presentation sequence. Reached when
 * DuelScene_Update (src/game/duel_scene.c) dispatches on
 * `gDuel_wSceneStateFlags & DUEL_SCENE_PHASE_MASK`, and it hands control
 * back by writing 5 into that word.
 *
 * On entry, while DUEL_SCENE_FLAG_INITIALIZED is clear, it latches the selected
 * slot's card id -- gDuel_aCardRecords[object->field_6A] -- into gDuel_wPlacementCardId, starts
 * CardPreview_RequestResourceTransfer's resource request, bumps gDuel_pActiveSideState->field_05 for a
 * CARD_TYPE_MAGIC card, and seeds gDuel_bSceneSubStep with 1.
 *
 * gDuel_bSceneSubStep then drives five states, its low nibble selecting the step and
 * the high bits latching progress within it:
 *
 *   1  opens the enlarged card window with CardPreview_CreateResourceObjects, ramps its
 *      field_21 alpha in steps of 6, releases the original object, then
 *      holds on field_60 counting down
 *   2  hands the latched id to DuelEffect_StartCardEffect with flag 0
 *   3  builds two Duel_CreateCardEffectOverlay copies of the window,
 *      one GsALON | GsAONE
 *      and one GsALON | GsATWO, sweeps their +0x44/+0x46 pair and their
 *      0xC colour word down together, and releases both when it reaches 0
 *   4  repeats DuelEffect_StartCardEffect with flag 1 and returns to state 5
 *
 * The previous comment here described a hand-refresh phase that walks the
 * acting side's hand slots. That is not this function: nothing here reads a
 * hand, and functions.csv and notes/research/the-game.md both already called
 * it the card-use presentation sequence. */
void DuelScene_UpdateCardUse(void);

void DuelScene_UpdateExodiaResult(void);

/* State private to duel scene-state 5, the field navigation, attack selection
 * and battle transition phase. DuelScene_UpdateFieldActions
 * (src/game/duel_scene.c) is the only source in the tree that
 * names either of these. This is the arrangement duel_scene_hand_actions.h
 * records for the neighbouring scene-state 4.
 *
 * Every access to both is gp-relative in retail: 4 and 6 relocations, none
 * through %hi/%lo, so these are the plain scalar declarations. That the
 * distinction is real rather than an artefact of the pattern is visible in
 * the same function, where gAi_Selection is reached through %hi/%lo five times
 * and gp-relatively never.
 */

/* The scene-state 5 callback, entry 4 of duel_scene.c's table. */
void DuelScene_UpdateFieldActions(void);

/* The target colour of the field dim ramp: one grey level repeated in three
 * channels. The pass that dims the field sets it to 0x202020
 * (duel_scene.c) and a second site sets it to 0x808080 while starting
 * the same walk over the gDuel_aCardRecords card records (:616). The ramp moves
 * gModel_dwFogBaseColor (sorted_entry.h, which this unit already includes) eight levels at
 * a time towards it, reading this symbol's low
 * channel through a (u8) cast to get the level, clamping there, repacking the
 * three channels, and comparing the whole word to know it has arrived
 * (:564, :577).
 *
 * Declared s32, which is what the private declaration said and what the
 * listing shows: the two stores are sw, and the two loads are one lw for that
 * word comparison and one lbu for the channel read -- so the narrow read is a
 * cast at the use rather than the declaration's width. */
extern s32 gDuel_dwFieldDimTarget;

/* Which of the two field passes the state is in. It is cleared when the state
 * sets up (duel_scene.c), raised once and cleared again,
 * and its three reads choose between the dim-and-ramp path and the path that
 * waits for the tagged display objects to retire before finishing
 *.
 *
 * Declared u8: the three loads are lbu and the three stores are sb. Every
 * value written is 0 or 1 and every read is a test against 0, so the stores
 * cannot settle a sign here -- but the loads are the unsigned form, which is
 * the spelling its one declarer chose, and it is kept verbatim. */

/* The attacker's and the target's record flags and stat modifiers, saved
 * when the field commits an attack or a fusion so the battle state can
 * restore them. All four are gp-relative halfwords in retail.
 *
 * Both are two-entry side arrays (attacker, target) owned by
 * src/game/duel_scene.c. As side arrays rather than struct
 * elements, DuelScene_UpdateBattle's loads cannot pass the record stores,
 * which is what reloads gDuel_awSavedRecordFlags[1] after each flags store. */
extern u16 gDuel_awSavedStatModifiers[2];
extern u16 gDuel_awSavedRecordFlags[2];

/* Duel scene-state 4, the hand navigation and card play phase, and the state
 * private to it. DuelScene_UpdateHandActions (duel_scene.c) is
 * the only source in the tree that names any of the four globals below;
 * duel_scene.c takes the prototype for the phase callback table.
 *
 * Every access to the last three is gp-relative in retail: 5, 3 and 4
 * relocations, none through %hi/%lo. So these are the plain scalar declarations. That the
 * distinction is real rather than an artefact of the pattern is visible in
 * the same function, where gAi_Selection is reached through %hi/%lo and
 * gp-relatively never.
 */

/* The scripted path's cursor target. The hand-selection step stores the
 * selection byte minus 11 and steps the hand cursor toward it; the placement
 * step stores the target column and steers gDuelCursor_Direction until the
 * selection record's column equals it. Its storage is defined with the duel
 * scene's other small state in duel_scene.c, two bytes after
 * gDuel_bBattleRemovalSlot0; it is a separate object rather than a third
 * replay slot, since the European builds place it 0x54 bytes away from the
 * replay pair. */
extern s8 gDuel_bAiCursorTarget;

/* Whether the scripted path has a card to stack onto the hand. The AI step
 * clears it, and raises it when gAi_Selection.bytes[1] (ai.h)
 * is non-zero; the following step keeps bit 0 and tests the result, and the
 * delayed step tests bit 0 again to decide whether the hand-stack child is
 * spawned.
 *
 * The two loads are lhu and the three stores are sh. Both reads are masked
 * with 1, and GCC loads unsigned for a mask that discards the sign, so the
 * declared sign does not show in the object code and the stores cannot settle
 * it either; s16 is kept from the declaration the unit matched with. The next
 * named symbol is gDuel_RitualResultCardId, two bytes on, so the declared width fills the
 * gap exactly. */
extern s16 gDuel_wAiHandStackPending;

/* The cursor into the AI's pending selection list gAi_Selection (ai.h). The
 * scripted step sets it to -1 through a signed lvalue cast, and each following
 * step pre-increments it and reads gAi_Selection at the new index, so the walk
 * starts at the list's first entry.
 *
 * Declared u8, as the private declaration had it: the single load is lbu and
 * the two stores are sb, and the signed views are casts at the use sites --
 * the arrangement duel_scene.h records for gDuel_bPendingDrawCount, where the declaration
 * stays unsigned and the cast at the use is what keeps the signed read. The
 * next named symbol, gDuel_wAiSelectionStepDelay below, is two bytes on, so
 * the one-byte gap is not evidence of a wider object. */
extern u8 gDuel_bAiSelectionCursor;

/* The step delay for that walk. Each selection entry loads it with 6, every
 * tick takes one off, and the step runs once the signed value reaches zero or
 * below, reloading 6.
 *
 * Declared u16. The single load is lhu and the three stores are sh; the one
 * signed read is spelled (s16) at the bound test, which is where the private
 * declaration put it. */
extern u16 gDuel_wAiSelectionStepDelay;

/* "Com Error SelCard\n", the scripted path's complaint about a selection byte
 * below 0xB. It stays in generated .rodata: retail keeps the string at
 * 0x80010060, ahead of every duel_scene jump table, but a literal would be
 * emitted after the tables of the earlier functions (from
 * Duel_LoadPackageStage's on). */
extern const char gDuel_szComSelCardError[];

/* The hand card's display object as DuelScene_UpdateHandActions reads it.
 * The position pairs are unions because retail copies them as words, and the
 * members are limited to what that function touches; the canonical record is
 * DisplayObject in display_object.h. */
typedef struct {
    u8 pad_00[8];
    u16 flags;                  /* 0x08 */
    u8 pad_0A[2];
    u32 color;                  /* 0x0C */
    u8 pad_10[6];
    u8 depth;                   /* 0x16 */
    u8 pad_17[0xA];
    u8 face;                    /* 0x21 */
    u8 pad_22[2];
    void (*update)();           /* 0x24 */
    union { struct { s16 x, y; } xy; s32 word; } target;  /* 0x28 */
    union { struct { u16 x, y; } xy; s32 word; } saved;   /* 0x2C */
    union { struct { u16 x, y; } xy; s32 word; } pos;     /* 0x30 */
    u8 pad_34[2];
    u16 home_x;                 /* 0x36 */
    u16 home_y;                 /* 0x38 */
    u8 pad_3A[0x22];
    u8 icon_state;              /* 0x5C */
    u8 pad_5D[3];
    s16 step;                   /* 0x60 */
    u8 pad_62[6];
    u8 kind;                    /* 0x68 */
    u8 pad_69;
    u8 card_index;              /* 0x6A */
    u8 pad_6B;
    u8 field_6C;                /* 0x6C */
} HandCardObject;

typedef char HandCardObject_field_6C_offset_must_be_0x6C[
    (u32)&(((HandCardObject *)0)->field_6C) == 0x6C ? 1 : -1
];

void DuelScene_UpdateHandActions(void);

/* gDuel_apfnSceneStateHandler duel phase entry: the duel-resume phase. It re-sets up every
 * occupied record of both card ranges, restoring the flags and position each
 * one had, spawns the side indicator objects, and then either replays the
 * field effects or starts the BGM before fading in. Later ticks walk the
 * gDuel_bBattleRemovalSlot0 replay list (duel_scene.h)
 * one entry per tick. */
void DuelScene_UpdateResume(void);

void DuelScene_Update(void);
void DuelScene_UpdateWithSideInput(void);

#define DUEL_SCREEN_CARD_POSITION_COUNT 30
#define DUEL_EXODIA_CARD_POSE_COUNT 5

typedef struct {
    u8 work_slot;
    u8 biased_x;
    u8 biased_y;
} DuelExodiaCardPose;

typedef struct {
    DuelExodiaCardPose poses[DUEL_EXODIA_CARD_POSE_COUNT];
    u8 padding;
} DuelExodiaCardPoseTable;

/* The card-slot projection coordinates and the two duel-result sprite
   tables, declared here so the source that defines them and the units that
   read them cannot drift apart. gDuel_aCardSlotGroundPositions is declared as its DuelFieldPosition pairs
   everywhere: the defining unit duel_scene.c and
   duel_scene.c, whose Duel_SetupFieldCardFromDeck loads the
   pair with lh, read it that way. duel_scene.c reads one pair's
   members as unsigned halfwords and duel_scene.c casts
   to a halfword pointer. The
   pair supplies ground X/Z; consumers choose height independently and the
   shared type retains its generic x/y names. */
extern DuelFieldPosition gDuel_aCardSlotGroundPositions[DUEL_SCREEN_CARD_POSITION_COUNT];
extern ScreenPair gDuel_aCardSlotScreenPositions[DUEL_SCREEN_CARD_POSITION_COUNT];
extern DuelExodiaCardPoseTable gDuel_ExodiaCardPoses;
extern DuelResultSpriteSpec gDuelResult_aSpriteSpecsOpponent[][DUEL_RESULT_SPRITE_COUNT];
extern DuelResultSpriteSpec gDuelResult_aSpriteSpecsNoOpponent[][DUEL_RESULT_SPRITE_COUNT];

/* Projects one card-slot ground pair at height -24; returns signed screen X. */
s16 Duel_ProjectCardSlotScreenX(s32 index);

/* Searches the acting side's five mapped field slots for an occupied trap
 * whose id equals the argument, and returns that id on a hit or 0 on a miss.
 *
 * A hit also records the id in gDuel_wSelectedTrapCardId and the owning object's +0x6A slot
 * index in gDuel_bSelectedTrapSlot; a miss leaves gDuel_wSelectedTrapCardId cleared. duel_side_state.h
 * describes both of those globals, and the definition in duel_scene.c
 * carries the rest.
 *
 * duel_scene.c calls it with two card ids, so the return is used as a
 * present/absent test rather than for the value it returns. */
s32 Duel_SelectTrapByCardId(s32 card_id);

void DuelSelection_UpdateLinkedObject(DisplayObject *object);

/* Shuffles both duelists' decks into gDuel_awPlayerShuffledDeck, running
 * Duel_ShuffleDeck once per side. Each side gets a halfword id list and a
 * byte order list, and the byte lists sit below the halfword base at -0x54
 * and -0x2C.
 *
 * Each source is an optional read-only halfword card-ID list. Null retains
 * Duel_ShuffleDeck's weighted-generation path. */
void Duel_ShuffleBothDecks(const u16 *player_deck, const u16 *opponent_deck);

/* The halfword base those four lists are reached from: the player's shuffled
 * card IDs. Duel_ShuffleBothDecks is the only C source that names this base.
 *
 * The offsets above are not free-standing numbers -- config/slus_01411/symbols.txt
 * names what each one lands on. From this symbol at 0x80177FE8,
 * `+ DECK_SIZE * sizeof(u16)` is gDuel_awOpponentShuffledDeck (0x80178038),
 * `- 0x54` is gDuel_awPlayerDeckShuffle (0x80177F94) and `- 0x2C` is
 * gDuel_awOpponentDeckShuffle (0x80177FBC). The distance to the next named
 * symbol is 0x50, which is DECK_SIZE halfwords exactly, so the player's list
 * ends where the opponent's begins.
 *
 * That distance is recorded as evidence about the extent, not asserted as a
 * bound: the declaration stays incomplete. Its u16 elements match the two-byte
 * ID outputs. The caller retains one base and uses byte views only for the
 * permutation-list addresses; all four offsets are unchanged. This does not
 * combine the lists or retype the adjacent byte output buffers. */
extern u16 gDuel_awPlayerShuffledDeck[];

/* The opponent's drop-weight table, loaded per opponent and drawn from.
 *
 * Duel_InitScene fills it, handing the address to a transfer as the
 * destination and choosing the source from the opponent id:
 *
 *     File_RequestAsyncTransfer(0, 0, gDuel_bOpponentID * 2 + gDuel_bOpponentID
 *                               + 7475, 3, 0, 0, (s32)gDuel_awOpponentDeckPool);
 *
 * so three sectors per opponent. Duel_ShuffleDeck then reads it as halfword
 * weights: it draws a limit in 1..DUEL_DROP_WEIGHT_TOTAL and walks the table
 * accumulating unsigned halfwords until the running total reaches it, which is a
 * weighted pick rather than a list of card ids.
 *
 * The incomplete u16 array models those weights without assigning the whole
 * three-sector transfer a new fixed table extent.
 *
 * The shuffle source is an optional read-only halfword list: null selects
 * weighted generation instead. The two outputs are DECK_SIZE halfword card
 * IDs and DECK_SIZE original-slot bytes, permuted together. Pointer steps
 * advance one element, preserving the measured two-byte and one-byte walks.
 * No card-ID validation, source/output separation or new bound is imposed.
 */
extern u16 gDuel_awOpponentDeckPool[];

void Duel_ShuffleDeck(const u16 *src, u16 *out16, u8 *out8);

#define DUEL_SWORDS_DURATION_TURNS 3
#define DUEL_SWORDS_INITIAL_COUNTER (DUEL_SWORDS_DURATION_TURNS + 1)

void DuelEffect_StartSwords(void);
void DuelEffect_ApplySwords(void);

/* Six terrains, matching the range Duel_GetTerrainBoost indexes with
   gDuel_bTerrain - 1. */
#define DUEL_TERRAIN_COUNT 6
#define DUEL_TERRAIN_STAT_ADJUSTMENT 500
#define DUEL_TERRAIN_BOOST_UNIT \
    (DUEL_TERRAIN_STAT_ADJUSTMENT / CARD_STAT_SCALE)
#define DUEL_TERRAIN_PENALTY_UNIT (-DUEL_TERRAIN_BOOST_UNIT)

/* gDuel_bTerrain is a single byte at 0x8009B364; gFreeDuel_bReturnFlags
   follows at 0x8009B365. Every user, at -G0 and -G8 alike, reaches it with an
   absolute %hi/%lo pair in the assembler macro form, which is what the .data
   placement gives: Duel_GetTerrainBoost materializes its address twice, and
   DuelEffect_ApplyTerrain and AiScript_LoadTerrain take the load at the
   assembler's expansion point. */
extern u8 gDuel_bTerrain;

/* Attack modifier in CARD_STAT_SCALE units, one row per monster card type and
   one column per terrain. Duel_GetTerrainBoost rejects cardType >=
   CARD_TYPE_MAGIC before indexing, so only the monster types have rows.
   src/game/duel_scene.c owns the table at 0x800909D4. */
extern s8 gDuel_aTerrainBoost[CARD_TYPE_MAGIC][DUEL_TERRAIN_COUNT];

/* Returns the attack/defence adjustment the active terrain gives a card of
   `cardType`, already multiplied by CARD_STAT_SCALE, or 0 when there is no
   terrain or the card is not a monster. */
s32 Duel_GetTerrainBoost(s32 cardType);

struct DisplayObject;

/* Decides whether the attacking card trips the trap it is walking into,
 * comparing its attack against gDuel_abTrapAttackThresholds scaled by 100.
 * The input is the attacking DisplayObject; field_6A indexes its card record.
 * The threshold table is defined in this unit and names each entry. */
s32 Duel_SelectAttackTrap(const struct DisplayObject *record);

/* Historical call symbol retained by the unmatched battle candidate. */

/* Runs the trap presentation one step and reports whether it is still busy:
 * zero once the sequence has finished, non-zero while it is running or when
 * gDuel_wDisplayTransitionFlags blocks it.
 *
 * It takes NO arguments. duel_scene.c declared it `s32 (s32)` and
 * called it with a flag at both sites; the definition ignores whatever is in
 * that register. Dropping the argument was measured rather than assumed --
 * the executable is byte-identical with the calls spelled
 * `Duel_UpdateTrapPresentation()` -- so the declaration follows the
 * definition here.
 *
 * duel_action_lock.h describes the mode word it sequences on, `gDuel_wTrapPresentationMode &
 * 0xF`, and records that duel_scene.c clears that word to 0 at the two
 * points the effect finishes -- the same two sites that make these calls. */
s32 Duel_UpdateTrapPresentation(void);

/* Historical call symbol retained by the unmatched battle candidate. */

/* Updates the card-use confirmation UI: 0 while active, -1 for cancel,
 * 1 for confirmation at zero rotation, and 2 for confirmation at nonzero
 * rotation. The caller proceeds with card use only for 1 and restores the
 * card for either other completed result. */
s32 Duel_UpdateCardUseConfirmation(void);

/* A display object's update callback that slides the object 0x140 pixels to
 * the left. On the first frame it fixes the target x at its current output x
 * minus 0x140, takes its step from the phase value it was given (0x400 /
 * field_60), and starts the phase at -0x400. Each frame then eases it through
 * Widget_SlideSine. When the phase reaches zero it snaps to the target and
 * clears its update hook and field_6C. duel_scene.c installs it by
 * address. */
void Duel_UpdateDrawCardSlide(DisplayObject *object);

/* Starts a linear tween of the view state at gViewState_Current over `frames` frames.
 * It reads the four current channel values, records them and the targets, and
 * stores a 16.16 fixed-point per-frame delta for each; advancing the tween is
 * someone else's job.
 *
 * Channels are radial distance, the orbit's second angle, heading and the
 * reference target's Z coordinate. Inputs remain full words; targets keep
 * their original halfword storage. Heading interpolation stays linear,
 * with no added wrap or frame validation. */
void Duel_StartViewTween(
    s32 frames, s32 distance, s32 orbit_angle, s32 heading, s32 target_z);

/* Remaining frames in the view tween. The producer treats the count as
 * signed; Duel_InitScene only clears the same 16-bit slot and uses an unsigned
 * view. Both compile to the same gp-relative halfword access. */
extern s16 gDuel_wViewTweenFramesRemaining;

/* State that Duel_UpdateDisplayTransition (duel_scene.c) uses to advance
 * the tween.
 *
 * The four target halfwords are distance, orbit angle, heading and target Z. The four
 * accumulators hold their starting values in 16.16 form with the low half
 * biased by 0x8000, and the final four words are the per-frame deltas. */
extern s16 gDuel_wViewTweenDistanceTarget;
extern s16 gDuel_wViewTweenOrbitAngleTarget;
extern s16 gDuel_wViewTweenHeadingTarget;
extern s16 gDuel_wViewTweenReferenceZTarget;

extern s32 gDuel_dwViewTweenDistanceAccumulator;
extern s32 gDuel_dwViewTweenOrbitAngleAccumulator;
extern s32 gDuel_dwViewTweenHeadingAccumulator;
extern s32 gDuel_dwViewTweenReferenceZAccumulator;

extern s32 gDuel_dwViewTweenDistanceStep;
extern s32 gDuel_dwViewTweenOrbitAngleStep;
extern s32 gDuel_dwViewTweenHeadingStep;
extern s32 gDuel_dwViewTweenReferenceZStep;

/* Arms a duel: stores side0_controller into gDuel_bSide0ControllerSelector[0], opponent_id into gDuel_bOpponentID and
 * win_scene/loss_scene into the halfwords gDuel_awCampaignSceneByOutcome[0]/gDuel_awCampaignSceneByOutcome[1], resets the terrain and
 * sets gMain_bActiveModeState to 3.
 *
 * The parameters are s32 even though every store is a byte or a halfword.
 * Retail's Free Duel overlay passes a computed grid index as opponent_id without
 * narrowing it, and an s8 parameter makes that caller emit an sll/sra pair
 * retail does not have. The definition stores through sb/sh either way, and
 * the two resident callers pass constants, so neither side moves. */
void Main_StartDuel(s32 side0_controller, s32 opponent_id, s32 win_scene, s32 loss_scene);

/* Three-way comparator over two signed halfwords, returning 0, 1 or -1. Its
 * only consumer is the retained `Duel_RequestCombinedDeckData` candidate, which hands it to
 * qsort to order the combined deck's card ids.
 *
 * The prototype is spelled exactly as the definition writes it, const and
 * all. The local declaration it replaces dropped the const and named the
 * parameters s16 * rather than const short * -- the same types, since s16 is
 * short, but a weaker promise than the function actually keeps. Adopting the
 * definition's spelling is free here because the qsort call casts the
 * function to (int (*)()) anyway, which erases the prototype at the point of
 * use; nothing observed depends on the declaration's parameter types. */
int Util_CompareS16(const short *left, const short *right);

/* Applies the measured defaults to shared ViewState, projection and view.
 *
 * It writes the fixed distance, angle and projection values, clears the
 * rotation and the three coordinate fields, pushes the projection distance
 * into the GTE through GsSetProjection, and finishes with ViewState_ApplyOrbit.
 *
 * Duel_InitGroundProjectionReference (src/game/duel_scene.c) is the
 * observed consumer and calls this before and after its reference sample.
 * The second call returns the view to these defaults rather than restoring
 * an arbitrary prior camera state.
 *
 * The definition holds a second base pointer into the same structure, used
 * for three of the stores. That is load bearing rather than leftover: the
 * unit's own comment records that spelling those three through the first base
 * comes out one instruction short of retail. It should not be folded away
 * without a measurement.
 *
 * The neutral shared-state name does not assign exclusive screen ownership
 * or claim that every byte of a larger backing allocation is cleared. */
void ViewState_ResetDefaults(void);

/* Localized duel-result outro sprite tables, one opponent/no-opponent pair
 * per language. DuelScene_UpdateResultOutro selects the pair by the language
 * index gText_bLanguageIndex, whose boot/reset default each single-language build sets
 * through BUILD_LANGUAGE_INDEX (1 French, 2 German, 3 Italian, 4 Spanish);
 * the debug language editor can store any value 1-4 in any of these builds,
 * so a table is chosen by the selector value rather than by the build. The
 * table within the pair is chosen by gDuel_bOpponentID >= 0, as the North
 * American path
 * chooses gDuelResult_aSpriteSpecsOpponent/NoOpponent. Each is indexed by
 * the winning side, then by up to ten specs. All four localized images bind
 * all eight tables at the same addresses. */
extern DuelResultSpriteSpec gDuelResult_aFrenchSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aFrenchSpriteSpecsNoOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aGermanSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aGermanSpriteSpecsNoOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aItalianSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aItalianSpriteSpecsNoOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aSpanishSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aSpanishSpriteSpecsNoOpponent[2][10];
extern s32 gDuelResult_nOutroSpriteCount;
#endif
