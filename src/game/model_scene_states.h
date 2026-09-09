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

#endif
