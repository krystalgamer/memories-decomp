#include "../types.h"

extern u8 *D_8009B290;
extern u16 D_8009B27C;
extern u8 D_801A8000[];
extern s32 func_8002E3B4(void);
extern void func_8003FF34(void);
extern void func_8003FF58(s32);

void func_8002EDB0(void)
{
    if (func_8002E3B4() == 0) {
        u8 *stream = D_8009B290;
        u32 raw = *stream;
        u32 argument;
        s32 command;

        D_8009B290 = stream + 1;
        argument = raw & 0x7F;
        command = raw;
        if (argument == 0) {
            func_8003FF34();
        } else {
            func_8003FF58(argument);
        }
        if (command & 0x80) {
            return;
        }
    }
    D_8009B27C = 0;
}

void func_8002EE20(void)
{
    u8 *source = D_8009B290;
    u32 offset;

    D_8009B290 = source + 2;
    offset = source[0] | (source[1] << 8);
    D_8009B27C = 0;
    D_8009B290 = D_801A8000 + offset;
}
