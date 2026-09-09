#ifndef MEMORIES_DECOMP_DUEL_GRID_H
#define MEMORIES_DECOMP_DUEL_GRID_H

#include "../types.h"

#define DUEL_SIDE_COUNT 2
#define DUEL_FIELD_ROW_SIZE 5
#define DUEL_FIELD_SIDE_ZONE_COUNT 10
#define DUEL_FIELD_SIDE_GRID_SLOT_COUNT 20

/* Screen coordinates, signed: func_80023D08 subtracts these to derive a
 * cursor step and the result must be able to go negative. */
typedef struct {
    s16 x;
    s16 y;
} DuelFieldPosition;

#define DUEL_FIELD_SIDE_POSITION_BYTES \
    (DUEL_FIELD_SIDE_GRID_SLOT_COUNT * sizeof(DuelFieldPosition))

/* The grid cursor's two coordinate bytes, at 0x0F and 0x10 of a record whose
 * earlier fields are not established here.
 *
 * They are a column and a row rather than a pixel pair, because
 * func_80023090 turns them into a grid index as
 * `cursor->row * DUEL_FIELD_ROW_SIZE + cursor->col`. func_8001D5B4 spelled
 * the same two bytes x and y, which is the same claim in weaker terms.
 *
 * This describes only those two bytes. duel_update_card_pick_cursor.c holds
 * a longer view of the same record reaching 0x19, and the two do not overlap
 * in what they name, so they stay separate. */
typedef struct {
    u8 pad_00[0xF];
    s8 col;
    s8 row;
} DuelFieldCursor;

/* The field grid: DUEL_SIDE_COUNT blocks of DUEL_FIELD_SIDE_GRID_SLOT_COUNT
 * bytes, each byte a D_801A7AD8 record index. The shape is not a guess --
 * func_800179F4.c and func_800208D4.c both build a per-side cursor as
 * D_800907D8 + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT.
 *
 * Left unsized on purpose: a declared size is a -G input for this toolchain,
 * and no consumer needs the bound. */
#ifdef DUEL_FIELD_GRID_2D
/* duel_field_effect_transition.c and func_80025F3C.c index it [side][slot].
 * That is not a spelling preference: rewriting either of them to the flat
 * index the other sixteen files use builds to the right size and differs
 * from byte 0x80025CA2. */
extern u8 D_800907D8[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
#else
extern u8 D_800907D8[];
#endif

/* The screen position of every field slot, defined with initialisers in
 * duel_field_layout.c as a two-dimensional table. Six sources declared it
 * locally and only two of them had the shape right: the other four spelled
 * it a flat DuelFieldPosition[] and then cast it to u8 * anyway, which is
 * why the wrong shape never showed. Declared here in its real form, the
 * cast still reaches the same address and the flat spelling is not needed.
 * 160 bytes, far above the -G threshold, so completing the type is free. */
extern DuelFieldPosition
    D_80090800[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];

#endif
