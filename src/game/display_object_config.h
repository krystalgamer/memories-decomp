#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_CONFIG_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_CONFIG_H

#include "../types.h"

/* Layout view used by func_80040510, which writes the position, extent and
 * blend fields of a display object in one call. It covers only the fields
 * func_80040510 itself touches.
 *
 * This is deliberately NOT the canonical DisplayObject in display_object.h,
 * and it is not a rival to it. func_80040510 writes 16-bit halves at four
 * offsets the canonical record holds as 32-bit words: 0x30, 0x3C, 0x40 and
 * 0x48, plus a pair of bytes inside the halfword at 0x5C. Splitting those
 * words to expose the halves would change how existing users store them --
 * display_slot_lifecycle.c zeroes field_40 and field_48 with single word
 * stores, which would become pairs of halfword stores -- and the build would
 * stop being byte-identical. That was the same hazard display_object.h
 * recorded for the byte at 0x22 inside its field_20; that one has since been
 * resolved by carrying 0x20 and 0x3C as unions of both widths, so the word
 * writes keep their sw and the narrow readers still get their halves. The
 * same device would retire this view; it has not been applied here yet.
 *
 * So the two descriptions coexist on purpose: the canonical record owns the
 * pool and the word-width fields, and this view records the halfword shape
 * func_80040510 works in. Neither is a substitute for the other. */
typedef struct {
    u8 pad_00[0x08];
    u16 flags;
    u8 pad_0A[0x0E];
    s16 half_height;
    s16 half_width;
    u8 pad_1C[0x14];
    s16 x;
    s16 y;
    u8 pad_34[0x08];
    s16 height;
    s16 width;
    s16 field_40;
    s16 field_42;
    u8 pad_44[0x04];
    s16 half_height_2;
    s16 half_width_2;
    u8 pad_4C[0x10];
    u8 field_5C;
    u8 field_5D;
    u8 pad_5E[0x08];
    u8 field_66;
} DisplayObjectConfigView;

DisplayObjectConfigView *func_80040510(
    DisplayObjectConfigView *object,
    s32 x,
    s32 y,
    s32 height,
    s32 width,
    s32 field_5C,
    s32 field_5D,
    s32 field_66,
    s32 field_40,
    s32 field_42);

#endif
