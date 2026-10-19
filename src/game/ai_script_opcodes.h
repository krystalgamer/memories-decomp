#ifndef MEMORIES_DECOMP_AI_SCRIPT_OPCODES_H
#define MEMORIES_DECOMP_AI_SCRIPT_OPCODES_H

#include "../types.h"

/* AiScript_Call, AiScript_Return and AiScript_SetRandom are declared in
   ai_script_commands.h with the rest of the command table's handlers, and
   twenty-one sources reach them there. This header used to repeat those three
   prototypes word for word; it points at their home instead.
   src/game/ai_script_control_flow.c is the only file that includes this one,
   and it already includes ai_script_commands.h above it. */
#include "ai_script_commands.h"

void AiScript_EndHand(void);
void AiScript_EndField(void);

#endif
