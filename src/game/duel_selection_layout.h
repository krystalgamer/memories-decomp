#ifndef MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H
#define MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H

#include "../types.h"
#include "duel_card_pick_cursor.h"
#include "duel_hand.h"

#define DUEL_SELECTION_RECORD_SIZE 0x1C
#define DUEL_SELECTION_RECORDS_PER_SIDE 4
#define DUEL_SELECTION_SIDE_SIZE \
    (DUEL_SELECTION_RECORDS_PER_SIDE * DUEL_SELECTION_RECORD_SIZE)

/* Prefix view of one side's selection records. The hand pointer at +8 is
 * written by both duel phase entries; the remaining side record stays byte
 * addressed because its wider layout is not established. */
typedef struct {
    u8 pad_00[8];
    DuelHandSlot *hand;
} DuelSelectionSideView;

typedef char DuelSelectionSideView_hand_offset_must_be_8[
    (u32)&(((DuelSelectionSideView *)0)->hand) == 8 ? 1 : -1
];
typedef char DuelSelectionSideView_size_must_be_0xC[
    sizeof(DuelSelectionSideView) == 0xC ? 1 : -1
];

/* One whole selection record, DUEL_SELECTION_RECORD_SIZE bytes, four to a
 * side. Duel_InitSelectionRecords resets every field named here, which is what fixes
 * their widths: three words at the front -- the third being the same hand
 * pointer DuelSelectionSideView views at +8 -- and the four bytes at 0x13,
 * 0x14, 0x17 and 0x18.
 *
 * DuelCardPickCursor covers the same record from 0xC to 0x19 and stops at
 * 0x1A so that it keeps alignment 1; this one starts at the front, so it
 * carries the hand pointer and the record's real stride instead. */
typedef struct {
    u32 field_00;
    /* The cursor sprite DuelScene_UpdateHandActions creates for the side and
     * flags through its halfword at +8; DuelScene_UpdateExodiaResult releases
     * it. */
    struct DisplayObject *cursor_object;
    DuelHandSlot *hand;
    /* The cursor step counter DuelCardPickCursor names field_0C. */
    s16 field_0C;
    /* The hand slot the side's cursor is on: func_8001B8B8 indexes
     * D_800EA030 with it to pick the card it leaves undimmed, and
     * DuelScene_UpdateHandActions clears it beside field_15. Read signed at
     * that index. */
    u8 field_0E;
    /* The grid column and row, as DuelFieldCursor and DuelCardPickCursor
     * name them; DuelScene_UpdateFieldActions reads them through this record. */
    s8 col;
    s8 row;
    u8 field_11;
    u8 field_12;
    u8 field_13;
    u8 field_14;
    /* Zero while the side is picking from its hand, which is the condition
     * func_8001B8B8 dims every other card under. */
    u8 field_15;
    u8 pad_16;
    u8 field_17;
    u8 field_18;
    /* DuelCardPickCursor's status byte. */
    u8 status;
    u8 pad_1A[2];
} DuelSelectionRecord;

typedef char DuelSelectionRecord_hand_offset_must_be_8[
    (u32)&(((DuelSelectionRecord *)0)->hand) == 8 ? 1 : -1
];
typedef char DuelSelectionRecord_field_0E_offset_must_be_0xE[
    (u32)&(((DuelSelectionRecord *)0)->field_0E) == 0xE ? 1 : -1
];
typedef char DuelSelectionRecord_field_15_offset_must_be_0x15[
    (u32)&(((DuelSelectionRecord *)0)->field_15) == 0x15 ? 1 : -1
];
typedef char DuelSelectionRecord_field_13_offset_must_be_0x13[
    (u32)&(((DuelSelectionRecord *)0)->field_13) == 0x13 ? 1 : -1
];
typedef char DuelSelectionRecord_field_18_offset_must_be_0x18[
    (u32)&(((DuelSelectionRecord *)0)->field_18) == 0x18 ? 1 : -1
];
typedef char DuelSelectionRecord_size_must_be_record_size[
    sizeof(DuelSelectionRecord) == DUEL_SELECTION_RECORD_SIZE ? 1 : -1
];

/* One side of the selection table with record 2 seen as its pick cursor.
 * DuelScene_UpdateHandActions indexes D_800E9F10 through this view so that
 * the record's 0x38 stays in the load displacement (`lb 0x48` / `lb 0x47`)
 * instead of folding into the symbol, which pointer arithmetic does. */
typedef struct {
    u8 pad_00[2 * DUEL_SELECTION_RECORD_SIZE];
    DuelCardPickCursor field;
    u8 pad_52[DUEL_SELECTION_SIDE_SIZE - 2 * DUEL_SELECTION_RECORD_SIZE -
              sizeof(DuelCardPickCursor)];
} DuelSelectionSideCursors;

typedef char DuelSelectionSideCursors_size_must_be_side_size[
    sizeof(DuelSelectionSideCursors) == DUEL_SELECTION_SIDE_SIZE ? 1 : -1
];

/* One side of the table as its four records, for the unit that reaches
 * record 2 and record 3 of the current side through the same base:
 * DuelScene_UpdateFieldActions keeps the 0x38 and 0x54 record offsets in the
 * load displacements this way. */
typedef struct {
    DuelSelectionRecord records[DUEL_SELECTION_RECORDS_PER_SIDE];
} DuelSelectionSide;

typedef char DuelSelectionSide_size_must_be_side_size[
    sizeof(DuelSelectionSide) == DUEL_SELECTION_SIDE_SIZE ? 1 : -1
];

/* The selection table itself: DUEL_SELECTION_SIDE_SIZE bytes per side. */
extern u8 D_800E9F10[];

/* A name for record 1 of that same table, 0x1C bytes in -- exactly
 * DUEL_SELECTION_RECORD_SIZE -- so it is record 1 of side 0, and because it
 * is indexed with the same per-side stride it is record 1 of whichever side
 * is selected:
 *
 *     D_8009B1B4 = (DuelCardPickCursor *)(D_800E9F2C +
 *         D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE);
 *
 * DuelScene_UpdateHandActions (duel_scene_hand_actions.c) is the only source
 * in the tree that mentions it; the plain pointer sum already adds the base
 * to the index, which is retail's operand order.
 *
 * Like the other names into this table it keeps its own relocations -- one
 * %hi/%lo pair in that function, and no access is gp-relative -- so this is
 * the plain array declaration. */
extern u8 D_800E9F2C[];

/* A second name for the inside of that same table, 0x38 bytes in --
 * exactly 2 * DUEL_SELECTION_RECORD_SIZE, so it is record 2 of side 0, and
 * because it is indexed with the same per-side stride it is record 2 of
 * whichever side is selected:
 *
 *     D_8009B1B4 = D_800E9F48 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE;
 *
 * which is the same cursor assignment the D_800E9F10 callers make, just from
 * two records further in. func_80023144 (duel_field_display_objects.c)
 * passes the same expression to func_80023090.
 *
 * The overlap is retail's, not an artefact of the port, and the two names
 * have to stay separate: DuelScene_UpdateHandActions and
 * DuelScene_UpdateFieldActions each take the
 * address of both symbols, and each carries its own
 * `addiu $v1, $v1, %lo(D_800E9F48)` relocation. Folding this into an offset
 * from D_800E9F10 would change which symbol those relocations name. */
extern u8 D_800E9F48[];

/* A third name for the same table, 0x54 bytes in -- exactly
 * 3 * DUEL_SELECTION_RECORD_SIZE, so it is record 3 of side 0, and because
 * both users index it with the per-side stride it is record 3 of whichever
 * side is selected:
 *
 *     D_800E9F64 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE
 *
 * DuelScene_UpdateBattle assigns exactly that to D_8009B1B4
 * (src/game/duel_scene_battle.c) and DuelScene_UpdateFieldActions forms
 * it six times, once reaching back 0x18 into record 2
 * (src/game/duel_scene_field_actions.c). No
 * other source in the tree mentions the symbol.
 *
 * Like D_800E9F48 above, this name has to stay separate rather than become an
 * offset from D_800E9F10: each listing carries its own relocations for it --
 * five %hi/%lo pairs in DuelScene_UpdateFieldActions and one in DuelScene_UpdateBattle -- and
 * folding it would change which symbol they name. No access is gp-relative,
 * so this is the plain array declaration. */
extern u8 D_800E9F64[];

/* Assigned from both bases above. Assigned by three C functions and loaded
 * by three: DuelScene_UpdateStartup assigns `D_800E9F10 + D_8009B1D5 *
 * DUEL_SELECTION_SIDE_SIZE` and stores 0xAE at +0xC (duel_phase_entry.c:
 * 69-70); DuelScene_UpdateDrawPhase assigns the same from `side` and stores `base` at
 * +8 (:154-156), then assigns it again and stores 0xAE at +0xC (:206-207);
 * func_8001B938 assigns `D_800E9F48 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE`
 * and stores a halfword at +0xC and bytes at +0x11, +0x12, +0x13, +0x18 and
 * +0x19 (func_8001B938.c:37-42), +0x11 and +0x12 again (:53-54), +0x10
 * (:59) and +0xF (:68), loading it back for each; Duel_GetCardViewerRequestId loads it
 * directly as a DuelCardPickCursor (src/game/duel_selected_card_checks.c); and
 * func_800235C0 loads it into the DuelFieldDisplaySource view that
 * duel_field_display_objects.c already casts the same record to.
 * DuelScene_UpdateHandActions (duel_scene_hand_actions.c) stores it six times
 * and loads it five. Four functions still in assembly, none with a profile in
 * matching_c.json, also store or load it: DuelScene_UpdateCardPlacement.s:32,
 * DuelScene_UpdateFieldActions
 * (stores :56, :613, :753, :1409,
 * :1439; load :1412), DuelScene_UpdateBattle.s (stores :93, :183; load :96) and
 * func_800235C0.
 *
 * DuelCardPickCursor * because that view does cover every store. This
 * note used to say the byte view was the only one that fits, on the
 * grounds that DuelFieldCursor is 0x11 bytes while func_8001B938 writes
 * +0x19. That is true of DuelFieldCursor and false of the record as a
 * whole: DuelCardPickCursor is 0x1A bytes and ends at 0x19 exactly, and
 * duel_card_pick_cursor.h already recorded that its col, row and status
 * agree with the narrower views. Duel_GetCardViewerRequestId now uses this owning type
 * without a cast.
 *
 * The store at +8 stays a cast: it writes a DuelHandSlot *, and giving
 * the record a pointer member there would raise its alignment to 4 and
 * its size to 0x1C, which the asserted 0x1A forbids. Every access in the binary is a %gp_rel lw or sw
 * (Duel_GetCardViewerRequestId.s:6, DuelScene_UpdateStartup.s:94, DuelScene_UpdateDrawPhase.s:38 and :164,
 * func_8001B938.s:12 and its nine loads), so this is the plain declaration.
 * Four bytes at 0x8009B1B4, with D_8009B1B8 at +4 (c_symbols.ld:133-134).
 * Initial value not read. */
extern DuelCardPickCursor *D_8009B1B4;

/* Resets that table. For both sides it walks the four records, zeroing the
 * first three words and the byte at 0x18, then writing 1 to 0x13, the record
 * index to 0x17, and to 0x14 the index again except for record 3, which gets
 * 1. Afterwards it clears 0x13 on the first record of each side, so the value
 * written in the loop survives only for records 1 to 3.
 *
 * Declared here because this header owns everything it touches: D_800E9F10
 * above, DuelSelectionRecord, which the walk now names those fields through,
 * and the three size constants it is written in terms of.
 * Duel_InitScene (src/game/duel_init_scene.c) is the only caller, and
 * its old duel_init_scene.c held the only declaration. */
void Duel_InitSelectionRecords(void);

#endif
