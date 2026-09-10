#define D_8009B3EA_IN_DATA
#define D_8009B3ED_IN_DATA
#include "../types.h"
#include "mem_card.h"
#include "file_transfer.h"
#include "../unmatched.h"

extern u8 D_8009B26C[];

extern s32 func_8003FD14(void);

void func_80031000(void)
{
    u8 flags = D_8009B2EB;
    s32 result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B3ED = 0;
        D_8009B3EA = 0;
    }
    result = func_8003FD14();
    if (result != 0) {
        if (result == 1) {
            File_RequestMainMenuPackage();
            File_WaitForTransfers();
            D_8009B26C[0] = 0x10;
        }
        D_8009B2EB = 0;
    }
}

void func_80031078(void)
{
    D_8009B2EB = 0;
}
