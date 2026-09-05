#ifndef MEMORIES_DECOMP_DUEL_GRID_H
#define MEMORIES_DECOMP_DUEL_GRID_H

#include "../types.h"

#define DUEL_FIELD_ROW_SIZE 5
#define DUEL_FIELD_SIDE_ZONE_COUNT 10
#define DUEL_FIELD_SIDE_GRID_SLOT_COUNT 20
#define DUEL_FIELD_SIDE_POSITION_BYTES \
    (DUEL_FIELD_SIDE_GRID_SLOT_COUNT * sizeof(DuelFieldPosition))

typedef struct {
    u16 x;
    u16 y;
} DuelFieldPosition;

#endif
