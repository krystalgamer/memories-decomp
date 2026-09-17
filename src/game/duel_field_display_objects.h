#ifndef MEMORIES_DECOMP_DUEL_FIELD_DISPLAY_OBJECTS_H
#define MEMORIES_DECOMP_DUEL_FIELD_DISPLAY_OBJECTS_H

#include "../types.h"
#include "display_object.h"
#include "display_parent_links.h"
#include "duel_grid.h"

/* One field-grid source record, as func_80023144, func_8002348C and
 * func_800234E4 in duel_field_display_objects.c reach it. The two sources
 * that unit was split across described it twice and disagreed in three
 * places; matched loads and stores settle all three.
 *
 * The DisplayObject pointers at 0x00 and 0x04 are distinct. func_800234E4
 * creates an object and stores it at 0x04; func_80023144 loads 0x00 to place
 * its text box. Whether they ever hold the same object is not established.
 *
 * table_index at 0x17 formerly had two names. func_800234E4 adds it to the
 * side offset to select a graphic, while func_80023144 compares it with 2 and
 * 3 to select a text-box style.
 *
 * The remaining former disagreement was func_80023144's index type. Its
 * definition takes s32; its caller supplies a u8 that promotes to that type.
 *
 * The byte pair at 0x0F/0x10 is also GridCursor's col/row view of the same
 * backing record. func_80023D08 relies on that overlap when it leaves the
 * cursor in $a0 for func_8002348C's settling-move call.
 *
 * func_800235C0 reads the rest: the DisplayLinkEntry array at 0x08 (the
 * same member display_parent_links.h's DisplayParent view names entries,
 * which func_800235C0 passes to DuelSelection_LinkDisplayObjects), the
 * halfword at 0x0C it stores as the panel's y, and the byte at 0x13 it applies with
 * DisplayObject_SetResourceVariant. */
typedef struct {
    DisplayObject *field_00;
    DisplayObject *object;
    DisplayLinkEntry *entries;
    s16 field_0C;
    u8 pad_0E[1];
    s8 x;
    s8 y;
    u8 pad_11[2];
    u8 field_13;
    u8 field_14;
    u8 pad_15[1];
    s8 field_16;
    u8 table_index;
    u8 field_18;
} DuelFieldDisplaySource;

#define DUEL_FIELD_DISPLAY_SOURCE_OFFSET(member) \
    ((u32)&(((DuelFieldDisplaySource *)0)->member))

typedef char DuelFieldDisplaySource_x_offset_must_be_0x0F[
    DUEL_FIELD_DISPLAY_SOURCE_OFFSET(x) == 0x0F ? 1 : -1
];
typedef char DuelFieldDisplaySource_y_offset_must_be_0x10[
    DUEL_FIELD_DISPLAY_SOURCE_OFFSET(y) == 0x10 ? 1 : -1
];
typedef char DuelFieldDisplaySource_table_index_offset_must_be_0x17[
    DUEL_FIELD_DISPLAY_SOURCE_OFFSET(table_index) == 0x17 ? 1 : -1
];

#undef DUEL_FIELD_DISPLAY_SOURCE_OFFSET

/* Compares the guardian-star matchup between two field cursors and returns
 * display code 4 for neutral, 1 when the first loses, or 6 when it wins.
 * The builder casts its overlapping source record and selection-state bytes
 * to this cursor view at the call site. */
s32 func_80023090(DuelFieldCursor *cursor_a, DuelFieldCursor *cursor_b);

/* The settling-move caller supplies no expression: it deliberately leaves
 * its GridCursor-backed record in $a0. */
#ifdef FUNC_8002348C_AMBIENT_SOURCE
void func_8002348C(void);
#else
void func_8002348C(DuelFieldDisplaySource *source);
#endif

/* Builds the field-card text box for card record `index`; defined in
 * duel_field_display_objects.c and called by func_8002348C there. */
void func_80023144(DuelFieldDisplaySource *source, s32 index);

/* Creates the field-card panel object and stores it at the record's 0x04,
 * selecting its graphic from table_index plus the side offset -- the two
 * behaviours the record comment above already describes. Defined in
 * duel_field_display_objects.c, which calls it at :423, and called once more
 * from outside the unit by DuelScene_UpdateFieldActions
 * (src/candidates/func_8001D670.c:354).
 *
 * That candidate declared this prototype for itself while already including
 * this header, which is the only reason the declaration was not here: the
 * unit's other four entry points are declared above, and the comment on the
 * record names this one three times. The spelling is the definition's own,
 * unchanged. */
void func_800234E4(DuelFieldDisplaySource *source);

/* Steps the field-card panel for the D_8009B162 request flags: on the first
 * frame it starts the camera tween and the panel's slide in or out, then it
 * advances the cursor, the panel and the camera, and clears the flags once
 * nothing is moving. Returns nonzero while the step is still in progress;
 * its one caller ignores the result. */
s32 func_800235C0(void);

#endif
