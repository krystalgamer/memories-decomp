#include "../../types.h"

extern u8 D_8016D400;
extern void func_80169734(void);

int NameEntry_PollCompletion(void)
{
    func_80169734();
    return D_8016D400 & 0x10;
}
