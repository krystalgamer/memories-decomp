#include "../../types.h"

extern u8 D_8016D400;
extern void NameEntry_UpdateDialog(void);

int NameEntry_PollCompletion(void)
{
    NameEntry_UpdateDialog();
    return D_8016D400 & 0x10;
}
