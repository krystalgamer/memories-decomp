/* duel_grid.h leaves D_800907D8 unsized for its consumers; the definition
   states the shape the tables actually have. */
#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "duel_grid.h"
#include "card_constants.h"

/* Initialized data at 0x800907AC: the duel's layout tables, in image order.
   The first is the cursor step per side and page that func_80023D08
   subtracts, the second maps a side's five hand slots to their card-record
   entries, and the last two are the field grid.

   The two field tables are walked together: one maps a side's twenty grid
   slots to their card-record entries, the other is the screen position of
   each of those slots. Every table here is mirrored between the sides. */
s16 D_800907AC[DUEL_SIDE_COUNT][2][4] = {
    {
        {  137,    71,  -119,  -184},
        {  136,   136,  -120,  -120},
    },
    {
        { -137,   -71,   119,   184},
        { -136,  -136,   120,   120},
    },
};

u8 D_800907CC[DUEL_SIDE_COUNT * HAND_SIZE + 2] = {
    0, 1, 2, 3, 4,
    15, 16, 17, 18, 19,
    0, 0,
};

u8 D_800907D8[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT] = {
    {29, 28, 27, 26, 25, 24, 23, 22, 21, 20,
     5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
    {14, 13, 12, 11, 10, 9, 8, 7, 6, 5,
     20, 21, 22, 23, 24, 25, 26, 27, 28, 29},
};

DuelFieldPosition
    D_80090800[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT] = {
    {
        {-140,  161}, { -70,  161}, {   0,  161}, {  70,  161}, { 140,  161},
        {-140,   95}, { -70,   95}, {   0,   95}, {  70,   95}, { 140,   95},
        {-140,  -95}, { -70,  -95}, {   0,  -95}, {  70,  -95}, { 140,  -95},
        {-140, -161}, { -70, -161}, {   0, -161}, {  70, -161}, { 140, -161},
    },
    {
        { 140, -161}, {  70, -161}, {   0, -161}, { -70, -161}, {-140, -161},
        { 140,  -95}, {  70,  -95}, {   0,  -95}, { -70,  -95}, {-140,  -95},
        { 140,   95}, {  70,   95}, {   0,   95}, { -70,   95}, {-140,   95},
        { 140,  161}, {  70,  161}, {   0,  161}, { -70,  161}, {-140,  161},
    },
};
