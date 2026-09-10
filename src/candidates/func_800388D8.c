/* Reclassified from matching_c (#3859). This was src/game/func_800388D8.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g0_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G0). Under
 * gcc_2_8_1_g0, a single threshold, it is 60 instructions against the
 * target's 59, opcode distance 3. The source below is the match, unchanged
 * apart from its include paths. */
#include "../types.h"
#include "../game/fade.h"
#include "../game/display_object_helpers.h"
#include "../unmatched.h"

void func_800388D8(u8 *arg0)
{
    u8 **cursor = (u8 **)(arg0 + *(s8 *)(arg0 + 0x58) * 4);
    u8 *stream = *cursor;
    s32 command = *stream;
    s32 opcode;

    *cursor = stream + 1;
    opcode = command;
    if (opcode & 0x40) {
        D_8009B140 = *(u8 *)&D_8009AF74[1] + 9;
    }
    if (opcode & 0x20) {
        D_8009B140 = 4;
    }
    if (opcode & 0x10) {
        if (opcode & 1) {
            Fade_InitOutColor(0xFFFFFF);
        } else {
            Fade_InitInColor(0xFFFFFF);
        }
        gFade_State.step = 4;
    } else if (opcode & 1) {
        func_80015C84();
    } else {
        func_80015C0C();
    }
    if (opcode & 0x80) {
        Fade_Wait();
    }
}
