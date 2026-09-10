#ifndef MEMORIES_DECOMP_DUEL_FIELD_DISPLAY_OBJECTS_H
#define MEMORIES_DECOMP_DUEL_FIELD_DISPLAY_OBJECTS_H

#include "../types.h"
#include "display_object.h"

/* One field-grid source record, as the three functions in
 * duel_field_display_objects.c reach it. The two sources that unit was split
 * across described it twice and disagreed in three places; matched loads and
 * stores settle all three.
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
 * cursor in $a0 for func_8002348C's settling-move call. */
typedef struct {
    DisplayObject *field_00;
    DisplayObject *object;
    u8 pad_08[7];
    s8 x;
    s8 y;
    u8 pad_11[3];
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

/* The settling-move caller supplies no expression: it deliberately leaves
 * its GridCursor-backed record in $a0. */
#ifdef FUNC_8002348C_AMBIENT_SOURCE
void func_8002348C(void);
#else
void func_8002348C(DuelFieldDisplaySource *source);
#endif

#endif
