#include "../types.h"
#include "text_stream_commands.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "dialog_choice.h"

void Text_ExtendGlyphCode(u8 *object)
{
    s32 index;
    u32 combined;
    u32 value;
    u8 **slot;
    u8 *p;

    index = *(s8 *)(object + 0x58);
    combined = (D_8009B33A - 0xF0) << 8;
    slot = &((u8 **)object)[index];
    p = *slot;
    value = *p;
    p++;
    *slot = p;
    value |= combined;
    *(volatile u16 *)&D_8009B33A = value;
    *(volatile s32 *)&D_8009B350 = -1;
}

void Text_SetStateFromStream(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    u8 *current = *stream;
    u8 value = current[0];

    *stream = current + 1;
    object[0x51] = value;
    D_8009B350 = 1;
}
