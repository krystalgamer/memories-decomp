#include "../types.h"
#define D_8009B140_IN_DATA
#include "fade.h"
#define D_8009AF74_IN_DATA
#include "display_object_helpers.h"
#include "duel_effect_command.h"

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
