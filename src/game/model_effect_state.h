#ifndef MEMORIES_DECOMP_MODEL_EFFECT_STATE_H
#define MEMORIES_DECOMP_MODEL_EFFECT_STATE_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../ygo_types.h"

/* Model-effect setup, state, endpoint queueing and dispatch.
 *
 * Model_SetCameraOrbit's last parameter is a pointer in the definition -- the SVECTOR
 * whose offset the step applies -- not the fifth s32 its two callers declared.
 * model_scene_states.c passes literal zeroes, which were always null pointers;
 * Model_SetCameraOrbitFromPreset and Model_QueueCameraOrbitFromPreset forward the same optional pointer type.
 *
 * Model_QueueEffectEndpointPair takes two indices into gModel_aEffectEndpoints rather than
 * pointers, and a negative index means "no endpoint" -- that is how a caller
 * asks for a one-sided effect, and model_slot_control.c passes -1 for exactly that.
 *
 * Model_ApplyEffectKeyQueueCommand dispatches begin/append/preparation for the
 * ring that Model_EvaluateKeyframeSpline evaluates. Its two endpoint pointers
 * are the narrowest common view used by its callers, which
 * pass ModelEffectEndpoint pointers, a local s16[4], or generic byte pointers.
 * Model_QueueEffectEndpointSequences walks sequences of that same endpoint type. Its post-step
 * stop tests retain an aligned s16 view of kind: retail loads that field
 * with lh, while direct access through the packed type expands it.
 * These operations return early while D_8009B07B and D_8009B07C are both 1. */
void Model_SetCameraOrbitFromPreset(s32 mode, s32 coefficient, SVECTOR *offset);
/* Steps the active effect-key queue and its two camera endpoint channels. */
void Model_UpdateEffectKeyframes(void);
void Model_SetCameraOrbit(s32 mode, s32 y, s32 a, s32 b, SVECTOR *offset);
s32 Model_NextEffectKeyQueueMode(void);
void Model_SetEffectKeyBatchingEnabled(s32 value);
void Model_QueueCameraOrbitFromPreset(s32 mode, s32 coefficient, SVECTOR *offset, s32 arg);
void Model_QueueEffectEndpointPair(s32 first, s32 second, s32 arg);
void Model_QueueCameraShake(s32 value, s32 arg);
void Model_QueueEffectEndpointSequences(
    s32 count, ModelEffectEndpoint *first, ModelEffectEndpoint *second, s32 arg
);
void Model_ApplyEffectKeyQueueCommand(
    s32 mode,
    ModelEffectEndpoint *first,
    ModelEffectEndpoint *second,
    s32 arg
);

/* Preserve historical matching overlay bodies while publishing canonical imports. */
#define func_8005F5C8 Model_QueueCameraOrbitFromPreset
#define func_8005F7B0 Model_QueueCameraShake

#endif
