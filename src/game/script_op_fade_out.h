#ifndef MEMORIES_DECOMP_SCRIPT_OP_FADE_OUT_H
#define MEMORIES_DECOMP_SCRIPT_OP_FADE_OUT_H

#include "../types.h"

/* gScript_apfnCommandHandlers handler: starts a fade out. The low six bits of its stream byte,
 * when set, become the fade step in gFade_State.step; bit 7 keeps the command busy
 * until gFade_State.flags bit 7 clears, which is how the script waits for the
 * fade rather than only starting it. */
void Script_OpFadeOut(void);

#endif
