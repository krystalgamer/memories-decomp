/* duel_grid.h leaves D_800907D8 unsized for its consumers; the definition
   states the shape the tables actually have. */
#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "duel_grid.h"

/* Initialized data at 0x800907D8: the two field-layout tables the duel code
   walks together. The first maps a side's twenty grid slots to their entries
   in the card record array; the second is the screen position of each of
   those slots. Both are mirrored between the two sides. */
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
