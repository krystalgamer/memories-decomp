#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_CONFIG_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_CONFIG_H

#include "../types.h"

/* The canonical record, declared in display_object.h. Named rather than
 * included so this header stays the narrow-view header it is meant to be;
 * every caller of the one function below already has the full record. */
struct DisplayObject;

/* The narrower view DisplayObject_SetResourceVariant and
 * DisplayObject_UpdateResourceVariant use: the flags halfword at 0x08 and the
 * three selector bytes at 0x67..0x69. It is the same record
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
} DisplayObjectConfig;

/* Clears bit 0x10 of the flags halfword and stores `value` into the 0x69
 * selector byte. The parameter is s32 rather than the u8 the definition used
 * to spell: the body stores it with an sb, which truncates anyway, so
 * widening it costs nothing -- while a u8 parameter costs an andi at every
 * call site whose argument range the compiler cannot prove. See
 * notes/research/matching-evidence.md. */
void DisplayObject_SetResourceVariant(DisplayObjectConfig *object, s32 value);

/* Changes the third resource selector only when needed and clears the
 * resolution flag so the display-object runtime rebuilds its resource. */
void DisplayObject_UpdateResourceVariant(
    DisplayObjectConfig *object,
    s32 value
);
void DisplayObject_SetResourcePath(
    DisplayObjectConfig *object, u8 field_67, u8 field_68, u8 field_69
);

/* Narrow position/extent view retained for Duel_CreateCardEffectOverlay,
 * whose source object is not otherwise exposed as the canonical record. */
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
 * which is how three of the four declared it as returning void.
 *
 * It takes the canonical record: it writes seven of its members and nothing
 * the narrow views above cover. The two callers that still hand it a byte
 * cursor keep their own view and name the record at the call: shop.c its
 * byte arithmetic, DuelScene_UpdateHandActions its hand-card view. */
void *DisplayObject_ConfigureSpriteResource(struct DisplayObject *object, s32 field_67,
                    s32 field_68, s32 field_69, s32 color, s32 texture);

struct DisplayObject *DisplayObject_ConfigureScreenSprite(
    struct DisplayObject *object,
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
