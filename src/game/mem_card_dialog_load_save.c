#define GDIALOG_CHOICE_IN_DATA
#include "../types.h"
#include "../psyq/libmcrd.h"
#include "duel_effect.h"
#include "file_transfer.h"
#include "mem_card.h"
#include "mem_card_dialog_load_save.h"
#include "mem_card_dialog_steps.h"
#include "mem_card_directory.h"
#include "save_data.h"
#include "text_staging.h"
#include "dialog_choice.h"
#include "../unmatched.h"

/* The load half of the memory-card dialog's operations: the message helper,
   the load state machine and the two load step callbacks D_80090F9C selects.
   The save state machine and its step callback follow them. */

/* The two stores below are deliberate: retail writes the masked value and
   then the value with the new bits set. Without volatile the first store is
   dead and GCC drops it, so this function reaches the word through a volatile
   lvalue rather than forcing volatile on every reader in mem_card.h. */
#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MEM_CARD_DIALOG_SET_MESSAGE)
void MemCardDialog_SetMessage(s32 value, s32 bits)
{
    u16 flags = *(volatile u16 *)&gMemCard_wDialogFlags;

    bits |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
    D_8009B3C6 = value;
    flags &= 0xFF87;
    *(volatile u16 *)&gMemCard_wDialogFlags = flags;
    *(volatile u16 *)&gMemCard_wDialogFlags = flags | bits;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MEM_CARD_DIALOG_UPDATE_LOAD)
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

void MemCardDialog_UpdateLoad(void)
{
    s32 files;
    s32 message;

    switch (tent_MemCardDialogStepState & 0xF) {
    case 0:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xC8, 0x20);
            break;
        }
        tent_MemCardDialogStepState = 1;
        if (gDialog_bChoice != 0) {
            tent_MemCardDialogStepState = 9;
            break;
        }
        /* fallthrough */
    case 1:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_801D5648[0] = (tent_MemCardPort >> 4) + 1;
            message = 0xD4;
            if ((gMemCard_wDialogFlags & 0x200) != 0) {
                message = 0xC0;
            }
            MemCardDialog_SetMessage(message, 0);
            do {
            } while (MemCardAccept(tent_MemCardPort) == 0);
            goto io_pending;
        }
        switch (D_8009B3F4) {
        case 0:
        case 3:
            tent_MemCardDialogStepState = 2;
            break;
        case 1:
            tent_MemCardDialogStepState = 5;
            break;
        case 2:
        case 4:
            tent_MemCardDialogStepState = 6;
            break;
        }
        break;
    case 2:
        if (MemCardGetDirentry(tent_MemCardPort, (char *)tent_MemCardFileNameBuffer,
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            tent_MemCardDialogStepState = 6;
            break;
        }
        if (files == 0) {
            tent_MemCardDialogStepState = 6;
            break;
        }
        tent_MemCardDialogStepState = 3;
        /* fallthrough */
    case 3:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD5, 0);
            MemCardReadFile(tent_MemCardPort, (char *)tent_MemCardFileNameBuffer,
                            (unsigned long *)gMemCard_pPrimaryTransferCursor,
                            tent_MemCardTransferOffset,
                            tent_MemCardTransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        switch (D_8009B3F4) {
        case 0:
            tent_MemCardDialogStepState = 7;
            if (SaveData_ValidateIntegrity(gMemCard_pPrimaryTransferCursor) != 0) {
                break;
            }
            tent_MemCardDialogStepState = 4;
            if ((gMemCard_wDialogFlags & 0x400) != 0) {
                MemCardDialog_SetMessage(0xBD, 0x18);
                break;
            }
            gMemCard_wDialogFlags |= 0x400;
            break;
        case 1:
            tent_MemCardDialogStepState = 5;
            break;
        case 2:
        case 3:
            tent_MemCardDialogStepState = 8;
            break;
        case 4:
            break;
        case 5:
            tent_MemCardDialogStepState = 6;
            break;
        }
        break;
    case 4:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xBD, 0x10);
            break;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xBC, 0x20);
            break;
        }
        tent_MemCardDialogStepState = 9;
        if (gDialog_bChoice != 0) {
            break;
        }
        tent_MemCardDialogStepState = 3;
        tent_MemCardTransferOffset += SAVE_DATA_STATE_SIZE;
        break;
    case 5:
        MemCardDialog_SetMessage(0xC1, 0x18);
        break;
    case 6:
        MemCardDialog_SetMessage(0xD3, 0x18);
        break;
    case 7:
        D_8009B3EF = 1;
        if ((gMemCard_wDialogFlags & 0x200) != 0) {
            MemCardDialog_SetMessage(0xD0, 8);
            break;
        }
        D_8009B3D4 = 0;
        MemCardDialog_SetMessage(0xD0, 0x18);
        break;
    case 8:
        MemCardDialog_SetMessage(0xD3, 0x18);
        break;
    case 9:
        D_8009B3EF = 3;
        MemCardDialog_SetMessage(0xC7, 0x18);
        break;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MEM_CARD_DIALOG_STEP_LOAD)
void MemCardDialog_StepLoad(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        tent_MemCardDialogStepState = 0;
    }
    MemCardDialog_UpdateLoad();
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MEM_CARD_DIALOG_STEP_LOAD_UNPROMPTED)
void MemCardDialog_StepLoadUnprompted(void)
{
    if (!(D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        tent_MemCardDialogStepState = 1;
        gMemCard_wDialogFlags |= 0x200;
    }
    MemCardDialog_UpdateLoad();
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MEM_CARD_DIALOG_UPDATE_SAVE)
#ifdef VERSION_JAPAN
/* The Japanese save dialog. Against the US one below: with
   gMemCard_wDialogFlags & 0x100 set, six states show a different message and
   state 1 goes to state 2 instead of reading the directory; state 2 polls
   three calls the US build does not have, retrying three times; state 4's
   choice is the other way round; state 5 formats without accepting the card
   again; and state 7 skips the read-back when the flag is set. */
void MemCardDialog_UpdateSave(void)
{
    long cmds;
    long result;
    long apl;
    s32 files;
    s32 needed;
    s32 polled;
    s32 mode;
    s32 status;
    s32 message;

    switch (tent_MemCardDialogStepState & 0xF) {
    case 0:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            message = 0xC9;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xCA;
            }
            MemCardDialog_SetMessage(message, 0x20);
            break;
        }
        tent_MemCardDialogStepState = 1;
        if (gDialog_bChoice != 0) {
            tent_MemCardDialogStepState = 0xC;
            break;
        }
        /* fallthrough */
    case 1:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            message = 0xD4;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xBA;
            }
            MemCardDialog_SetMessage(message, 0);
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        switch (D_8009B3F4) {
        case 0:
        case 3:
            tent_MemCardDialogStepState = 3;
            if (gMemCard_wDialogFlags & 0x100) {
                tent_MemCardDialogStepState = 2;
            }
            break;
        case 4:
            tent_MemCardDialogStepState = 4;
            if (gMemCard_wDialogFlags & 0x100) {
                break;
            }
            if (D_8009B3D4 != 0) {
                break;
            }
            tent_MemCardDialogStepState = 0xE;
            break;
        case 1:
            tent_MemCardDialogStepState = 9;
            break;
        case 2:
            tent_MemCardDialogStepState = 0xD;
            break;
        }
        break;
    case 2:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_8009B3EC = 3;
            func_8008D070(0);
            break;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) != 0) {
            MemCardDialog_SetMessage(0xB8, 0x18);
            break;
        }
        polled = func_8008DCB4(1, &cmds, &result);
        if (polled != 1) {
            break;
        }
        if (result == 0) {
            func_8008CF10(0, &apl);
            if (func_8008DCB4(0, &cmds, &result) != polled) {
                MemCardDialog_SetMessage(0xBB, 0x18);
                break;
            }
            if (apl != 0) {
                tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardDialog_SetMessage(0xB9, 0x10);
                break;
            }
            tent_MemCardDialogStepState = 3;
            break;
        }
        if (--D_8009B3EC != 0) {
            func_8008D070(0);
            break;
        }
        MemCardDialog_SetMessage(0xBB, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)D_8009AF70,
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            tent_MemCardDialogStepState = 0xD;
            break;
        }
        if (MemCard_FindEntry(tent_MemCardFileNameBuffer, (struct DIRENTRY *)D_800EFBC0, files) >= 0) {
            tent_MemCardDialogStepState = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && D_8009B3D4 == 0) {
            tent_MemCardDialogStepState = 0xE;
            break;
        }
        message = MemCard_CalcFreeBlocks((struct DIRENTRY *)D_800EFBC0, files);
        needed = tent_MemCardBlockCount;
        if (message >= needed) {
            tent_MemCardDialogStepState = 6;
            goto create;
        }
        D_801D5608[0].blocks.used = MEM_CARD_BLOCK_COUNT - message;
        D_801D5608[0].blocks.needed = needed;
        MemCardDialog_SetMessage(0xDB, 0x18);
        break;
    case 4:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xDE, 0x10);
            break;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xDF, 0x20);
            break;
        }
        if (gDialog_bChoice != 0) {
            tent_MemCardDialogStepState = 0xC;
            break;
        }
        tent_MemCardDialogStepState = 5;
        MemCardDialog_SetMessage(0xBE, 0);
        break;
    case 5:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            break;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (MemCardFormat(0) != 0) {
                MemCardDialog_SetMessage(0xDD, 0x18);
                break;
            }
            MemCardDialog_SetMessage(0xBF, 0x10);
            break;
        }
        tent_MemCardDialogStepState = 1;
        break;
    case 6:
    create:
        if (MemCardCreateFile(0, (char *)tent_MemCardFileNameBuffer, tent_MemCardBlockCount) != 0) {
            tent_MemCardDialogStepState = 0xB;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            tent_MemCardTransferOffset = 0;
            gMemCard_pPrimaryTransferCursor -= SAVE_DATA_HEADER_SIZE;
            tent_MemCardTransferSize += SAVE_DATA_HEADER_SIZE;
        }
        tent_MemCardDialogStepState = 8;
        goto write;
    case 7:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            if (gMemCard_wDialogFlags & 0x100) {
                tent_MemCardDialogStepState = 8;
                break;
            }
            D_8009B3EC = 0;
            MemCardReadFile(0, (char *)tent_MemCardFileNameBuffer,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            tent_MemCardTransferOffset, 0x480);
            goto io_pending;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            mode = D_8009B3F4;
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (mode != 0) {
                tent_MemCardDialogStepState = 0xD;
                if (mode == 1) {
                    tent_MemCardDialogStepState = 9;
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
                tent_MemCardDialogStepState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardReadFile(0, (char *)tent_MemCardFileNameBuffer,
                                (unsigned long *)gLibrary_aCardArtRecord,
                                tent_MemCardTransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
                goto io_pending;
            }
            break;
        state_e:
            tent_MemCardDialogStepState = 0xE;
            break;
        message_cf:
            MemCardDialog_SetMessage(0xCF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            tent_MemCardDialogStepState = 8;
            goto write;
        }
        tent_MemCardDialogStepState = 0xC;
        break;
    case 8:
    write:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            message = 0xD6;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xD7;
            }
            MemCardDialog_SetMessage(message, 0);
            MemCardWriteFile(0, (char *)tent_MemCardFileNameBuffer,
                             (unsigned long *)gMemCard_pPrimaryTransferCursor,
                             tent_MemCardTransferOffset,
                             tent_MemCardTransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        status = D_8009B3F4;
        if (status != 0) {
            tent_MemCardDialogStepState = 0xB;
            if (status == 1) {
                tent_MemCardDialogStepState = 9;
            }
            break;
        }
        tent_MemCardDialogStepState = 0xA;
        break;
    case 9:
        message = 0xD2;
        if (gMemCard_wDialogFlags & 0x100) {
            message = 0xBB;
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 10:
        D_8009B3EF = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            D_8009B3D4 = 0;
            gSaveDataSequence = (s32)((u32)gSaveDataSequence + 1);
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 11:
        message = 0xD9;
        if (gMemCard_wDialogFlags & 0x100) {
            message = 0xDC;
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 12:
        message = 0xCD;
        if (gMemCard_wDialogFlags & 0x100) {
            message = 0xCB;
        }
        MemCardDialog_SetMessage(message, 0x18);
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
#else
/* Memory-card save dialog state machine, the third of the family beside
   MemCardDialog_UpdateLoad and MemCardDialog_UpdateTradeSave.

   It walks the card from "no save present" to "save written": accept the
   card, read the directory, look the file up, compare free blocks against
   the size the save needs, offer and run a format, create the file, read
   back and check the existing state, and finally write it. The low nibble of
   tent_MemCardDialogStepState is the state and the 0x80/0x40 bits latch the first pass
   through each one; D_8009B3F4 carries the previous call's result and
   tent_MemCardDialogStepState is set to the message state on every failure.

   Levers that mattered here:
   - The three jump tables follow the load table in this unit's contiguous
     rodata run starting at 0xC10.
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
   - One local carries both the free-block count and the state-10 message
     id. Its longer life keeps it out of $a0, so retail's $a2 copy into $a0 at
     the shared MemCardDialog_SetMessage call survives; MemCardDialog_SetMessage
     takes full words, so the count's unknown upper bits need no mask there. */
void MemCardDialog_UpdateSave(void)
{
    s32 files;
    s32 needed;
    s32 result;
    s32 mode;
    s32 message;

    switch (tent_MemCardDialogStepState & 0xF) {
    case 0:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_801D5648[0] = (tent_MemCardPort >> 4) + 1;
            MemCardDialog_SetMessage(0xC9, 0x20);
            break;
        }
        tent_MemCardDialogStepState = 1;
        if (gDialog_bChoice != 0) {
            tent_MemCardDialogStepState = 0xC;
            break;
        }
        /* fallthrough */
    case 1:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD4, 0);
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        switch (D_8009B3F4) {
        case 0:
        case 3:
            tent_MemCardDialogStepState = 3;
            break;
        case 4:
            tent_MemCardDialogStepState = 4;
            if (gMemCard_wDialogFlags & 0x100) {
                break;
            }
            if (D_8009B3D4 != 0) {
                break;
            }
            tent_MemCardDialogStepState = 0xE;
            break;
        case 1:
            tent_MemCardDialogStepState = 9;
            break;
        case 2:
            tent_MemCardDialogStepState = 0xD;
            break;
        }
        break;
    case 2:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_8009B3EC = 3;
            break;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            break;
        }
        MemCardDialog_SetMessage(0xB8, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)D_8009AF70,
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            tent_MemCardDialogStepState = 0xD;
            break;
        }
        if (MemCard_FindEntry(tent_MemCardFileNameBuffer, (struct DIRENTRY *)D_800EFBC0, files) >= 0) {
            tent_MemCardDialogStepState = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && D_8009B3D4 == 0) {
            tent_MemCardDialogStepState = 0xE;
            break;
        }
        message = MemCard_CalcFreeBlocks((struct DIRENTRY *)D_800EFBC0, files);
        needed = tent_MemCardBlockCount;
        if (message >= needed) {
            tent_MemCardDialogStepState = 6;
            goto create;
        }
        D_801D5608[0].blocks.used = MEM_CARD_BLOCK_COUNT - message;
        D_801D5608[0].blocks.needed = needed;
        MemCardDialog_SetMessage(0xDB, 0x18);
        break;
    case 4:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xDE, 0x10);
            break;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xDF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            tent_MemCardDialogStepState = 0xC;
            break;
        }
        tent_MemCardDialogStepState = 5;
        MemCardDialog_SetMessage(0xBE, 0);
        break;
    case 5:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
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
            tent_MemCardDialogStepState = 0xD;
            break;
        case 4:
            if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    MemCardDialog_SetMessage(0xDD, 0x18);
                    break;
                }
                MemCardDialog_SetMessage(0xBF, 0x10);
                break;
            }
            tent_MemCardDialogStepState = 1;
            break;
        case 1:
            tent_MemCardDialogStepState = 9;
            break;
        }
        break;
    case 6:
    create:
        if (MemCardCreateFile(0, (char *)tent_MemCardFileNameBuffer, tent_MemCardBlockCount) != 0) {
            tent_MemCardDialogStepState = 0xB;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            tent_MemCardTransferOffset = 0;
            gMemCard_pPrimaryTransferCursor -= SAVE_DATA_HEADER_SIZE;
            tent_MemCardTransferSize += SAVE_DATA_HEADER_SIZE;
        }
        tent_MemCardDialogStepState = 8;
        goto write;
    case 7:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_8009B3EC = 0;
            MemCardReadFile(0, (char *)tent_MemCardFileNameBuffer,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            tent_MemCardTransferOffset, 0x480);
            goto io_pending;
        }
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            mode = D_8009B3F4;
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (mode != 0) {
                tent_MemCardDialogStepState = 0xD;
                if (mode == 1) {
                    tent_MemCardDialogStepState = 9;
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
                tent_MemCardDialogStepState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardReadFile(0, (char *)tent_MemCardFileNameBuffer,
                                (unsigned long *)gLibrary_aCardArtRecord,
                                tent_MemCardTransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
                goto io_pending;
            }
            break;
        state_e:
            tent_MemCardDialogStepState = 0xE;
            break;
        message_cf:
            MemCardDialog_SetMessage(0xCF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            tent_MemCardDialogStepState = 8;
            goto write;
        }
        tent_MemCardDialogStepState = 0xC;
        break;
    case 8:
    write:
        if ((tent_MemCardDialogStepState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            tent_MemCardDialogStepState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD6, 0);
            MemCardWriteFile(0, (char *)tent_MemCardFileNameBuffer,
                             (unsigned long *)gMemCard_pPrimaryTransferCursor,
                             tent_MemCardTransferOffset,
                             tent_MemCardTransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        result = D_8009B3F4;
        if (result != 0) {
            tent_MemCardDialogStepState = 0xB;
            if (result == 1) {
                tent_MemCardDialogStepState = 9;
            }
            break;
        }
        tent_MemCardDialogStepState = 0xA;
        break;
    case 9:
        MemCardDialog_SetMessage(0xD2, 0x18);
        break;
    case 10:
        D_8009B3EF = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            D_8009B3D4 = 0;
            gSaveDataSequence = (s32)((u32)gSaveDataSequence + 1);
        }
        MemCardDialog_SetMessage(message, 0x18);
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
#endif
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MEM_CARD_DIALOG_STEP_SAVE)
void MemCardDialog_StepSave(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        tent_MemCardDialogStepState = 0;
    }
    MemCardDialog_UpdateSave();
}
#endif
