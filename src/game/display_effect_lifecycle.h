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
 * consumer that needs the tail should take MenuRecord, as func_8003A990 in
 * display_effect_update_callbacks.c does. */
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

/* D_8009B328 moved to menu_record.h with the MenuRecord * type it now
 * carries; it points into D_800EB010, which is declared there too. */
void func_80039FF8(DisplayEffectState *object);

#endif
