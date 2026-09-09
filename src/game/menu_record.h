#ifndef MEMORIES_DECOMP_MENU_RECORD_H
#define MEMORIES_DECOMP_MENU_RECORD_H

#include "../types.h"

/* The three 0x4C-byte records at D_800EB010. Every accessor agrees on the
 * stride: func_80038EB0 indexes `D_800EB010 + slot * 0x4C`, func_8002EB78
 * selects a record with `((bit >> 7) * 0x4C)`, and func_80039E9C and
 * DuelEffect_MarkObjectIfActive both walk indices 2..0.
 *
 * field_30 is the signed marker the walkers test for `>= 0`;
 * display_effect_step selects the D_80090F68 lifecycle handler; field_34 is
 * the halfword func_8002EB78 seeds with 0xD8 and
 * DuelEffect_MarkObjectIfActive later ORs bit 1 into.
 */
typedef struct {
    s32 grid[4][3];
    s8 field_30;
    u8 pad_31;
    u8 field_32;
    u8 display_effect_step;
    u16 field_34;
    u8 pad_36[4];
    u8 field_3A;
    u8 field_3B;
    u8 pad_3C[0x10];
} MenuRecord;

#define MENU_RECORD_COUNT 3

extern MenuRecord D_800EB010[];

#endif
