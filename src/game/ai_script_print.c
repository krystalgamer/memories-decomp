#include "../types.h"
#include "../psyq/stdio.h"

extern int AiScript_ReadByte(void);
extern const char D_80011908[];
extern const char D_80011918[];

void AiScript_Print(void)
{
    int checkpoint = AiScript_ReadByte();

    printf(D_80011908);
    printf(D_80011918, checkpoint);
}
