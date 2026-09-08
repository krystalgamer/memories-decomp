#ifndef AI_SCRIPT_CALL_CONTROL_H
#define AI_SCRIPT_CALL_CONTROL_H

#include "../types.h"

extern char gAiScript_szSourceLineFormat[];

void AiScript_Call(void);
void AiScript_Return(void);
void AiScript_SetRandom(void);

#endif
