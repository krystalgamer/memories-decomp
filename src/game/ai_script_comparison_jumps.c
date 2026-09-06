#include "../types.h"

extern int gAiScript_aMemory[];
typedef struct{int first,addend,result;}State;
extern State gAiScript_State;
extern int AiScript_ReadByte(void),AiScript_ReadShort(void);

void AiScript_JumpGreaterEqual(void){int a=AiScript_ReadByte(),b=AiScript_ReadByte(),result=AiScript_ReadShort();register int*values=gAiScript_aMemory;if(values[a]>=values[b]){register State*s=&gAiScript_State;result+=s->addend;s->result=result;}}

void AiScript_JumpGreater(void){int a=AiScript_ReadByte(),b=AiScript_ReadByte(),result=AiScript_ReadShort();register int*values=gAiScript_aMemory;if(values[a]>values[b]){register State*s=&gAiScript_State;result+=s->addend;s->result=result;}}
