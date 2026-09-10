#ifndef MEMORIES_DECOMP_MENU_RECORD_H
#define MEMORIES_DECOMP_MENU_RECORD_H

#include "../types.h"

/* The three 0x4C-byte records at D_800EB010. Every accessor agrees on the
 * stride: Text_HandleDisplayEffectCommand indexes
 * `D_800EB010 + slot * 0x4C`, func_8002EB78
 * selects a record with `((bit >> 7) * 0x4C)`, and func_80039E9C and
 * DuelEffect_MarkObjectIfActive both walk indices 2..0.
 *
 * field_30 is the signed marker the walkers test for `>= 0`;
 * display_effect_step selects the D_80090F68 lifecycle handler; field_34 is
 * the halfword func_8002EB78 seeds with 0xD8 and
 * DuelEffect_MarkObjectIfActive later ORs bit 1 into.
 */
typedef struct {
    /* Four DisplayPositionGroup children arrays, packed. Each row is
       the three DisplayPositionChild pointers that type begins with,
       and display_effect_update_callbacks.c hands rows 0 to 3 to
       func_8003A920 as (DisplayPositionGroup *)(p + 0), (p + 0xC),
       (p + 0x18) and (p + 0x24). Four rows of three pointers is 0x30,
       exactly this member's extent.

       The apparent contradiction is worth writing down, because it is
       what makes the reading safe rather than reckless:
       DisplayPositionGroup is 0x38 bytes, so a view based at 0x24
       would run to 0x5C and overrun this 0x4C record. It does not,
       because func_8003A920 touches only children[0..2] -- the first
       twelve bytes -- and never the x/y pair at the type's tail. Only
       func_8003A95C reads that tail, and nothing passes it a row.

       Left as s32 grid[4][3] rather than retyped: making it an array
       of a three-pointer struct would be a better description, but
       every existing user spells it as this shape and the change
       belongs with the conversion of
       display_effect_update_callbacks.c, not ahead of it. */
    s32 grid[4][3];
    s8 field_30;
    /* DisplayEffectState names this same byte field_31 on this same
       memory; func_8002EB78 and display_effect_update_callbacks.c
       reach it too. Named to agree with that view. */
    u8 field_31;
    u8 field_32;
    u8 display_effect_step;
    u16 field_34;
    /* The second half of the pair func_80039F44 seeds -- 0x34 with 0x68 and
       this with 0xB2 -- and the pair func_8003A990 and func_8003AAE4 hand to
       the position helpers as x and y. DisplayEffectState in
       display_effect_lifecycle.h names the same halfword field_36 on this
       same memory. */
    u16 field_36;
    u8 pad_38[2];
    u8 field_3A;
    u8 field_3B;
    /* Written by func_8002EB78 and Text_HandleDisplayEffectCommand and read by
       display_effect_update_callbacks.c and
       duel_effect_state_callbacks.c. Four sites prove a byte is
       here; none of them says what it carries. */
    u8 field_3C;
    u8 pad_3D;
    /* DisplayEffectState names this same halfword field_3E on this
       same memory, and func_8003B378 works it as a countdown: it
       seeds it from rand() and decrements it to zero. Named to agree
       with that view, as field_31 was in #3534. */
    u16 field_3E;
    /* The six halfwords the sweep in func_8003A990 works, and the tail this
       record had no names for.

       0x40 and 0x42 are the destination the sweep eases toward: it hands
       them to func_8003A95C as the final x and y, and subtracts a cosine
       fraction of 0x44/0x46 from them on the way. func_8003AAE4 uses 0x40
       differently -- a 0..0x80 fade ramp it steps by D_8009B0D8 * 8 -- so
       neither reading governs and the offsets stay the names.

       0x44 and 0x46 are the distance still to travel: the sweep seeds them
       with 0x40 - 0x34 and 0x42 - 0x36 on its first tick.

       0x48 is the eased angle, running to TRIG_ANGLE_QUARTER_TURN, and 0x4A
       the per-tick step derived from it, whose sign picks which end of the
       cosine the sweep starts from.

       Signed, because the sweep compares 0x4A against zero and passes 0x40,
       0x42, 0x44 and 0x46 through s16 casts. The sites that want the
       unsigned load spell it `*(u16 *)&record->field_40`, which keeps the
       lhu the target issues there and still names the field. */
    s16 field_40;
    s16 field_42;
    s16 field_44;
    s16 field_46;
    s16 field_48;
    s16 field_4A;
} MenuRecord;

#define MENU_RECORD_OFFSET(member) ((u32)&(((MenuRecord *)0)->member))

typedef char MenuRecord_size_must_be_0x4C[
    sizeof(MenuRecord) == 0x4C ? 1 : -1
];
typedef char MenuRecord_field_30_must_be_at_0x30[
    MENU_RECORD_OFFSET(field_30) == 0x30 ? 1 : -1
];
typedef char MenuRecord_field_31_must_be_at_0x31[
    MENU_RECORD_OFFSET(field_31) == 0x31 ? 1 : -1
];
typedef char MenuRecord_display_effect_step_must_be_at_0x33[
    MENU_RECORD_OFFSET(display_effect_step) == 0x33 ? 1 : -1
];
typedef char MenuRecord_field_34_must_be_at_0x34[
    MENU_RECORD_OFFSET(field_34) == 0x34 ? 1 : -1
];
typedef char MenuRecord_field_36_must_be_at_0x36[
    MENU_RECORD_OFFSET(field_36) == 0x36 ? 1 : -1
];
typedef char MenuRecord_field_3C_must_be_at_0x3C[
    MENU_RECORD_OFFSET(field_3C) == 0x3C ? 1 : -1
];
typedef char MenuRecord_field_40_must_be_at_0x40[
    MENU_RECORD_OFFSET(field_40) == 0x40 ? 1 : -1
];
typedef char MenuRecord_field_4A_must_be_at_0x4A[
    MENU_RECORD_OFFSET(field_4A) == 0x4A ? 1 : -1
];

#undef MENU_RECORD_OFFSET

#define MENU_RECORD_COUNT 3

extern MenuRecord D_800EB010[];

/* func_8002EB78 selects element 0 or 1 from D_800EB010 using operand bit
 * 7, arms it, then polls its display_effect_step on subsequent ticks. */
extern MenuRecord *D_8009B274;

/* The effect record Text_HandleDisplayEffectCommand last armed: it stores an element of
   D_800EB010 here at three sites, and that is the whole of its provenance.
   It was declared u8 * in display_effect_lifecycle.h while its readers
   walked the record by byte offset -- 0x30, 0x32, 0x33, 0x3C, 0x40, 0x42
   and 0x44. This record names every one of those, the last of them in
   #3534, so the gap that justified the byte view is closed and the
   declaration now lives with its type.

   func_80039FD4 still takes u8 *, so the two sites that hand it this
   pointer cast; that parameter is shared with four callers of other types
   and is a separate unit. Retail reaches the pointer gp-relative at every
   one of its thirteen sites. */
extern MenuRecord *D_8009B328;

#endif
