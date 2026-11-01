#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_RESOURCE_ACQUISITION_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_RESOURCE_ACQUISITION_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#define DISPLAY_EFFECT_VRAM_SLOT_COUNT 5

/* The wider display-effect view owned by this callback. The lifecycle helpers
 * consume the same record through their narrower DisplayEffectState view. */
typedef struct {
    u8 pad_00[0x30];
    s8 effect_id;
    u8 resource_entry_index;
    u8 variant_update_flags;
    u8 state;
    u16 field_34;
    u16 field_36;
    u8 pad_38[0x04];
    u8 slot_index;
    u8 pad_3D;
    u16 variant_countdown;
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
 * the gRuntime_pSharedWorkBase buffer that DisplayEffect_UpdateResourceAcquisition walks: it starts slot at the last one, steps
 * i down to 0 and slot with it, takes i into gDisplayEffect_ResourceCacheWriteSlot when the entry is
 * negative, compares each entry with a->effect_id, stores a->effect_id at that
 * selected index and indexes the gRuntime_pSharedWorkBase slots with the same byte;
 * DisplayEffect_ResetMenuRecords stores -1 into all of them (menu_record_reset.c:28-35).
 * The declaration stays unsized: display_effect_resource_setup.c builds
 * under gcc_2_8_1_g8_split, where a complete five-byte array is small data
 * and the link fails
 * (notes/research/matching-evidence.md:6684-6695). menu_record_reset.c used
 * to declare it `s8 [DISPLAY_EFFECT_VRAM_SLOT_COUNT]` while this consumer
 * used `s8 []`. */
extern s8 gDisplayEffect_ResourceCacheWriteSlot;
extern s8 gDisplayEffect_CachedResourceIds[];

/* Step two: restore a cache hit or request/capture a miss, then build row
 * zero on a later invocation. The write-slot byte is shared across calls;
 * the callback retains this narrow view and the step table's explicit cast. */
void DisplayEffect_UpdateResourceAcquisition(DisplayEffectVramState *state);

#endif
