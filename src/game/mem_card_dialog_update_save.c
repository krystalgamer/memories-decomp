#define MEM_CARD_DIALOG_MESSAGE_WIDE
#define GDIALOG_CHOICE_IN_DATA
#include "../types.h"
#include "../psyq/libmcrd.h"
#include "duel_effect.h"
#include "file_transfer.h"
#include "mem_card.h"
#include "mem_card_dialog_load_save.h"
#include "mem_card_directory.h"
#include "save_data.h"
#include "text_staging.h"
#include "dialog_choice.h"
#include "../unmatched.h"

void MemCardDialog_UpdateSave(void)
{
    s32 files;
    s32 value;
    s32 needed;
    s32 result;
    s32 mode;

    switch (D_8009B3EB & 0xF) {
    case 0:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
            MemCardDialog_SetMessage(0xC9, 0x20);
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
            MemCardDialog_SetMessage(0xD4, 0);
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
        MemCardDialog_SetMessage(0xB8, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)D_8009AF70,
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            D_8009B3EB = 0xD;
            break;
        }
        if (MemCard_FindEntry(D_800EFE18, (struct DIRENTRY *)D_800EFBC0, files) >= 0) {
            D_8009B3EB = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && D_8009B3D4 == 0) {
            D_8009B3EB = 0xE;
            break;
        }
        value = MemCard_CalcFreeBlocks((struct DIRENTRY *)D_800EFBC0, files);
        needed = D_8009B3DC;
        if (value >= needed) {
            D_8009B3EB = 6;
            goto create;
        }
        D_801D5608[0].blocks.used = MEM_CARD_BLOCK_COUNT - value;
        D_801D5608[0].blocks.needed = needed;
        MemCardDialog_SetMessage(0xDB, 0x18);
        break;
    case 4:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xDE, 0x10);
            break;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xDF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            D_8009B3EB = 0xC;
            break;
        }
        D_8009B3EB = 5;
        MemCardDialog_SetMessage(0xBE, 0);
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
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 3:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 2:
            D_8009B3EB = 0xD;
            break;
        case 4:
            if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    MemCardDialog_SetMessage(0xDD, 0x18);
                    break;
                }
                MemCardDialog_SetMessage(0xBF, 0x10);
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
            gMemCard_pPrimaryTransferCursor -= SAVE_DATA_HEADER_SIZE;
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
                        (SaveDataState *)gMemCard_pPrimaryTransferCursor,
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
            MemCardDialog_SetMessage(0xCF, 0x20);
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
            MemCardDialog_SetMessage(0xD6, 0);
            MemCardWriteFile(0, (char *)D_800EFE18,
                             (unsigned long *)gMemCard_pPrimaryTransferCursor,
                             D_8009B3C4,
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
        MemCardDialog_SetMessage(0xD2, 0x18);
        break;
    case 10:
        D_8009B3EF = 1;
        value = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            value = 0xD1;
            D_8009B3D4 = 0;
            gSaveDataSequence = (s32)((u32)gSaveDataSequence + 1);
        }
        MemCardDialog_SetMessage(value, 0x18);
        break;
    case 11:
        MemCardDialog_SetMessage(0xD9, 0x18);
        break;
    case 12:
        MemCardDialog_SetMessage(0xCD, 0x18);
        D_8009B3EF = 3;
        break;
    case 13:
        MemCardDialog_SetMessage(0xDA, 0x18);
        break;
    case 14:
        MemCardDialog_SetMessage(0xCE, 0x18);
        break;
    }
}
