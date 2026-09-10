#include "../types.h"
#include "../psyq/strings.h"

#include "mem_card.h"
#include "data_transfer_request.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "mem_card_dialog_runtime.h"

int func_8003F70C(void)
{
    func_8003F454();
    if (gMemCard_wDialogFlags != 0) {
        return 0;
    }
    return D_8009B3EF;
}

void func_8003F740(s32 value)
{
    gMemCard_wDialogFlags = MEM_CARD_DIALOG_FLAG_ACTIVE;
    D_8009B3DE = value;
    D_8009B3C1 = 0;
}

void func_8003F758(void *arg0, s32 arg1, u8 *arg2, s32 arg3)
{
    strcpy(D_800EFE18, arg2);
    D_8009B3F9 = 0;
    D_8009B3C2 = arg1;
    D_8009B3C4 = 0x200;
    D_8009B3DC =
        (arg1 + MEM_CARD_BLOCK_SIZE - 1) / MEM_CARD_BLOCK_SIZE;
    D_8009B3D0 = (u32)arg0;
    func_8003F740(arg3);
}
