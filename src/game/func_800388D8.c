#include "../types.h"
#include "fade.h"

extern u8 D_8009AF76;
extern u8 D_8009B140;
extern u8 D_800E9ECF[];

void func_800388D8(u8 *arg0)
{
    u8 **cursor = (u8 **)(arg0 + *(s8 *)(arg0 + 0x58) * 4);
    u8 *stream = *cursor;
    s32 command = *stream;
    s32 opcode;

    *cursor = stream + 1;
    opcode = command;
    if (opcode & 0x40) {
        D_8009B140 = D_8009AF76 + 9;
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
        D_800E9ECF[0] = 4;
    } else if (opcode & 1) {
        func_80015C84();
    } else {
        func_80015C0C();
    }
    if (opcode & 0x80) {
        Fade_Wait();
    }
}
