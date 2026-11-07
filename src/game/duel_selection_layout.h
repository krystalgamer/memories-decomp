#ifndef MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H
#define MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H

#include "../types.h"
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
 * Cursor input/status APIs use this actual record and its real stride. */
typedef struct DuelSelectionRecord {
    /* Position base for linked display objects; the field-display path
     * stores its sliding information panel here. */
    struct DisplayObject *field_00;
    /* The cursor sprite DuelScene_UpdateHandActions creates for the side and
     * flags through its halfword at +8; DuelScene_UpdateExodiaResult releases
     * it. */
    struct DisplayObject *cursor_object;
    DuelHandSlot *hand;
    /* Cursor step counter or panel position, according to record context. */
    s16 field_0C;
    /* The hand slot the side's cursor is on: DuelHand_RefreshSelectionColors indexes
     * gDuelHand_aSlots with it to pick the card it leaves undimmed, and
     * DuelScene_UpdateHandActions clears it beside field_15. Read signed at
     * that index. */
    u8 hand_cursor_slot;
    /* Signed grid coordinates, also consumed by the input/status APIs. */
    s8 col;
    s8 row;
    /* Allowed rows are [row_min, row_end) through the existing signed views. */
    u8 row_min;
    u8 row_end;
    u8 field_13;
    /* Text-box channel of the record's information panel: TextBox_Create,
     * TextBox_SetPos and TextBox_Destroy all select it through
     * DUEL_FIELD_DISPLAY_CHANNEL. Japanese keeps the record index; North
     * America maps index 3 to 1. */
    u8 text_channel;
    /* Ordered-pick count in the hand-selection record. The color helper
     * restores the cursor card only when this is zero; other record
     * contexts keep their existing interpretation. */
    u8 field_15;
    s8 text_y_offset;
    /* Ordinal within the side's four records; also tags linked objects and
     * selects the field-cursor graphic and information-box mode. */
    u8 record_index;
    u8 field_18;
    /* Cursor input/status byte. */
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
 * Duel_InitScene (src/game/duel_init_scene.c) is the only caller, and
 * its old duel_init_scene.c held the only declaration. */
void Duel_InitSelectionRecords(void);

#endif
