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
 * That exception is now resolved. func_80040588.c and func_800408D0.c used
 * to keep private copies because they reach a u8 at +0x22 and a pair of u16
 * at +0x3C/+0x3E, which fall inside the words display_slot_lifecycle.c
 * stores with single word writes. Splitting those words outright would have
 * turned an sw into an sh; carrying each as a union of both widths does not,
 * so the word writes keep their sw and both renderers now take this record.
 * The same device would retire display_object_config.h's separate halfword
 * view of 0x30/0x3C/0x40/0x48, which is left for its own change. */
/* `attribute` is a GsSPRITE / GsBOXF attribute word, not a game flag word and
 * not a GPU packet tag. Both renderers copy it verbatim into the descriptor
 * they hand to GsSortSprite and friends, so every bit the game sets there is
 * read by libgs, and the names are libgs.h's:
 *
 *   0x01000000 / 0x02000000  colour mode; the texture-page step of 1, 2 or 4
 *                            a strip wrap applies is 4bpp, 8bpp and 16bpp
 *   0x04000000  GsPERS       perspective
 *   0x08000000  GsROTOFF     rotation off -- which is why the renderers only
 *                            compute `rotate` when it is clear
 *   0x10000000  GsAONE   \
 *   0x20000000  GsATWO    >  the two-bit semi-transparency rate at bit 28
 *   0x30000000  GsATHREE /
 *   0x40000000  GsALON       semi-transparency on
 *   0x80000000  GsDOFF       display off
 *
 * That is what the recurring composites mean: 0x50000000 is GsALON | GsAONE,
 * additive blending, which is what sparkles and afterimages want; 0x60000000
 * is GsALON | GsATWO, subtractive, which is what a fade-to-black overlay
 * wants. The `& 0x8FFFFFFF` masks clear the rate and GsALON together and keep
 * GsDOFF -- "turn semi-transparency off" -- and `& 0xF7FFFFFF` clears
 * GsROTOFF, "turn rotation on". */
typedef struct DisplayObject {
    s16 previous;                  /* 0x00 */
    s16 next;                      /* 0x02 */
    u32 attribute;                 /* 0x04 */
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
    /* 0x20 is reached both as a word and as the byte at +0x22. Both are
       retail's: display_slot_lifecycle.c clears the whole word with one sw,
       and the two sprite renderers read only the byte. Neither view is a
       superset, so the record carries both rather than choosing. */
    union {
        u32 word;
        struct {
            u16 field_20;
            u8 field_22;
            u8 field_23;
        } h;
    } field_20;                    /* 0x20 */
    DisplayObjectCallback update;  /* 0x24 */
    /* 0x28 and 0x30 are each read both ways: display_projection.c and the two
       sprite emitters take whole words, while display_parent_links.c derives a
       parent-relative offset from the halves. A union records both without
       forcing either side to spell the other's access. */
    union {
        struct {
            u16 field_28;
            u16 field_2A;
        } h;
        s32 word;
    } position;                    /* 0x28 */
    u16 field_2C;                  /* 0x2C */
    u8 pad_2E[2];                  /* 0x2E */
    union {
        struct {
            u16 field_30;
            u16 field_32;
        } h;
        s32 word;
    } field_30;                    /* 0x30 */
    u8 pad_34[8];                  /* 0x34 */
    /* 0x3C likewise: func_80040588 copies the whole word into the sprite
       primitive, while func_800408D0 reads the two halves separately. */
    union {
        s32 word;
        struct {
            u16 field_3C;
            u16 field_3E;
        } h;
    } field_3C;                    /* 0x3C */
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
typedef char DisplayObject_field_22_must_be_at_0x22[
    DISPLAY_OBJECT_OFFSET(field_20.h.field_22) == 0x22 ? 1 : -1
];
typedef char DisplayObject_update_must_be_at_0x24[
    DISPLAY_OBJECT_OFFSET(update) == 0x24 ? 1 : -1
];
typedef char DisplayObject_position_must_be_at_0x28[
    DISPLAY_OBJECT_OFFSET(position) == 0x28 ? 1 : -1
];
typedef char DisplayObject_field_3E_must_be_at_0x3E[
    DISPLAY_OBJECT_OFFSET(field_3C.h.field_3E) == 0x3E ? 1 : -1
];
typedef char DisplayObject_field_65_must_be_at_0x65[
    DISPLAY_OBJECT_OFFSET(field_65) == 0x65 ? 1 : -1
];

/* The DISPLAY_OBJECT_LIST_COUNT list heads, immediately below the pool.
 *
 * Each entry is the index of the first object on one list, or -1 for an empty
 * list; an object's own `next` continues the chain.  DisplayObject_ResetPool
 * writes -1 through all DISPLAY_OBJECT_LIST_COUNT of them with a single s16
 * cursor started at this address, which is what says the seven halfwords are
 * one array rather than seven objects that happen to be adjacent.
 */
extern s16 D_800EFE38[DISPLAY_OBJECT_LIST_COUNT];

extern DisplayObject D_800EFE48[DISPLAY_OBJECT_POOL_CAPACITY];
/* &D_800EFE48[DISPLAY_OBJECT_RESERVED_CAPACITY]: the allocatable tail of the
 * same pool, which func_8004002C scans. */
extern DisplayObject D_800F0548[
    DISPLAY_OBJECT_POOL_CAPACITY - DISPLAY_OBJECT_RESERVED_CAPACITY
];

#endif
