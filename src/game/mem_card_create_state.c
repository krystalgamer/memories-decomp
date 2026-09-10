#include "../types.h"
#include "mem_card_directory.h"
#include "../psyq/libmcrd.h"

#define GMEMCARD_RESULT_USES_WIDE_ARGS
#include "mem_card.h"
#include "save_data.h"
#include "file_transfer.h"
#include "../unmatched.h"

/* Memory-card create dialog state machine, the third of the family beside
   func_8003E490 and func_8003EED0.

   It walks the card from "no save present" to "save written": accept the
   card, read the directory, look the file up, compare free blocks against
   the size the save needs, offer and run a format, create the file, read
   back and check the existing state, and finally write it. The low nibble of
   D_8009B3EB is the state and the 0x80/0x40 bits latch the first pass
   through each one; D_8009B3F4 carries the previous call's result and
   D_8009B3EB is set to the message state on every failure.

   Levers that mattered here:
   - The three jump tables belong to this file, so split.yaml hands it the
     rodata at 0xC68 that initial_data_1e used to cover.
   - The two inner switches are written in the target's block-layout order,
     not numeric order, because gcc emits switch arms in source order. The
     arms that merge into blocks elsewhere in the function (state 9 and state
     0xD) go last.
   - State 7 and state 8 each need their OWN local for D_8009B3F4. Sharing
     one gave the two "0xD/9" and "0xB/9" tails the same register, and
     cross-jumping then merged the compare as well; retail merges only the
     final store because its two tails are in $a0 and $v1.
   - The three early exits inside state 7 are gotos to tails written at the
     end of that arm. As inline `{ ...; break; }` bodies gcc leaves the calls
     in line and the shared blocks retail keeps between the two halves of the
     0x40 test never form.
   - The message id in state 10 is pinned. It is the only argument in the
     function that is not a constant, retail keeps it in $a2 and copies it to
     $a0 at the shared call, and every unpinned spelling - if/else, both
     assignment orders, u8 and s32, and the u8 prototype - let gcc allocate
     it straight into $a0 and drop the copy. */

extern s8 gDialog_bChoice __attribute__((section(".data")));
extern u8 D_8009AF70[];
extern s32 D_801D5608[];

void func_8003E854(void)
{
    s32 files;
    s32 free_blocks;
    s32 needed;
    s32 result;
    s32 mode;
    register s32 message __asm__("$6");

    switch (D_8009B3EB & 0xF) {
    case 0:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
            func_8003E46C(0xC9, 0x20);
            break;
        }
        D_8009B3EB = 1;
        if (gDialog_bChoice != 0) {
            D_8009B3EB = 0xC;
            break;
        }
        /* fallthrough */
    case 1:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            func_8003E46C(0xD4, 0);
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        switch (D_8009B3F4) {
        case 0:
        case 3:
            D_8009B3EB = 3;
            break;
        case 4:
            D_8009B3EB = 4;
            if (gMemCard_wDialogFlags & 0x100) {
                break;
            }
            if (D_8009B3D4 != 0) {
                break;
            }
            D_8009B3EB = 0xE;
            break;
        case 1:
            D_8009B3EB = 9;
            break;
        case 2:
            D_8009B3EB = 0xD;
            break;
        }
        break;
    case 2:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_8009B3EC = 3;
            break;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            break;
        }
        func_8003E46C(0xB8, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)D_8009AF70,
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            D_8009B3EB = 0xD;
            break;
        }
        if (func_80044598(D_800EFE18, D_800EFBC0, files) >= 0) {
            D_8009B3EB = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && D_8009B3D4 == 0) {
            D_8009B3EB = 0xE;
            break;
        }
        free_blocks = func_80044544(D_800EFBC0, files);
        needed = D_8009B3DC;
        if (free_blocks >= needed) {
            D_8009B3EB = 6;
            goto create;
        }
        D_801D5608[0] = MEM_CARD_BLOCK_COUNT - free_blocks;
        D_801D5608[1] = needed;
        func_8003E46C(0xDB, 0x18);
        break;
    case 4:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            func_8003E46C(0xDE, 0x10);
            break;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            func_8003E46C(0xDF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            D_8009B3EB = 0xC;
            break;
        }
        D_8009B3EB = 5;
        func_8003E46C(0xBE, 0);
        break;
    case 5:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        if (D_8009B3F0 != 2) {
            break;
        }
        switch (D_8009B3F4) {
        case 0:
            func_8003E46C(0xC3, 0x18);
            break;
        case 3:
            func_8003E46C(0xC3, 0x18);
            break;
        case 2:
            D_8009B3EB = 0xD;
            break;
        case 4:
            if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    func_8003E46C(0xDD, 0x18);
                    break;
                }
                func_8003E46C(0xBF, 0x10);
                break;
            }
            D_8009B3EB = 1;
            break;
        case 1:
            D_8009B3EB = 9;
            break;
        }
        break;
    case 6:
    create:
        if (MemCardCreateFile(0, (char *)D_800EFE18, D_8009B3DC) != 0) {
            D_8009B3EB = 0xB;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            D_8009B3C4 = 0;
            D_8009B3D0 -= SAVE_DATA_HEADER_SIZE;
            D_8009B3C2 += SAVE_DATA_HEADER_SIZE;
        }
        D_8009B3EB = 8;
        goto write;
    case 7:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_8009B3EC = 0;
            MemCardReadFile(0, (char *)D_800EFE18,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            D_8009B3C4, 0x480);
            goto io_pending;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            mode = D_8009B3F4;
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (mode != 0) {
                D_8009B3EB = 0xD;
                if (mode == 1) {
                    D_8009B3EB = 9;
                }
            } else {
                if (D_8009B3D4 != 0) {
                    goto message_cf;
                }
                if (SaveData_MatchesDuelistAndCurrentSequence(
                        (SaveDataState *)D_8009B3D0,
                        (SaveDataState *)gLibrary_aCardArtRecord) != 0) {
                    goto message_cf;
                }
                if (D_8009B3EC != 0) {
                    goto state_e;
                }
                D_8009B3EC++;
                D_8009B3EB &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardReadFile(0, (char *)D_800EFE18,
                                (unsigned long *)gLibrary_aCardArtRecord,
                                D_8009B3C4 + SAVE_DATA_STATE_SIZE, 0x480);
                goto io_pending;
            }
            break;
        state_e:
            D_8009B3EB = 0xE;
            break;
        message_cf:
            func_8003E46C(0xCF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            D_8009B3EB = 8;
            goto write;
        }
        D_8009B3EB = 0xC;
        break;
    case 8:
    write:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            func_8003E46C(0xD6, 0);
            MemCardWriteFile(0, (char *)D_800EFE18,
                             (unsigned long *)D_8009B3D0, D_8009B3C4,
                             D_8009B3C2);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        result = D_8009B3F4;
        if (result != 0) {
            D_8009B3EB = 0xB;
            if (result == 1) {
                D_8009B3EB = 9;
            }
            break;
        }
        D_8009B3EB = 0xA;
        break;
    case 9:
        func_8003E46C(0xD2, 0x18);
        break;
    case 10:
        D_8009B3EF = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            D_8009B3D4 = 0;
            gSaveDataSequence++;
        }
        func_8003E46C(message, 0x18);
        break;
    case 11:
        func_8003E46C(0xD9, 0x18);
        break;
    case 12:
        func_8003E46C(0xCD, 0x18);
        D_8009B3EF = 3;
        break;
    case 13:
        func_8003E46C(0xDA, 0x18);
        break;
    case 14:
        func_8003E46C(0xCE, 0x18);
        break;
    }
}
