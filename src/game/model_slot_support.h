#ifndef MEMORIES_DECOMP_MODEL_SLOT_SUPPORT_H
#define MEMORIES_DECOMP_MODEL_SLOT_SUPPORT_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "model.h"
#include "../ygo_types.h"

void Model_GetSlotDimensionsWithOverrides(s32 index, s16 *output);
void Model_DrawSlotUnitsOnly(s32 index);

/* Returns flat light `light` of model slot `index`, clamping `light` to 0 at
   three; callers write the light's vectors through the returned pointer. */
unsigned char *Model_GetFlatLight(unsigned int index, unsigned int light);

/* Retain the separate fourth halfword; it is not vector padding. */
void Model_CopySlotPosition(s32 idx, u16 *out);

void Model_InitLightTriplet(s32 index);

/* Address of one MODEL_SLOT_DATA_ENTRY_SIZE entry of model slot `index`'s data.
 * `slot` beyond the slot's entry_count falls back to its current entry,
 * root_coordinate_index. */
void *Model_GetSlotDataEntry(s32 index, s32 slot);

/* The same address for the current entry, returned as an integer. The only
 * caller, the campaign map's set_location.c, casts it back to a pointer and
 * hands it to Model_UpdateCoordinateUnitMatrix as the slot's coordinate unit. */
u32 Model_GetCurrentDataEntry(s32 index);

s32 Model_TransformPartVertex(s32 slot, s32 part, s32 vertex, VECTOR *out);
ModelSlotS32Quad *Model_GetSlotScale(s32 index);
void Model_SetSlotScale(s32 index, ModelSlotS32Quad *source);
u8 *Model_GetSlotTintDescriptor(s32 index);
void Model_SetSlotTintTarget(
    s32 slot,
    s32 mode,
    s32 target0,
    s32 target1,
    s32 target2
);
void Model_SetSlotAmbient(s32 index, s32 x, s32 y, s32 z);

struct _GsCOORDUNIT;

/* Rebuilds a coordinate unit's local matrix from its Euler angles, scales it
 * when `scale` is non-null, and clears the stamp so libgs recomputes the world
 * matrix on the next pass. Declared on the incomplete type so callers that
 * only forward a slot's root_coordinate do not have to pull in the libhmd chain. */
void Model_UpdateCoordinateUnitMatrix(struct _GsCOORDUNIT *unit, VECTOR *scale);

/* The light-source matrix GsSetLsMatrix is handed. Five callers --
 * display_projection.c, both functions in duel_scene.c, duel_scene.c and
 * duel_scene.c -- each declared it u8[] and cast it to (MATRIX *) at the
 * call, so the SDK signature already fixed the type at every site; the cast
 * was the type escaping the declaration. model_slot_support.c spelled it as
 * a scalar u8 and returned its address as void *. library_draw_wireframe_globe.c declared it
 * with this type but privately, which is the same statement written twice.
 * The last two took this header on 2026-09-11, each measured byte-identical;
 * both build at gcc_2_8_1_g0. */
extern MATRIX gGraphics_LightSourceMatrix;

/* Returns &gGraphics_LightSourceMatrix through the definition's existing void * interface.
 * The background renderer converts that object pointer to the SDK's
 * MATRIX * parameter of GsSetLsMatrix; no alternate allocation is involved. */
void *Model_GetLightSourceMatrix(void);

#endif
