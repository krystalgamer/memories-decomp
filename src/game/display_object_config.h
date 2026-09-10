#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_CONFIG_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_CONFIG_H

#include "../types.h"

/* The narrower view func_80040410 and func_80040424 use: the flags halfword
 * at 0x08 and the three selector bytes at 0x67..0x69. It is the same record
 * DisplayObjectConfigView below describes, seen through the only fields these
 * two writers touch, and it is spelt separately for the same reason that one
 * is: neither is the canonical DisplayObject in display_object.h. */
typedef struct {
    u8 pad_00[8];
    u16 flags;
    u8 pad_0A[0x5D];
    u8 field_67;
    u8 field_68;
    u8 field_69;
    /* 0x6A is the value func_80029108 compares against the 0x69 selector
       before it calls func_80040410 to store one into the other, so the two
       bytes are read as a pair by the same function.

       This byte is not unnamed elsewhere: duel_card_display_state.h calls it
       card_index, and on a duel card's display object that is exactly what it
       holds -- func_80017F04 stores (record - D_801A7AD8) / 0x1C into it, and
       func_80017DB4 and func_80017E3C index D_801A7AD8 with it. It is spelt
       field_6A here rather than card_index because nothing establishes that
       the objects func_80029108 walks are duel card records, and a selector
       fed from a card index on one kind of object is not evidence that every
       object carries one. Naming it after the one use we can see would assert
       more than is known.

       The canonical DisplayObject in display_object.h covers 0x67..0x6B with
       pad_67, so this does not contradict that record. It is last, and this
       type has no sizeof and no array user, so adding it moves no existing
       offset. */
    u8 field_6A;
} DisplayObjectConfig;

/* Clears bit 0x10 of the flags halfword and stores `value` into the 0x69
 * selector byte. The parameter is s32 rather than the u8 the definition used
 * to spell: the body stores it with an sb, which truncates anyway, so
 * widening it costs nothing -- while a u8 parameter costs an andi at every
 * call site whose argument range the compiler cannot prove. See
 * notes/research/matching-evidence.md. */
void func_80040410(DisplayObjectConfig *object, s32 value);

/* The sibling the comment above already names. It was defined here and
   declared nowhere, so its one caller wrote `extern s32 func_80040424();`
   -- wrong in the return type and in the parameter list, and undetectable,
   because that caller discards the result. */
void func_80040424(DisplayObjectConfig *object, s32 value);
void func_8004044C(
    DisplayObjectConfig *object, u8 field_67, u8 field_68, u8 field_69
);

/* Layout view used by func_80040510, which writes the position, extent and
 * blend fields of a display object in one call. It covers only the fields
 * func_80040510 itself touches.
 *
 * This is deliberately NOT the canonical DisplayObject in display_object.h,
 * and it is not a rival to it. func_80040510 writes 16-bit halves at four
 * offsets the canonical record held as 32-bit words: 0x30, 0x3C, 0x40 and
 * 0x48, plus a pair of bytes inside the halfword at 0x5C. The note here used
 * to say that splitting those words would change how existing users store
 * them -- display_slot_lifecycle.c zeroes field_40 and field_48 with single
 * word stores, which would become pairs of halfword stores -- and that the
 * build would stop being byte-identical.
 *
 * That is no longer the state of the record. The union device display_object.h
 * adopted for the byte at 0x22 inside field_20, and then for 0x28, 0x30 and
 * 0x3C, now covers 0x40 and 0x48 as well, so every offset this view exists to
 * name is reachable through the canonical record at both widths: the word
 * writes keep their sw through `.word` and the narrow users take `.h`.
 * Measured -- display_slot_lifecycle.c, func_80040588.c and func_800408D0.c
 * all still build byte-identically.
 *
 * What keeps this view alive is no longer the layout but the signature below:
 * func_80040510 and its callers pass the record as this type, and several of
 * those callers still hold it as a bare pointer. Retiring the view is that
 * change, not a layout change. Until then the two descriptions coexist: the
 * canonical record owns the pool, and this view records the shape
 * func_80040510 works in. */
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

/* Writes the three glyph-selector bytes at 0x67..0x69 plus the colour and
 * texture fields, and returns the object. Every caller discards the result,
 * which is how three of the four declared it as returning void. */
void *func_80040468(u8 *object, s32 field_67, s32 field_68, s32 field_69,
                    s32 color, s32 texture);

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
