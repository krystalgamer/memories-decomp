#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_LIFECYCLE_H

#include "../types.h"

/* Fields the display-effect lifecycle helpers read and write while arming,
 * resetting and tearing down an effect slot.
 *
 * This is a narrower view of MenuRecord in menu_record.h: the records these
 * helpers are handed are elements of D_800EB010, and every offset the two
 * spell in common agrees -- 0x30, 0x32, 0x33 (display_effect_step there),
 * 0x34 and now 0x36. The view stays because the helpers' own signatures are
 * written against it; what it adds over the full record is nothing, and a
 * consumer that needs the tail should take MenuRecord, as func_8003A990.c
 * does. */
typedef struct {
    u8 pad_00[0x31];
    u8 field_31;
    u8 field_32;
    u8 state;
    u16 field_34;
    u16 field_36;
    u8 pad_38[0x06];
    u16 field_3E;
} DisplayEffectState;

s32 func_80039F1C(DisplayEffectState *object);
void func_80039F44(DisplayEffectState *object);
void func_80039F90(void **objects);
void func_80039FD4(u8 *object);

/* The effect object func_80038EB0 last armed: it stores a pointer into
 * D_800EB010 here at three sites, and the readers walk that record by
 * byte offset (0x30, 0x32, 0x33, 0x3C, 0x40, 0x42 and 0x44 across the
 * three reading units) or hand it to func_80039FD4, whose parameter is
 * u8 *. MenuRecord now names all of those but 0x3C, so the reason the
 * pointer is still u8 * is func_80039FD4's parameter and func_8003787C.c's
 * one-field view, not a gap in the record. Retail reaches the pointer
 * gp-relative at every one of its thirteen sites. */
extern u8 *D_8009B328;
void func_80039FF8(DisplayEffectState *object);

#endif
