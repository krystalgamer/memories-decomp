#include "../types.h"

extern u8 D_8009AF76;
extern u8 D_8009B140;
extern u8 D_800E9ECF[];

extern void func_80015944(s32);
extern void func_8001581C(s32);
extern void func_80015C84(void);
extern void func_80015C0C(void);
extern void func_80015998(void);

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
            func_80015944(0xFFFFFF);
        } else {
            func_8001581C(0xFFFFFF);
        }
        D_800E9ECF[0] = 4;
    } else if (opcode & 1) {
        func_80015C84();
    } else {
        func_80015C0C();
    }
    if (opcode & 0x80) {
        func_80015998();
    }
}
