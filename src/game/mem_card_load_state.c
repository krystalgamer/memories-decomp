#include "../types.h"
#include "../psyq/libmcrd.h"

#include "mem_card.h"
#include "save_data.h"
#include "../unmatched.h"

/* Memory-card load dialog state machine.

   Levers that mattered here:
   - D_801D5648 must be declared as an unsized array. As a plain s32 extern
     the -G8 build puts it in small data and the store becomes one gp-relative
     word; the target materialises %hi in its own register, which also keeps
     $a0 busy across the message value and is why the message ends up in $a1
     and is copied into $a0 at the call.
   - gDialog_bChoice is read through a .data section attribute so it keeps the
     %hi/%lo pair the target uses instead of a gp-relative byte load.
   - The gMemCard_wDialogFlags |= 0x1000 tail is written once inside case 3
     and entered by goto from case 1, matching the retail block order.
   - Cases 0/1 and 2/3 fall through, and cases 6 and 8 are spelled as separate
     bodies: retail keeps two distinct jump-table targets for them.
*/
extern u8 D_8009B3D4;
extern u8 D_8009B3EF;
extern u16 D_8009B3C2;
extern u16 D_8009B3C4;
extern u32 D_8009B3D0;
extern s32 D_8009B3F4;
extern s8 gDialog_bChoice __attribute__((section(".data")));
extern u8 D_800EFBC0[];
extern u8 D_800EFE18[];
extern s32 D_801D5648[];

void func_8003E490(void)
{
    s32 files;
    s32 message;

    switch (D_8009B3EB & 0xF) {
    case 0:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            func_8003E46C(0xC8, 0x20);
            break;
        }
        D_8009B3EB = 1;
        if (gDialog_bChoice != 0) {
            D_8009B3EB = 9;
            break;
        }
        /* fallthrough */
    case 1:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
            message = 0xD4;
            if ((gMemCard_wDialogFlags & 0x200) != 0) {
                message = 0xC0;
            }
            func_8003E46C(message, 0);
            do {
            } while (MemCardAccept(D_8009B3F9) == 0);
            goto io_pending;
        }
        switch (D_8009B3F4) {
        case 0:
        case 3:
            D_8009B3EB = 2;
            break;
        case 1:
            D_8009B3EB = 5;
            break;
        case 2:
        case 4:
            D_8009B3EB = 6;
            break;
        }
        break;
    case 2:
        if (MemCardGetDirentry(D_8009B3F9, (char *)D_800EFE18,
                               (struct DIRENTRY *)D_800EFBC0, &files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            D_8009B3EB = 6;
            break;
        }
        if (files == 0) {
            D_8009B3EB = 6;
            break;
        }
        D_8009B3EB = 3;
        /* fallthrough */
    case 3:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            func_8003E46C(0xD5, 0);
            MemCardReadFile(D_8009B3F9, (char *)D_800EFE18,
                            (unsigned long *)D_8009B3D0, D_8009B3C4,
                            D_8009B3C2);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        switch (D_8009B3F4) {
        case 0:
            D_8009B3EB = 7;
            if (SaveData_ValidateIntegrity((u8 *)D_8009B3D0) != 0) {
                break;
            }
            D_8009B3EB = 4;
            if ((gMemCard_wDialogFlags & 0x400) != 0) {
                func_8003E46C(0xBD, 0x18);
                break;
            }
            gMemCard_wDialogFlags |= 0x400;
            break;
        case 1:
            D_8009B3EB = 5;
            break;
        case 2:
        case 3:
            D_8009B3EB = 8;
            break;
        case 4:
            break;
        case 5:
            D_8009B3EB = 6;
            break;
        }
        break;
    case 4:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            func_8003E46C(0xBD, 0x10);
            break;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            func_8003E46C(0xBC, 0x20);
            break;
        }
        D_8009B3EB = 9;
        if (gDialog_bChoice != 0) {
            break;
        }
        D_8009B3EB = 3;
        D_8009B3C4 += SAVE_DATA_STATE_SIZE;
        break;
    case 5:
        func_8003E46C(0xC1, 0x18);
        break;
    case 6:
        func_8003E46C(0xD3, 0x18);
        break;
    case 7:
        D_8009B3EF = 1;
        if ((gMemCard_wDialogFlags & 0x200) != 0) {
            func_8003E46C(0xD0, 8);
            break;
        }
        D_8009B3D4 = 0;
        func_8003E46C(0xD0, 0x18);
        break;
    case 8:
        func_8003E46C(0xD3, 0x18);
        break;
    case 9:
        D_8009B3EF = 3;
        func_8003E46C(0xC7, 0x18);
        break;
    }
}
