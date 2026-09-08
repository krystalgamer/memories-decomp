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

#endif
