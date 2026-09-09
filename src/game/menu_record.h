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
    /* The second half of the pair func_80039F44 seeds -- 0x34 with 0x68 and
       this with 0xB2 -- and the pair func_8003A990 and func_8003AAE4 hand to
       the position helpers as x and y. DisplayEffectState in
       display_effect_lifecycle.h names the same halfword field_36 on this
       same memory. */
    u16 field_36;
    u8 pad_38[2];
    u8 field_3A;
    u8 field_3B;
    u8 pad_3C[4];
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
typedef char MenuRecord_field_36_must_be_at_0x36[
    MENU_RECORD_OFFSET(field_36) == 0x36 ? 1 : -1
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

#endif
