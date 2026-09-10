#include "../types.h"
#include "../psyq/libmcrd.h"
#include "duel_effect.h"
#include "file_transfer.h"
#include "mem_card.h"
#include "mem_card_dialog_load_save.h"
#include "mem_card_directory.h"
#include "save_data.h"
#include "../unmatched.h"

/* The load/save operation half of the memory-card dialog. The message helper
   feeds both machines, and the small step callbacks select and initialize
   them through D_80090F9C. */

/* The two stores below are deliberate: retail writes the masked value and
   then the value with the new bits set. Without volatile the first store is
   dead and GCC drops it, so this unit reaches the word under a second linker
   name rather than forcing volatile on every reader in mem_card.h. */
extern volatile u16 gMemCard_wDialogFlags_v asm("gMemCard_wDialogFlags");

extern s8 gDialog_bChoice __attribute__((section(".data")));
extern u8 D_8009AF70[];
extern s32 D_801D5608[];

/* The save machine needs the historical wide caller contract to retain the
   register-to-argument move for its state-10 message. */
extern void MemCardDialog_SetMessage_wide(s32 value, s32 bits)
    asm("MemCardDialog_SetMessage");

void MemCardDialog_SetMessage(u8 value, u16 bits)
{
    u16 flags = gMemCard_wDialogFlags_v;

    bits |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
    D_8009B3C6 = value;
    flags &= 0xFF87;
    gMemCard_wDialogFlags_v = flags;
    gMemCard_wDialogFlags_v = flags | bits;
}

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

    switch (D_8009B3EB & 0xF) {
    case 0:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xC8, 0x20);
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
            MemCardDialog_SetMessage(message, 0);
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
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
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
            MemCardDialog_SetMessage(0xD5, 0);
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
                MemCardDialog_SetMessage(0xBD, 0x18);
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
            MemCardDialog_SetMessage(0xBD, 0x10);
            break;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xBC, 0x20);
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

void MemCardDialog_StepLoad(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        D_8009B3EB = 0;
    }
    MemCardDialog_UpdateLoad();
}

void MemCardDialog_StepLoadUnprompted(void)
{
    if (!(D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        D_8009B3EB = 1;
        gMemCard_wDialogFlags |= 0x200;
    }
    MemCardDialog_UpdateLoad();
}

/* Memory-card save dialog state machine, the third of the family beside
   MemCardDialog_UpdateLoad and MemCardDialog_UpdateTradeSave.

   It walks the card from "no save present" to "save written": accept the
   card, read the directory, look the file up, compare free blocks against
   the size the save needs, offer and run a format, create the file, read
   back and check the existing state, and finally write it. The low nibble of
   D_8009B3EB is the state and the 0x80/0x40 bits latch the first pass
   through each one; D_8009B3F4 carries the previous call's result and
   D_8009B3EB is set to the message state on every failure.

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
   - The state-10 message id uses the wide same-symbol alias above. It is the
     only argument in the function that is not a constant, retail keeps it in
     $a2 and copies it to $a0 at the shared call; the narrow declaration would
     let gcc allocate it straight into $a0 and drop the copy. */

void MemCardDialog_UpdateSave(void)
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
            MemCardDialog_SetMessage_wide(0xC9, 0x20);
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
            MemCardDialog_SetMessage_wide(0xD4, 0);
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
        MemCardDialog_SetMessage_wide(0xB8, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)D_8009AF70,
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            D_8009B3EB = 0xD;
            break;
        }
        if (MemCard_FindEntry(D_800EFE18, D_800EFBC0, files) >= 0) {
            D_8009B3EB = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && D_8009B3D4 == 0) {
            D_8009B3EB = 0xE;
            break;
        }
        free_blocks = MemCard_CalcFreeBlocks(D_800EFBC0, files);
        needed = D_8009B3DC;
        if (free_blocks >= needed) {
            D_8009B3EB = 6;
            goto create;
        }
        D_801D5608[0] = MEM_CARD_BLOCK_COUNT - free_blocks;
        D_801D5608[1] = needed;
        MemCardDialog_SetMessage_wide(0xDB, 0x18);
        break;
    case 4:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage_wide(0xDE, 0x10);
            break;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage_wide(0xDF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            D_8009B3EB = 0xC;
            break;
        }
        D_8009B3EB = 5;
        MemCardDialog_SetMessage_wide(0xBE, 0);
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
            MemCardDialog_SetMessage_wide(0xC3, 0x18);
            break;
        case 3:
            MemCardDialog_SetMessage_wide(0xC3, 0x18);
            break;
        case 2:
            D_8009B3EB = 0xD;
            break;
        case 4:
            if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    MemCardDialog_SetMessage_wide(0xDD, 0x18);
                    break;
                }
                MemCardDialog_SetMessage_wide(0xBF, 0x10);
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
            MemCardDialog_SetMessage_wide(0xCF, 0x20);
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
            MemCardDialog_SetMessage_wide(0xD6, 0);
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
        MemCardDialog_SetMessage_wide(0xD2, 0x18);
        break;
    case 10:
        D_8009B3EF = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            D_8009B3D4 = 0;
            gSaveDataSequence++;
        }
        MemCardDialog_SetMessage_wide(message, 0x18);
        break;
    case 11:
        MemCardDialog_SetMessage_wide(0xD9, 0x18);
        break;
    case 12:
        MemCardDialog_SetMessage_wide(0xCD, 0x18);
        D_8009B3EF = 3;
        break;
    case 13:
        MemCardDialog_SetMessage_wide(0xDA, 0x18);
        break;
    case 14:
        MemCardDialog_SetMessage_wide(0xCE, 0x18);
        break;
    }
}

void MemCardDialog_StepSave(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        D_8009B3EB = 0;
    }
    MemCardDialog_UpdateSave();
}

void MemCardDialog_StepNone(void)
{
}
