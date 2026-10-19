#ifndef MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H
#define MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H

#include "../types.h"
#include "duel_card_pick_cursor.h"
#include "duel_hand.h"

#define DUEL_SELECTION_RECORD_SIZE 0x1C
#define DUEL_SELECTION_RECORDS_PER_SIDE 4
#define DUEL_SELECTION_SIDE_SIZE \
    (DUEL_SELECTION_RECORDS_PER_SIDE * DUEL_SELECTION_RECORD_SIZE)

/* One whole selection record, DUEL_SELECTION_RECORD_SIZE bytes, four to a
 * side. Duel_InitSelectionRecords resets the three leading words and bytes
 * at 0x13, 0x14, 0x17 and 0x18. Cursor and mode code supply the other named
 * fields; this is not a claim that initialization clears the whole record.
 *
 * DuelCardPickCursor covers the same record from 0xC to 0x19 and stops at
 * 0x1A with halfword alignment; this one starts at the front, so it
 * carries the hand pointer and the record's real stride instead. */
typedef struct {
    /* Position base for linked display objects; the field-display path
     * stores its sliding information panel here. */
    struct DisplayObject *G32 field_00;
    /* The cursor sprite DuelScene_UpdateHandActions creates for the side and
     * flags through its halfword at +8; DuelScene_UpdateExodiaResult releases
     * it. */
    struct DisplayObject *G32 cursor_object;
    DuelHandSlot *G32 hand;
    /* The cursor step counter DuelCardPickCursor names field_0C. */
    s16 field_0C;
    /* The hand slot the side's cursor is on: DuelHand_RefreshSelectionColors indexes
     * gDuelHand_aSlots with it to pick the card it leaves undimmed, and
     * DuelScene_UpdateHandActions clears it beside field_15. Read signed at
     * that index. */
    u8 field_0E;
    /* The grid column and row, as DuelFieldCursor and DuelCardPickCursor
     * name them; DuelScene_UpdateFieldActions reads them through this record. */
    s8 col;
    s8 row;
    /* Allowed rows are [row_min, row_end) through the existing signed views. */
    u8 row_min;
    u8 row_end;
    u8 field_13;
    /* Japanese keeps the record index; North America maps index 3 to 1. */
    u8 field_14;
    /* Ordered-pick count in the hand-selection record. The color helper
     * restores the cursor card only when this is zero; other record
     * contexts keep their existing interpretation. */
    u8 field_15;
    u8 pad_16;
    u8 field_17;
    u8 field_18;
    /* DuelCardPickCursor's status byte. */
    u8 status;
    u8 pad_1A[2];
} DuelSelectionRecord;

typedef char DuelSelectionRecord_field_00_offset_must_be_0[
    (u32)&(((DuelSelectionRecord *)0)->field_00) == 0 ? 1 : -1
];
typedef char DuelSelectionRecord_cursor_object_offset_must_be_4[
    (u32)&(((DuelSelectionRecord *)0)->cursor_object) == 4 ? 1 : -1
];
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
 *     gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(D_800E9F2C +
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
 *     gDuel_pActiveSelectionRecord = D_800E9F48 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE;
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
 * DuelScene_UpdateBattle assigns exactly that to gDuel_pActiveSelectionRecord
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

/* Active record in the selection table. Every writer selects a whole
 * DuelSelectionRecord through one of the four table symbols above, retaining
 * its original side-stride arithmetic and relocations. Phase entry writes
 * the hand pointer at +8; battle and Exodia paths access cursor_object at +4;
 * the cursor/viewer paths read the shared coordinates and status.
 *
 * DuelCardPickCursor remains a separate checked 0x1A API view. Its size
 * is not the table stride, and the call that needs it takes an explicit view.
 * The field-display consumer likewise keeps its context-specific view.
 *
 * The global itself is still one four-byte pointer, accessed gp-relative;
 * D_8009B1B8 follows at +4. No new initial-value or nullability claim is made. */
extern DuelSelectionRecord *G32 gDuel_pActiveSelectionRecord;

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
