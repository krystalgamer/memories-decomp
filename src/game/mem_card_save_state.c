#include "../types.h"
#include "../psyq/libmcrd.h"

#include "mem_card.h"
#include "save_data.h"
#include "duel_effect.h"
#include "../unmatched.h"

/* Memory-card save dialog state machine, the write-side twin of
   func_8003E490.

   Levers that mattered here:
   - D_801D5648 is an unsized array for the same reason it is in the load
     state machine: as a plain s32 extern the -G8 build puts it in small data
     and the store collapses to one gp-relative word, where retail
     materialises the %hi half in its own register.
   - The inner switch on D_8009B3F4 is written in the target's block order,
     case 1 first and the shared 0/3 body second, because gcc emits switch
     arms in source order.
   - The gMemCard_wDialogFlags |= 0x1000 tail is written once inside case 3,
     between the write block and the retry block, and entered by goto from
     cases 0 and 2; the retry block is reached by goto so it stays out of
     line after that tail, which is retail's layout.
   - Both MemCardWriteFile calls are spelled out, one per branch, so only the
     trailing pair cross-jumps and the D_800EFE18 %lo stays duplicated. */
extern u32 D_8009B3E0;
extern u8 gLibrary_aCardArtRecord[];

extern s32 SaveData_HasSameDuelistCode(u8 *, u8 *);

void func_8003EED0(void)
{
    s32 files;
    u8 *record;

    if ((D_8009B3C1 & 0x80) == 0) {
        D_8009B3C1 |= 0x80;
        func_8003E46C(0xC0, 0);
        D_8009B3EB = 0;
    }
    switch (D_8009B3EB & 0xF) {
    case 0:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
            do {
            } while (MemCardAccept(D_8009B3F9) == 0);
            goto io_pending;
        }
        switch (D_8009B3F4) {
        case 1:
            func_8003E46C(0xC1, 0x18);
            break;
        case 0:
        case 3:
            D_8009B3EB = 1;
            D_8009B3F9 ^= 0x10;
            if (D_8009B3F9 != 0) {
                D_8009B3EB = 0;
            }
            break;
        case 2:
            func_8003E46C(0xDA, 0x18);
            break;
        case 4:
            func_8003E46C(0xC3, 0x18);
            break;
        }
        break;
    case 1:
        D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
        if (MemCardGetDirentry(D_8009B3F9, (char *)D_800EFE18,
                               (struct DIRENTRY *)D_800EFBC0, &files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            func_8003E46C(0xDA, 0x18);
            break;
        }
        if (files == 0) {
            func_8003E46C(0xC3, 0x18);
            break;
        }
        D_8009B3F9 ^= 0x10;
        if (D_8009B3F9 != 0) {
            D_8009B3EB = 1;
            break;
        }
        D_8009B3EB = 2;
        /* fallthrough */
    case 2:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_8009B3EC = 0;
            D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
            MemCardReadFile(D_8009B3F9, (char *)D_800EFE18,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            D_8009B3C4, 0x480);
            goto io_pending;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (D_8009B3F4 != 0) {
                func_8003E46C(0xDA, 0x18);
                break;
            }
        }
        record = (u8 *)D_8009B3D0;
        if (D_8009B3F9 != 0) {
            record = (u8 *)D_8009B3E0;
        }
        if (SaveData_HasSameDuelistCode(record, gLibrary_aCardArtRecord) == 0) {
            if (D_8009B3EC != 0) {
                func_8003E46C(0xC3, 0x18);
                break;
            }
            D_8009B3EC++;
            D_8009B3EB &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardReadFile(D_8009B3F9, (char *)D_800EFE18,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            D_8009B3C4 + SAVE_DATA_STATE_SIZE, 0x480);
            goto io_pending;
        }
        D_8009B3F9 ^= 0x10;
        if (D_8009B3F9 != 0) {
            D_8009B3EB = 2;
            break;
        }
        D_8009B3EB = 3;
        /* fallthrough */
    case 3:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY
                          | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            func_8003E46C(0xC4, 0);
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            goto step;
        }
    write:
        D_8009B3EB &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        if (D_8009B3F9 != 0) {
            MemCardWriteFile(D_8009B3F9, (char *)D_800EFE18,
                             (unsigned long *)D_8009B3E0, D_8009B3C4, 0x80);
        } else {
            MemCardWriteFile(D_8009B3F9, (char *)D_800EFE18,
                             (unsigned long *)D_8009B3D0, D_8009B3C4, 0x80);
        }
    io_pending:
        gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
        break;
    step:
        if (D_8009B3F4 != 0) {
            func_8003E46C(0xC5, 0x18);
            break;
        }
        D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        D_8009B3F9 ^= 0x10;
        if (D_8009B3F9 == 0) {
            D_8009B3D0 += 0x80;
            D_8009B3E0 += 0x80;
            D_8009B3C4 += 0x80;
            D_8009B3C2 -= 0x80;
            if (D_8009B3C2 == 0) {
                D_8009B3EF = 1;
                func_8003E46C(0xC6, 0x18);
                break;
            }
        }
        goto write;
    }
}
