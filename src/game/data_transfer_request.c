#include "../types.h"
#include "../psyq/strings.h"

#include "mem_card.h"
#include "data_transfer_request.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "mem_card_dialog_runtime.h"

int MemCardDialog_Poll(void)
{
    MemCardDialog_Update();
    if (gMemCard_wDialogFlags != 0) {
        return 0;
    }
    return D_8009B3EF;
}

void MemCardDialog_Start(s32 step)
{
    gMemCard_wDialogFlags = MEM_CARD_DIALOG_FLAG_ACTIVE;
    D_8009B3DE = step;
    D_8009B3C1 = 0;
}

void MemCardDialog_Request(void *buf, s32 size, u8 *name, s32 step)
{
    strcpy(D_800EFE18, name);
    D_8009B3F9 = 0;
    D_8009B3C2 = size;
    D_8009B3C4 = 0x200;
    D_8009B3DC =
        (size + MEM_CARD_BLOCK_SIZE - 1) / MEM_CARD_BLOCK_SIZE;
    D_8009B3D0 = (u32)buf;
    MemCardDialog_Start(step);
}
