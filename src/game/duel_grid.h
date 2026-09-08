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

#endif
