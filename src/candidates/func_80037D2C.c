/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 4 variables to hard registers and 3 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/text_stream_commands.c.
 */
#include "../types.h"
#include "../game/text_stream_commands.h"
#include "../game/duel_effect.h"
#include "../unmatched.h"
#include "../game/dialog_choice.h"

void Text_ExtendGlyphCode(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    register s32 temporary asm("$2");
    register u32 combined asm("$3");
    register u32 value asm("$5");
    register u8 **slot asm("$4");

    temporary = *(s8 *)(object + 0x58);
    combined = D_8009B33A;
    asm("" : "+r"(temporary), "+r"(combined));
    slot = &((u8 **)object)[temporary];
    asm("" : "+r"(slot), "+r"(combined));
    combined -= 0xF0;
    temporary = (s32)*slot;
    combined <<= 8;
    value = *(u8 *)temporary;
    temporary++;
    *slot = (u8 *)temporary;
    temporary = -1;
    value |= combined;
    D_8009B33A = value;
    asm("" : "+r"(temporary) : : "memory");
    D_8009B350 = temporary;
}

