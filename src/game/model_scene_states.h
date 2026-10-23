#ifndef MEMORIES_DECOMP_MODEL_SCENE_STATES_H
#define MEMORIES_DECOMP_MODEL_SCENE_STATES_H

#include "../types.h"

/* Per-slot preparation and the shared model-scene update. */
/* The credits presentation's per-slot step; it does nothing for a slot that
 * is already ready (is_active set). Otherwise, with no load pending
 * (load_step 0xFF) and the transfer queue free, it picks a random card id
 * outside the MODEL.MRG gaps and starts Model_LoadMonsterMerge; with a load
 * pending it advances it. When that makes the slot ready, it sets the slot's
 * yaw to a quarter turn -- or, if the other slot is ready, to that slot's
 * coordinate yaw plus a quarter turn modulo a full turn -- clears the x/z
 * rotation and translation, updates the matrix and runs the slot handlers. */
void Model_UpdateCreditsMonsterSlot(s32 index);
void Model_UpdateCreditsPresentation(void);

/* Scene mode 15's phase controller, which Model_UpdateScene runs while
   gModel_bSceneMode is 15; Model_UpdateExodiaPresentation below is its mode 19
   counterpart. */
void Model_UpdateAnimatedBattle(void);

/* The animated-battle scene's small state machine.
   Model_SetPresentationBgmCommand stores the BGM command replayed when the
   scene begins, Model_StartExodiaPresentation starts the Exodia presentation,
   and Model_IsExodiaPresentationComplete reports when it has finished.
   Model_StartCreditsPresentation and Model_IsCreditsPresentationComplete are
   the second initializer/predicate pair. */
void Model_UpdateExodiaPresentation(void);
void Model_SetPresentationBgmCommand(s32 value);
void Model_StartExodiaPresentation(void);
s32 Model_IsExodiaPresentationComplete(void);
void Model_StartCreditsPresentation(void);
s32 Model_IsCreditsPresentationComplete(void);

#endif
