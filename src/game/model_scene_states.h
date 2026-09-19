#ifndef MEMORIES_DECOMP_MODEL_SCENE_STATES_H
#define MEMORIES_DECOMP_MODEL_SCENE_STATES_H

#include "../types.h"

/* Per-slot preparation and the scene update called by func_80059CE4. */
void func_80050584(s32 index);
void func_800507D0(void);

/* Scene mode 15's phase controller, which func_80059CE4 runs while
   D_8009AF94 is 15; func_8004FE2C below is its mode 19 counterpart. */
void func_8004EB00(void);

/* The animated-battle scene's small state machine. func_80059C18 arms a
   scene, func_80059C24 starts the Exodia presentation, and
   Model_IsExodiaPresentationComplete reports when it has finished.
   Model_StartCreditsPresentation and Model_IsCreditsPresentationComplete are
   the second initializer/predicate pair. */
void func_8004FE2C(void);
void func_80059C18(s32 value);
void func_80059C24(void);
s32 Model_IsExodiaPresentationComplete(void);
void Model_StartCreditsPresentation(void);
s32 Model_IsCreditsPresentationComplete(void);

#endif
