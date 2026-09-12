#include "../types.h"
#include "duel_result_outro.h"
#include "duel_screen_tables.h"

/* Initialized data at 0x800908A0 through 0x80090998, previously a generated
   blob (#2602).

   D_800908A0 is the thirty coordinate pairs func_800177C4 projects through
   the GTE, one per card slot, writing the biased result to D_800EA070. The
   pairs form a five-column grid at x = -140, -70, 0, 70 and 140.
   debug_effect_screen.c also reads the pair at index 0xC/0xD on its own.

   It stays u16 because its u16 readers declare it that way. The coordinates
   are signed, so the values are written as hex rather than as decimals the
   declaration would misrepresent.

   D_80090918 supplies the five Exodia card poses consumed by the retained
   func_80018FEC candidate. Each card ID from 0x11 through 0x15 selects a
   record, whose first byte chooses one of the five shared display-object work
   slots and whose remaining bytes become object x/y after removing the
   candidate's 0x1A/0x1E coordinate biases. The trailing byte preserves the
   table's 16-byte extent before D_80090928.

   D_80090928 and D_80090960 are the duel-result sprite tables, for a real
   opponent and for none. The outro already declared both as
   DuelResultSpriteSpec[][DUEL_RESULT_SPRITE_COUNT], and the extracted sizes
   agree: two winning sides by seven sprites by a four-byte spec. */
u16 D_800908A0[] = {
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0xFF74, 0xFFA1,
    0xFFBA, 0xFFA1,
    0x0000, 0xFFA1,
    0x0046, 0xFFA1,
    0x008C, 0xFFA1,
    0xFF74, 0xFF5F,
    0xFFBA, 0xFF5F,
    0x0000, 0xFF5F,
    0x0046, 0xFF5F,
    0x008C, 0xFF5F,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x0000, 0x0000,
    0x008C, 0x005F,
    0x0046, 0x005F,
    0x0000, 0x005F,
    0xFFBA, 0x005F,
    0xFF74, 0x005F,
    0x008C, 0x00A1,
    0x0046, 0x00A1,
    0x0000, 0x00A1,
    0xFFBA, 0x00A1,
    0xFF74, 0x00A1,
};

DuelExodiaCardPoseTable D_80090918 = {
    {
        { 3, 104, 206 },
        { 2, 216, 206 },
        { 4, 69, 98 },
        { 1, 251, 98 },
        { 0, 160, 32 },
    },
    0,
};

DuelResultSpriteSpec D_80090928[][DUEL_RESULT_SPRITE_COUNT] = {
    {
        { 112, 68, 1, 20 },
        { 160, 68, 2, 20 },
        { 208, 68, 3, 20 },
        { 88, 124, 4, 20 },
        { 136, 124, 5, 8 },
        { 184, 124, 6, 20 },
        { 232, 124, 7, 8 },
    },
    {
        { 112, 68, 8, 20 },
        { 160, 68, 9, 20 },
        { 208, 68, 10, 20 },
        { 88, 124, 11, 20 },
        { 136, 124, 12, 8 },
        { 184, 124, 13, 20 },
        { 232, 124, 14, 8 },
    },
};

DuelResultSpriteSpec D_80090960[][DUEL_RESULT_SPRITE_COUNT] = {
    {
        { 136, 68, 16, 12 },
        { 184, 68, 17, 20 },
        { 0, 0, 0, 20 },
        { 88, 124, 4, 20 },
        { 136, 124, 5, 8 },
        { 184, 124, 6, 20 },
        { 232, 124, 7, 8 },
    },
    {
        { 136, 68, 24, 12 },
        { 184, 68, 25, 20 },
        { 0, 0, 0, 20 },
        { 88, 124, 26, 20 },
        { 136, 124, 27, 8 },
        { 184, 124, 28, 20 },
        { 232, 124, 29, 8 },
    },
};
