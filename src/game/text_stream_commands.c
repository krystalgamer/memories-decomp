#include "../types.h"
#include "text_stream_commands.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "dialog_choice.h"

void Text_SetStateFromStream(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    u8 *current = *stream;
    u8 value = current[0];

    *stream = current + 1;
    object[0x51] = value;
    D_8009B350 = 1;
}
