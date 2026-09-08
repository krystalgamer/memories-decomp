#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_H

#include "../types.h"
#include "display_object_layout.h"

typedef void (*DisplayObjectCallback)(u8 *);

/* One entry of the DISPLAY_OBJECT_POOL_CAPACITY-entry pool at D_800EFE48.
 *
 * previous and next are pool indices, not pointers: func_800402A0 unlinks an
 * entry by writing -1 to both and then repairs the neighbour's link through
 * D_800EFE48[v].previous, and func_80040814 walks the list through next.
 */
/* The union of the four private views this record had: display_object.h's
 * own (previous/next/flags/update), display_slot_lifecycle.c's DisplaySlot
 * (the richest), display_projection.c's ProjectionEntry (+0x28) and
 * func_80040588.c's local DisplayObject (+0x17, +0x30, +0x3C, +0x5C). Every
 * offset they share agrees; each named a different subset.
 *
 * One exception, and it is why func_80040588.c still keeps a private copy:
 * that view has a u8 at +0x22, which falls inside the u32 at +0x20 that
 * display_slot_lifecycle.c stores zero into with a single word write.
 * Splitting field_20 to expose +0x22 would turn that sw into an sh and the
 * build stops being byte-identical, so both facts are recorded here and
 * neither is forced into the layout. */
typedef struct DisplayObject {
    s16 previous;                  /* 0x00 */
    s16 next;                      /* 0x02 */
    u32 field_04;                  /* 0x04 */
    u16 flags;                     /* 0x08 */
    u8 field_0A;                   /* 0x0A */
    u8 field_0B;                   /* 0x0B */
    u32 field_0C;                  /* 0x0C */
    u32 field_10;                  /* 0x10 */
    u16 field_14;                  /* 0x14 */
    s8 field_16;                   /* 0x16 */
    u8 tex_index;                  /* 0x17 */
    u16 field_18;                  /* 0x18 */
    u16 field_1A;                  /* 0x1A */
    u16 field_1C;                  /* 0x1C */
    s16 field_1E;                  /* 0x1E */
    u32 field_20;                  /* 0x20 */
    DisplayObjectCallback update;  /* 0x24 */
    s32 position;                  /* 0x28 */
    u8 pad_2C[4];                  /* 0x2C */
    s32 field_30;                  /* 0x30 */
    u8 pad_34[8];                  /* 0x34 */
    s32 field_3C;                  /* 0x3C */
    u32 field_40;                  /* 0x40 */
    u32 field_44;                  /* 0x44 */
    u32 field_48;                  /* 0x48 */
    u8 pad_4C[8];                  /* 0x4C */
    void *field_54;                /* 0x54 */
    u8 pad_58[4];                  /* 0x58 */
    u16 field_5C;                  /* 0x5C */
    u16 field_5E;                  /* 0x5E */
    u8 pad_60[5];                  /* 0x60 */
    u8 field_65;                   /* 0x65 */
    u8 field_66;                   /* 0x66 */
    u8 pad_67[5];                  /* 0x67 */
    u8 field_6C;                   /* 0x6C */
    u8 pad_6D[DISPLAY_OBJECT_RECORD_SIZE - 0x6D];
} DisplayObject;

#define DISPLAY_OBJECT_OFFSET(member) ((u32)&(((DisplayObject *)0)->member))

typedef char DisplayObject_size_must_match_record_size[
    sizeof(DisplayObject) == DISPLAY_OBJECT_RECORD_SIZE ? 1 : -1
];
typedef char DisplayObject_update_must_be_at_0x24[
    DISPLAY_OBJECT_OFFSET(update) == 0x24 ? 1 : -1
];
typedef char DisplayObject_position_must_be_at_0x28[
    DISPLAY_OBJECT_OFFSET(position) == 0x28 ? 1 : -1
];
typedef char DisplayObject_field_65_must_be_at_0x65[
    DISPLAY_OBJECT_OFFSET(field_65) == 0x65 ? 1 : -1
];

extern DisplayObject D_800EFE48[DISPLAY_OBJECT_POOL_CAPACITY];
/* &D_800EFE48[DISPLAY_OBJECT_RESERVED_CAPACITY]: the allocatable tail of the
 * same pool, which func_8004002C scans. */
extern DisplayObject D_800F0548[
    DISPLAY_OBJECT_POOL_CAPACITY - DISPLAY_OBJECT_RESERVED_CAPACITY
];

#endif
