#ifndef YUGIOH_GAME_FUNC_8005EBF4_H
#define YUGIOH_GAME_FUNC_8005EBF4_H

#include "../types.h"
#include "model_effect_key.h"

/* One entry of the keyframe ring at gModel_aEffectKeys. func_8005F91C installs two
   requested endpoints, Model_RefreshEffectKeySlotValues refreshes slot-backed
   resolved values, and Model_PrepareEffectKey derives the duration/bound at
   +0x22. */

/* The ring itself, ten records, declared here because the evaluator and
 * dispatcher share it and this is where Key lives. Two facts bound it at
 * ten: the next named object, tent_ModelHandlerRegistry (c_symbols.ld:396, model.h:541-542),
 * starts at +0x190 = 10 * 0x28, and func_8005F91C writes a record only while
 * `gModel_bEffectKeyCount < 0xA` (model_effect_state.c; gModel_bEffectKeyCount is a u8,
 * model_transfer_flags.h:47). That unit stores through a u8 pointer: two
 * 8-byte ModelEffectEndpoint copies at +0 and +8, halfwords at +6, +0xE,
 * +0x20, +0x22 and +0x24, a byte at +0x26 (:51-66), and stores the ring's
 * address into gModel_pCurrentEffectKey. Model_EvaluateKeyframeSpline forms Key pointers from the ring and
 * derives each channel pointer as `(u8 *)key + k * 8`.
 * Model_UpdateEffectKeyframes walks and wraps the same ring in model_keyframe_update.c.
 * The evaluator used to declare it `Key []` and
 * func_8005F91C used a local `u8 []` declaration. */
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

#endif
