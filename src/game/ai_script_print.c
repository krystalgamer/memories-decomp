#include "../types.h"
#include "../unmatched.h"
#include "../psyq/stdio.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_Print(void)
{
    int checkpoint = AiScript_ReadByte();

    printf(D_80011908);
    printf(D_80011918, checkpoint);
}
