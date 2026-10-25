#ifndef MEMORIES_DECOMP_MODEL_TRANSFER_FLAGS_H
#define MEMORIES_DECOMP_MODEL_TRANSFER_FLAGS_H

#include "../types.h"
#include "model_effect_key.h"

/* Two independent flag bytes, gModel_bCameraHoldState and gModel_bHandlerCallbackState, each with a getter
 * and a setter.
 *
 * The accessors are not the only route to them: later functions in
 * model_effect_state.c and two single-function sources read the bytes directly
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
extern Key *G32 gModel_pCurrentEffectKey;
extern u8 gModel_bEffectKeyCount;
extern u8 gModel_bEffectKeyPreparationPending;
extern s8 gModel_nEffectKeyBatchOrdinal;
extern u8 gModel_bCameraHoldState;
extern u8 gModel_bHandlerCallbackState;

s32 Model_GetCameraHoldState(void);
#ifdef VERSION_JAPAN
extern void Model_SetCameraHoldState(s32 value) asm("func_8004EBD0");
#else
void Model_SetCameraHoldState(s32 value);
#endif
s32 Model_GetHandlerCallbackState(void);
void Model_SetHandlerCallbackState(s32 value);

#endif
