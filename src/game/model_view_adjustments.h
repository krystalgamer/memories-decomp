#ifndef MEMORIES_DECOMP_MODEL_VIEW_ADJUSTMENTS_H
#define MEMORIES_DECOMP_MODEL_VIEW_ADJUSTMENTS_H

#include "../types.h"

/* The pitch correction Model_CorrectViewDistance runs: while gModel_wViewPitch
 * is above 0x800 it steps the pitch through Model_StepViewOrbit into a band
 * around a target interpolated between 3968 and 3755 by gModel_RView.vry,
 * and otherwise steps it down by 4. */
#ifdef VERSION_JAPAN
extern void Model_CorrectViewPitch(void) asm("func_80055D7C");
#else
void Model_CorrectViewPitch(void);
#endif
/* The second camera/view correction pass Model_UpdateCamera runs. It takes
 * a 0x2BC-0x3E8 distance range adjusted by Model_AdjustCameraDistanceForSlot,
 * pushes the viewpoint out of each model slot (Model_SeparateViewFromSlots),
 * steps the camera-move field_04 outside Japan, runs Model_CorrectViewPitch, then steps
 * gModel_wViewDistance up towards the range's lower bound, down towards that
 * bound when arg0 is set, and down to the upper bound when it is exceeded. */
#ifdef VERSION_JAPAN
extern void Model_CorrectViewDistance(s32 arg0) asm("func_80055EE8");
#else
void Model_CorrectViewDistance(s32 arg0);
#endif

#endif
