#ifndef MEMORIES_DECOMP_MODEL_SCENE_STATES_H
#define MEMORIES_DECOMP_MODEL_SCENE_STATES_H

#include "../types.h"

/* The animated-battle scene's small state machine. func_80059C18 arms a
   scene, func_80059C24 and func_80059C9C start the two phases, and the two
   predicates report when each has finished. */
void func_80059C18(s32 value);
void func_80059C24(void);
s32 func_80059C88(void);
void func_80059C9C(void);
s32 func_80059CD0(void);

/* The scene mode those phases run under: func_80059C24 stores 0x13 and
 * func_80059C9C stores 20, Model_SetSlotProperties in model_scene_setup.c
 * stores 15 and func_800533D8 there resets it to 0, and model_cleanup.c
 * dispatches on 15, 19 and 20 to one of three calls. Retail stores it
 * with sb at four sites and reads it with lbu at four, all gp-relative,
 * so it is one unsigned byte; three of the reads are model_cleanup.c's
 * and the fourth is in func_800559D4, still assembly. The one s8
 * spelling was in model_scene_states.c, which only stores it. */
extern u8 D_8009AF94;

#endif
