#ifndef MEMORIES_DECOMP_FUNC_8003A560_H
#define MEMORIES_DECOMP_FUNC_8003A560_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#define DISPLAY_EFFECT_VRAM_SLOT_COUNT 5

/* The wider display-effect view owned by this callback. The lifecycle helpers
 * consume the same record through their narrower DisplayEffectState view. */
typedef struct {
    u8 pad_00[0x30];
    s8 field_30;
    u8 field_31;
    u8 field_32;
    u8 state;
    u16 field_34;
    u16 field_36;
    u8 pad_38[0x04];
    u8 field_3C;
    u8 pad_3D;
    u16 field_3E;
} DisplayEffectVramState;

typedef struct {
    u8 image[0x18000];
    u8 clut[0x400];
    u8 extra[0x800];
    RECT image_rect;
    RECT clut_rect;
} DisplayEffectVramSlot;

typedef char DisplayEffectVramSlot_size_must_be_0x18C10[
    sizeof(DisplayEffectVramSlot) == 0x18C10 ? 1 : -1
];

/* DISPLAY_EFFECT_VRAM_SLOT_COUNT bytes, one per DisplayEffectVramSlot of
 * D_80010000 that func_8003A560 walks: it starts slot at the last one, steps
 * i down to 0 and slot with it (func_8003A560.c:31-32, :58), takes i into
 * D_8009B326 when the entry is negative (:33-34), compares each entry with
 * a->field_30 (:36), stores a->field_30 at D_8009B326 (:80) and indexes the
 * D_80010000 slots with the same byte (:84); func_80039E9C stores -1 into
 * all of them (menu_record_reset.c:28-35). The declaration stays unsized:
 * func_8003A560.c builds under gcc_2_8_1_g8_split, where a complete
 * five-byte array is small data and the link fails
 * (notes/research/matching-evidence.md:6684-6695). menu_record_reset.c used
 * to declare it `s8 [DISPLAY_EFFECT_VRAM_SLOT_COUNT]` and func_8003A560.c
 * `s8 []`. */
extern s8 D_8015C410[];

void func_8003A560(DisplayEffectVramState *state);

#endif
