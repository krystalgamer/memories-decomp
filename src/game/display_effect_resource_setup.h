#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_RESOURCE_SETUP_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_RESOURCE_SETUP_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "display_object.h"
#include "menu_record.h"

/* Builds up to three display objects and writes them back through `out` as
 * out[0], out[1] and out[2], storing NULL in the slots it does not fill. That
 * is why the second parameter is DisplayObject **. The backing MenuRecord
 * grid has mixed consumers, so callers cast each three-word row to this
 * output view. DisplayEffect_CrossfadeResources passes rows 0 through 3, and each call fills its
 * own three-slot group. Returns 1 when it built anything and 0 when
 * DisplayEffect_HasResourceEntry rejected the first index; every caller
 * discards that result.
 *
 * The first parameter is the record itself. menu_record.h already names the
 * three fields this reads -- slot_index selects which of the two buffer bases
 * and which layer constants to use, and field_34/field_36 are the x/y pair
 * that header documents as "the pair DisplayEffect_UpdatePositionTween and DisplayEffect_FadeInRecord hand to
 * the position helpers". They are u16 there and read signed here, so the two
 * reads keep an explicit `*(s16 *)&` at the use.
 *
 * DisplayEffect_UpdateResourceAcquisition in display_effect_resource_setup.c holds the same memory as a
 * DisplayEffectState instead and says so with its cast; menu_record.h records
 * that the two views name these same bytes. */
s32 DisplayEffect_BuildResourceObjects(
    MenuRecord *record, DisplayObject **out, s32 resource_index);

/* Walks three levels of halfword offset tables rooted at `b`: entry `x` of the
 * root gives the offset of a second table, entry `y` of that gives a third,
 * and entry `z` of the third must be non-zero. Returns 1 when all three are
 * non-zero, 0 at the first zero. DisplayEffect_BuildResourceObjects asks it
 * about the table at gDisplay_abResourceBank0 or gDisplay_abResourceBank1. */
int DisplayEffect_HasResourceEntry(
    unsigned char *base, int group, int row, int entry);

/* Exactly three nullable slots, descending. Replaces blend bits, narrows
 * depth to s8 and adjusts CLUT y; zero retains GsALON, restores neutral RGB
 * and decrements CLUT y rather than selecting an opaque/idempotent mode. */
void DisplayEffect_SetObjectRowBlend(u8 **objects, u32 attribute, s32 depth_offset);

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
