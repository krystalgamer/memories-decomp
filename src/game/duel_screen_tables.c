#define DUEL_SCREEN_TABLES_TYPED_POSITIONS
#include "../types.h"
#include "duel_result_outro.h"
#include "duel_screen_tables.h"

/* Initialized data at 0x800908A0 through 0x80090998, previously a generated
   blob (#2602).

   D_800908A0 is the thirty coordinate pairs func_800177C4 projects through
   the GTE, one per card slot, writing the biased result to D_800EA070. The
   pairs form a five-column grid at x = -140, -70, 0, 70 and 140.
   debug_effect_screen.c also reads the pair at index 0xC/0xD on its own.

   The owning source records those signed pairs as DuelFieldPosition entries.
   The shared header preserves a flat u16 view for the projection candidate
   and debug screen because their exact code generation depends on halfword
   pointer arithmetic.

   D_80090918 has no reader in tracked C and no shape anything supports. It
   is transcribed as bytes and left unnamed.

   D_80090928 and D_80090960 are the duel-result sprite tables, for a real
   opponent and for none. The outro already declared both as
   DuelResultSpriteSpec[][DUEL_RESULT_SPRITE_COUNT], and the extracted sizes
   agree: two winning sides by seven sprites by a four-byte spec. */
DuelFieldPosition D_800908A0[DUEL_SCREEN_CARD_POSITION_COUNT] = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { -140, -95 },
    { -70, -95 },
    { 0, -95 },
    { 70, -95 },
    { 140, -95 },
    { -140, -161 },
    { -70, -161 },
    { 0, -161 },
    { 70, -161 },
    { 140, -161 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 140, 95 },
    { 70, 95 },
    { 0, 95 },
    { -70, 95 },
    { -140, 95 },
    { 140, 161 },
    { 70, 161 },
    { 0, 161 },
    { -70, 161 },
    { -140, 161 },
};

/* No reader in tracked C. */
u8 D_80090918[] = {
    0x03, 0x68, 0xCE, 0x02, 0xD8, 0xCE, 0x04, 0x45,
    0x62, 0x01, 0xFB, 0x62, 0x00, 0xA0, 0x20, 0x00,
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
