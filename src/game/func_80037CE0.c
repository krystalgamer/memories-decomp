#include "../types.h"
#include "../unmatched.h"
#include "dialog_choice.h"
#include "text_stream_commands.h"

void func_80037CE0(volatile u8 *object)
{
    u32 flags;
    u8 control;

    if (object[0x56] >= gDialog_bChoiceCount) {
        object[0x51] = 1;
        flags = *(volatile u16 *)(object + 0x34);
        object[0x56] = 0;
        control = D_8009B34C;
        D_8009B340 = 0;
        flags &= 0xEFFF;
        control &= 0x30;
        *(u16 *)(object + 0x34) = flags;
        if (control != 0)
            gDialog_bChoiceCount = 2;
    }
}
