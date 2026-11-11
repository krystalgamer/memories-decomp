#ifndef MEMORIES_DECOMP_MODEL_CAMERA_EFFECTS_H
#define MEMORIES_DECOMP_MODEL_CAMERA_EFFECTS_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "model_effect_key.h"
#include "model.h"
#include "../ygo_types.h"

/* One entry of the keyframe ring at gModel_aEffectKeys.
   Model_ApplyEffectKeyQueueCommand installs two requested endpoints.
   Model_RefreshEffectKeySlotValues refreshes slot-backed resolved values.
   Model_PrepareEffectKey derives the duration/bound at +0x22. */

/* The ring itself, ten records, declared here because the evaluator and
 * dispatcher share it and this is where Key lives. Two facts bound it at
 * ten: the next named object, gModel_aHandlerKeyRegistry (c_symbols.ld:396, model.h:541-542),
 * starts at +0x190 = 10 * 0x28, and Model_ApplyEffectKeyQueueCommand writes a record only while
 * `gModel_bEffectKeyCount < 0xA` (model_camera_effects.c; gModel_bEffectKeyCount is a u8,
 * model_transfer_flags.h:47). That unit stores through a u8 pointer: two
 * 8-byte ModelEffectEndpoint copies at +0 and +8, halfwords at +6, +0xE,
 * +0x20, +0x22 and +0x24, a byte at +0x26 (:51-66), and stores the ring's
 * address into gModel_pCurrentEffectKey. Model_EvaluateKeyframeSpline forms Key pointers from the ring and
 * derives each channel pointer as `(u8 *)key + k * 8`.
 * Model_UpdateEffectKeyframes walks and wraps the same ring in model_camera_effects.c.
 * The evaluator used to declare it `Key []` and
 * Model_ApplyEffectKeyQueueCommand used a local `u8 []` declaration. */
extern Key gModel_aEffectKeys[10];

/* One 8-byte channel record. The evaluator reads the first three halfwords as
   the interpolated components; `w` covers the halfword at +6, which the
   keyframe records use as a channel kind and which the current pose leaves
   untouched. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u16 w;
} Coeff;

void Model_EvaluateKeyframeSpline(Key *cur, s32 k, s32 scale, s32 den, s16 *out);

/* Refreshes endpoints and progress, then prepares readiness/timing when the
 * separate current-key guard permits. */
void Model_PrepareEffectKey(Key *key);

/* Always captures the evaluator's starting pose; a nonzero flag also
   refreshes slot-derived endpoints in the current key. */
void Model_CaptureKeyframePose(s32 capture_endpoints);

void Model_BuildCubicCoefficients(
    s32 a, s32 b, s32 c, s32 d, Coeff *out, s32 scale
);

ModelCameraOrbitPreset *ModelEffect_GetCoefficient(s32 index);
s32 Model_AdjustCameraDistanceForSlot(s32 level, s32 value);

/* Model-effect setup, state, endpoint queueing and dispatch.
 *
 * Model_SetCameraOrbit's last parameter is a pointer in the definition -- the SVECTOR
 * whose offset the step applies -- not the fifth s32 its two callers declared.
 * model_scene_services.c passes literal zeroes, which were always null pointers;
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
 * These operations return early while gModel_bCameraHoldState and gModel_bHandlerCallbackState are both 1. */
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
    s32 count, ModelEffectEndpoint *first, ModelEffectEndpoint *second, s32 magnitude
);
void Model_ApplyEffectKeyQueueCommand(
    s32 mode,
    ModelEffectEndpoint *first,
    ModelEffectEndpoint *second,
    s32 magnitude
);

/* Queues a slot-relative reference endpoint and distance/yaw/pitch deltas.
 * The optional SVECTOR offset retains side-dependent x/z signs and the
 * unsigned middle-component read. Scene controllers and the preset wrapper
 * pass the same pointer type; no integer-to-pointer forwarding is involved. */
void Model_QueueRelativeCameraOrbit(
    s32 slot_selector, s32 distance, s32 yaw, s32 pitch, SVECTOR *offset, s32 key_magnitude
);

/* Two independent flag bytes, gModel_bCameraHoldState and gModel_bHandlerCallbackState, each with a getter
 * and a setter.
 *
 * The accessors are not the only route to them: later functions in
 * model_camera_effects.c read the bytes directly
 * rather than through the getters, which is why the bytes are declared here
 * as well.
 *
 * Six callers share one guard, spelled as
 *
 *     state = Model_GetCameraHoldState();
 *     if (state != 1 || Model_GetHandlerCallbackState() != state) { ... }
 *
 * so the interesting condition is that both flags read exactly 1: camera-move
 * requests, camera target/eye slot stores and key preparation are then held
 * off. The setter for the first is called with values other than 0 and 1,
 * which is why the callers compare against 1 rather than testing for
 * non-zero: Model_StartBattleSlotAction stores 2 or 0 from bit 7 of the slot's
 * record flags (and 1 when bit 6 is set), and Model_UpdateAnimatedBattle
 * promotes 2 to 1 just before it queues its next effect-key command.
 * Model_ResetEffectKeyQueue clears it.
 */
/* Three more bytes of the same block, immediately before the two flags below.
 *
 *   gModel_bEffectKeyCount  The live key count. Model_EvaluateKeyframeSpline takes indices modulo it
 *               to walk the key ring, and its note there calls it that.
 *   gModel_bEffectKeyPreparationPending  Queue begin sets this flag; the updater
 *               prepares the queued keys and clears it. Explicit prepare and
 *               reset also clear it.
 *   gModel_nEffectKeyBatchOrdinal  -1 disables batching. Starting at zero makes
 *               the first command begin a queue and later commands append;
 *               each enabled command advances the signed byte. Preparation
 *               and reset restore -1. The explicit signed-byte casts and
 *               postincrement forms are retained at their original readers.
 *
 * gModel_pCurrentEffectKey sits in the same run. Before consolidation, the state and
 * transfer sources spelled it s32 while the evaluator source spelled it
 * u8 *, and a pointer against a word is a question about what the storage is,
 * not a spelling to pick.
 * The listings answer it: every retail read of the word is a base for a load
 * at a fixed offset (lhu 0x22 and 0x24, lbu 0x26, lh 0x6, or an addu with an
 * index and then lhu) or a beqz null test. Queue setup stores gModel_aEffectKeys;
 * the updater advances by Key records, wraps to that base, or clears the
 * cursor together with the count. Reset clears it too. Key * therefore
 * names the current record rather than ownership of the array; the two
 * former s32 spellings were casts around a pointer. Every retail access
 * is gp-relative, so no unit needs an arm.
 */
extern Key *gModel_pCurrentEffectKey;
extern u8 gModel_bEffectKeyCount;
extern u8 gModel_bEffectKeyPreparationPending;
extern s8 gModel_nEffectKeyBatchOrdinal;
extern u8 gModel_bCameraHoldState;
extern u8 gModel_bHandlerCallbackState;

s32 Model_GetCameraHoldState(void);
void Model_SetCameraHoldState(s32 value);
s32 Model_GetHandlerCallbackState(void);
void Model_SetHandlerCallbackState(s32 value);

void Model_ResetEffectKeyQueue(void);
s32 Model_IsEffectKeyQueueEmpty(void);
s32 Model_GetCurrentEffectKeyAddress(void);
/* Refreshes slot-backed kinds 2/3; a null argument selects the current key. */
void Model_RefreshEffectKeySlotValues(Key *key);

#endif
