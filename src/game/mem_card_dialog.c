/* The memory-card dialog and the save-transfer runtime built on it: 22
 * contiguous functions from the low-level card controller MemCardWork_UpdateOperation
 * (0x8003DC1C) through SaveData_RequestTradeWrite. The first fifteen drive
 * the card controller and the load/save/trade dialog; the last seven
 * request, poll and apply single-player and two-player save transfers
 * through that dialog. */
#define GDIALOG_CHOICE_IN_DATA
#define MEM_CARD_DIRECTORY_IN_DATA
#define MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE

#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800EF6D0-0x800EFE38, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 D_800EF6D0[0x4F0];
u8 gMemCardDialog_DirectoryBuffer[600];
u8 gMemCardDialog_FileNameBuffer[32];

#include "text_box_blocking_build.h"
#include "mem_card_dialog.h"

/* Zero-initialized storage this unit owns at 0x8009B3BC-0x8009B400, defined in
 * address order ahead of the headers that declare it (#2602). */
s32 gMemCardDialog_SyncStatus;
u8 gSaveData_PairMessageCode;
u8 D_8009B3C1;
u16 gMemCardDialog_TransferSize;
u16 gMemCardDialog_TransferOffset;
u8 gMemCardDialog_MessageCode;
u8 gMemCardWork_OperationMode;
s32 gMemCardWork_nRequestResult;
u16 gMemCardWork_wFlags;
static u8 D_8009B3CE;
u8 gMemCardWork_OperationState;
u8 *G32 gMemCard_pPrimaryTransferCursor;
u8 gSaveData_AllowIdentityBypass;
u8 gMemCardWork_bOperationResult;
DisplayObject *G32 gMemCard_pDialogObject;
u8 gMemCardDialog_BlockCount;
u8 gMemCardWork_bSlotIndex;
u8 gMemCardDialog_StepIndex;
u8 *G32 gMemCard_pSecondaryTransferCursor;
s32 gMemCardWork_nCompletedRequest;
static u16 D_8009B3E8;
u8 gSaveData_LoadPairState;
u8 gMemCardDialog_OperationState;
u8 gMemCardDialog_RetryCounter;
u8 gSaveData_PairSetupState;
u8 gMemCardDialog_ChannelIndex;
u8 gMemCardDialog_Outcome;
s32 gMemCardDialog_SyncCommand;
s32 gMemCardDialog_SyncResult;
static u8 D_8009B3F8;
u8 gMemCardDialog_Port;
u16 gMemCard_wDialogFlags;
static u32 D_8009B3FC;

#include "display_object.h"
#include "display_object_interpolation.h"
#include "display_object_lifecycle.h"
#include "display_object_core.h"
#include "../psyq/libmcrd.h"
#include "../psyq/strings.h"
#include "data_transfer_request.h"
#include "duel_effect.h"
#include "dialog_callback_state.h"
#include "text_channel_state.h"
#include "mem_card.h"
#include "save_data.h"
#include "save_pair_workspace.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "file_transfer.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "text_box_update_channels.h"
#include "duel_effect_mark_object_if_active.h"
#include "mem_card_dialog_steps.h"
#include "dialog_layout.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/stdio.h"
#include "mem_card_directory.h"
#include "mem_card_directory_patterns.h"
#include "mem_card_work.h"
#include "mem_card_work_state.h"
#include "mem_card_diagnostic_strings.h"
#include "io_event_helpers.h"
#include "duel_effect_create_channel.h"
#include "dialog_choice.h"
#include "file_constants.h"
#include "util_memory.h"
#include "card_constants.h"
#include "input.h"
#include "save_data_update_trade_load.h"
#include "save_data_update_load_pair.h"
#include "text_sjis_to_glyph_codes.h"
#include "two_player_save_setup.h"
#include "sound.h"
#include "../unmatched.h"
#include "paired_player_name_glyphs.h"
#include "save_pair_transfer_views.h"

/* Regional value: the completion mask it tests (0x2008 -> 0x2010). The
 * European build (src/game/european/) defines its own. */
#ifndef MEM_CARD_MESSAGE_COMPLETION_MASK
#define MEM_CARD_MESSAGE_COMPLETION_MASK 0x2008
#endif

void MemCardWork_UpdateOperation(void)
{
    MemCardWorkRoot *root = &gMemCardWork_Area.root;
    MemCardWorkSlot *slot = &gMemCardWork_Area.slots[gMemCardWork_bSlotIndex];
    s32 zero = 0;
    s32 status;
    s32 index;
    s32 offset;
    s32 remaining;
    u8 *cursor;
    s32 message;
    s32 checksum;
    u8 *name;

    if (gMemCardWork_wFlags & 0x2000) {
        index = zero;
        if (index < gMemCard_nDirEntries) {
            do {
#ifndef VERSION_EUROPE
                FntPrint(gMemCard_szDirectoryEntryFormat, &gMemCard_pDirEntries[index]);
#endif
                index++;
            } while (index < gMemCard_nDirEntries);
        }
#ifndef VERSION_EUROPE
        FntPrint(gMemCard_szFreeBlockCountFormat, slot->free_blocks);
#endif
    }
    status = -1;
    if (gMemCardWork_wFlags & 0x4000)
        status = MemCard_ProcessRequest(1, &gMemCardWork_nCompletedRequest, &gMemCardWork_nRequestResult);
    if ((gMemCardWork_wFlags & 0x1000) && status != 0 && status == 1) {
        switch (gMemCardWork_nCompletedRequest) {
        case 2:
            gMemCardWork_wFlags |= 0x2000;
            switch (gMemCardWork_nRequestResult) {
            case 3:
                gMemCardWork_wFlags |= 0x800;
            case 0:
                slot->free_blocks = (u8)gMemCard_nFreeBlocks;
                goto success;
            case 4:
                gMemCardWork_wFlags |= 2;
                gMemCardWork_bOperationResult = 1;
                break;
            case 1:
            default:
                gMemCardWork_bOperationResult = 2;
                break;
            }
            break;
        case 3:
            if (gMemCardWork_nRequestResult == 0) {
                index = root->chunk;
                remaining = (u16)root->remaining - index;
                root->remaining = remaining;
                if ((s32)((u32)remaining << 16) > 0) {
                    slot->cursor += index;
                    root->offset += index;
                    if (root->remaining < root->chunk)
                        root->chunk = (u16)root->remaining;
                } else {
                    gMemCardWork_bOperationResult = 0;
                }
            } else {
                gMemCardWork_bOperationResult = 1;
            }
            break;
        case 4:
            if (gMemCardWork_nRequestResult == 0) {
                index = root->chunk;
                remaining = (u16)root->remaining - index;
                root->remaining = remaining;
                if ((s32)((u32)remaining << 16) > 0) {
                    slot->cursor += index;
                    root->offset += index;
                    if (root->remaining < root->chunk)
                        root->chunk = (u16)root->remaining;
                } else {
                    gMemCardWork_bOperationResult = 0;
                }
            } else {
                gMemCardWork_bOperationResult = 1;
            }
            break;
        case 8:
        case 11:
        case 12:
            gMemCardWork_bOperationResult = 1;
            if (gMemCardWork_nRequestResult)
                break;
success:
            gMemCardWork_bOperationResult = 0;
            break;
        }
    }
    if (gDialog_bCallbackState & 0xF) {
        status = 1;
        CALL32(void (*)(MemCardWorkRoot *, MemCardWorkSlot *), gDialog_apfnTransitionSteps[gDialog_bCallbackState & 0xF])(root, slot);
    }
    if (status >= 0)
        return;
    if (gMemCardWork_wFlags & 0x400) {
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
        DuelEffectChannel *text = (DuelEffectChannel *)(
            (u8 *)gText_aChannels +
            root->text_index * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
        DuelEffectChannel *text = &gText_aChannels[root->text_index];
#endif
        TextBox_UpdateChannels();
        if ((*(u32 *)&text->flags_34 & MEM_CARD_MESSAGE_COMPLETION_MASK) == 0x2000)
            gMemCardWork_wFlags &= ~0x400;
        return;
    }
    switch (gMemCardWork_OperationState & 0xF) {
    case 0:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            gDialog_bCallbackState = 1;
            break;
        }
        if (gDialog_bChoice) {
            gDialog_bCallbackState = 2;
            break;
        }
        MemCard_Init(1);
        MemCard_InitIOEvents();
        gMemCardWork_OperationState = 1;
        gMemCardWork_wFlags |= 0x4000;
    case 1:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            gMemCardWork_wFlags |= 0x1000;
            MemCard_ReqLoadDirectory(zero);
            break;
        }
        gMemCardWork_OperationState = 4;
    case 4:
        if (!(gMemCardWork_wFlags & 0x2000))
            break;
        gMemCardWork_wFlags &= ~0x1000;
        if (gMemCardWork_bOperationResult == 2) {
            message = 0xD2;
            goto show_message;
        }
        if (!(gMemCardWork_OperationMode & 1))
            gMemCardWork_OperationState = 5;
        else
            gMemCardWork_OperationState = 7;
        break;
    case 5:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            index = MemCard_FindLoadedEntry(gMemCard_szSaveFileName);
            if (index < 0 || (gMemCardWork_wFlags & 2)) {
                message = 0xD3;
                goto show_message;
            }
            MemCard_ReqReadSector(zero, MEM_CARD_WORK_FRAME_ADDRESS,
                gMemCard_pDirEntries[index].head / 64);
            break;
        }
        if (gMemCardWork_bOperationResult) {
            message = 0xD8;
            goto show_message;
        }
        gMemCardWork_OperationState = 6;
    case 6:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            name = gMemCard_szSaveFileName;
            index = MemCard_FindLoadedEntry(name);
            message = 0xD3;
            if (index < 0 || (gMemCardWork_wFlags & 2))
                goto show_message;
            DuelEffect_CreateChannel(0x80D5, 0);
            slot->entry_index = index;
            gMemCardWork_wFlags |= 0x1000;
            MemCard_ReqReadFile(
                zero,
                (s32)name,
                MEM_CARD_WORK_READ_ADDRESS,
                SAVE_DATA_HEADER_SIZE,
                MEM_CARD_BLOCK_SIZE - SAVE_DATA_HEADER_SIZE
            );
            break;
        }
        message = 0xD7;
        if (gMemCardWork_bOperationResult == 1)
            message = 0xD8;
        goto show_message;
    case 7: {
        s32 wait;
        s32 result;
        u8 *file;
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            if (gMemCardWork_wFlags & 2) {
                gMemCardWork_OperationState = 9;
                break;
            }
            file = gMemCard_szSaveFileName;
            if (MemCard_FindLoadedEntry(file) < 0) {
                gMemCardWork_OperationState = 10;
                break;
            }
            DuelEffect_CreateChannel(0x80D6, 0);
            Util_CopyWords(
                (u8 *)MEM_CARD_WORK_WRITE_ADDRESS,
                gSaveData_aHeaderTemplate,
                SAVE_DATA_HEADER_SIZE
            );
            Util_FillMemory(
                (u8 *)MEM_CARD_WORK_WRITE_BODY_ADDRESS,
                0x86,
                FILE_SECTOR_SIZE
            );
            MemCard_ReqWriteFile(
                zero,
                (s32)file,
                MEM_CARD_WORK_WRITE_ADDRESS,
                0,
                SAVE_DATA_HEADER_SIZE + FILE_SECTOR_SIZE
            );
            gMemCardWork_wFlags |= 0x1000;
            break;
        }
        result = gMemCardWork_bOperationResult;
        if (result == 1) {
            message = 0xD9;
            wait = result;
        } else {
            message = 0xD7;
            wait = 1;
            gMemCardWork_OperationState = 8;
        }
        DuelEffect_CreateChannel(message, wait);
        remaining = gMemCardWork_wFlags | 0x400;
        gMemCardWork_wFlags = remaining;
        remaining = 8;
        gMemCardWork_OperationState = remaining;
    }
    case 8:
        if (!(gMemCardWork_OperationState & 0x80)) {
            s32 i;
            s32 sector;
            gMemCardWork_OperationState |= 0x80;
            index = MemCard_FindLoadedEntry(gMemCard_szSaveFileName);
            checksum = 0;
            i = 0;
            D_8021007E = 1;
            D_8021007D = 255;
            D_8021007C = 255;
            D_8021007B = 255;
            D_8021007A = 255;
            do {
                checksum ^= MEM_CARD_WORK_FRAME_BYTES[i];
                i++;
            } while (i < 0x7F);
            sector = gMemCard_pDirEntries[index].head;
            gMemCardWork_bFrameChecksum = checksum;
            if (sector < 0)
                sector += 63;
            MemCard_ReqWriteSector(zero, MEM_CARD_WORK_FRAME_ADDRESS, sector >> 6);
            break;
        }
        if (!gMemCardWork_bOperationResult)
            break;
        message = 0xD9;
show_message:
        DuelEffect_CreateChannel(message, 1);
        gMemCardWork_wFlags |= 0x400;
        break;
    case 9:
    case 10:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            if (slot->free_blocks >= root->blocks)
                MemCard_ReqCreateFile(zero, (s32)gMemCard_szSaveFileName, root->blocks);
            break;
        }
#ifndef VERSION_EUROPE
        if (!gMemCardWork_bOperationResult)
            printf(gMemCard_szMakeFileTrace);
        printf(gMemCard_szFailFileTrace);
#endif
        break;
    }
}

/* The load half of the memory-card dialog's operations: the message helper,
   the load state machine and the two load step callbacks gMemCardDialog_apfnSteps selects.
   The save state machine and its step callback follow them. */

/* The two stores below are deliberate: retail writes the masked value and
   then the value with the new bits set. Without volatile the first store is
   dead and GCC drops it, so this function reaches the word through a volatile
   lvalue rather than forcing volatile on every reader in mem_card.h. */
void MemCardDialog_SetMessage(s32 value, s32 bits)
{
    u16 flags = *(volatile u16 *)&gMemCard_wDialogFlags;

    bits |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
    gMemCardDialog_MessageCode = value;
    flags &= 0xFF87;
    *(volatile u16 *)&gMemCard_wDialogFlags = flags;
    *(volatile u16 *)&gMemCard_wDialogFlags = flags | bits;
}

/* Memory-card load dialog state machine.

   Levers that mattered here:
   - gMemCardDialog_CardNumberArg must be declared as an unsized array. As a plain s32 extern
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

    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xC8, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 1;
        if (gDialog_bChoice != 0) {
            gMemCardDialog_OperationState = 9;
            break;
        }
        /* fallthrough */
    case 1:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            message = 0xD4;
            if ((gMemCard_wDialogFlags & 0x200) != 0) {
                message = 0xC0;
            }
            MemCardDialog_SetMessage(message, 0);
            do {
            } while (MemCardAccept(gMemCardDialog_Port) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
        case 3:
            gMemCardDialog_OperationState = 2;
            break;
        case 1:
            gMemCardDialog_OperationState = 5;
            break;
        case 2:
        case 4:
            gMemCardDialog_OperationState = 6;
            break;
        }
        break;
    case 2:
        if (MemCardGetDirentry(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (PSXLONG *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            gMemCardDialog_OperationState = 6;
            break;
        }
        if (files == 0) {
            gMemCardDialog_OperationState = 6;
            break;
        }
        gMemCardDialog_OperationState = 3;
        /* fallthrough */
    case 3:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD5, 0);
            MemCardReadFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned PSXLONG *)gMemCard_pPrimaryTransferCursor,
                            gMemCardDialog_TransferOffset,
                            gMemCardDialog_TransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
            gMemCardDialog_OperationState = 7;
            if (SaveData_ValidateIntegrity(gMemCard_pPrimaryTransferCursor) != 0) {
                break;
            }
            gMemCardDialog_OperationState = 4;
            if ((gMemCard_wDialogFlags & 0x400) != 0) {
                MemCardDialog_SetMessage(0xBD, 0x18);
                break;
            }
            gMemCard_wDialogFlags |= 0x400;
            break;
        case 1:
            gMemCardDialog_OperationState = 5;
            break;
        case 2:
        case 3:
            gMemCardDialog_OperationState = 8;
            break;
        case 4:
            break;
        case 5:
            gMemCardDialog_OperationState = 6;
            break;
        }
        break;
    case 4:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xBD, 0x10);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xBC, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 9;
        if (gDialog_bChoice != 0) {
            break;
        }
        gMemCardDialog_OperationState = 3;
        gMemCardDialog_TransferOffset += SAVE_DATA_STATE_SIZE;
        break;
    case 5:
        MemCardDialog_SetMessage(0xC1, 0x18);
        break;
    case 6:
        MemCardDialog_SetMessage(0xD3, 0x18);
        break;
    case 7:
        gMemCardDialog_Outcome = 1;
        if ((gMemCard_wDialogFlags & 0x200) != 0) {
            MemCardDialog_SetMessage(0xD0, 8);
            break;
        }
        gSaveData_AllowIdentityBypass = 0;
        MemCardDialog_SetMessage(0xD0, 0x18);
        break;
    case 8:
        MemCardDialog_SetMessage(0xD3, 0x18);
        break;
    case 9:
        gMemCardDialog_Outcome = 3;
        MemCardDialog_SetMessage(0xC7, 0x18);
        break;
    }
}

void MemCardDialog_StepLoad(void)
{
    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_OperationState = 0;
    }
    MemCardDialog_UpdateLoad();
}

void MemCardDialog_StepLoadUnprompted(void)
{
    if (!(gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_OperationState = 1;
        gMemCard_wDialogFlags |= 0x200;
    }
    MemCardDialog_UpdateLoad();
}

#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
/* The Japanese save dialog and European derivative. Against the US one below: with
   gMemCard_wDialogFlags & 0x100 set, six states show a different message and
   state 1 goes to state 2 instead of reading the directory; state 2 polls
   three calls the US build does not have, retrying three times; state 4's
   choice is the other way round; state 5 formats without accepting the card
   again; and state 7 skips the read-back when the flag is set. Europe keeps
   the message, state 2 and read-back differences, but restores the port
   marker, state 4 choice direction and state 5 flow used by the US build. */
void MemCardDialog_UpdateSave(void)
{
    PSXLONG cmds;
    PSXLONG result;
    PSXLONG apl;
    s32 files;
    s32 needed;
    s32 polled;
    s32 mode;
    s32 status;
    s32 message;

    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            message = 0xC9;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xCA;
            }
            MemCardDialog_SetMessage(message, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 1;
        if (gDialog_bChoice != 0) {
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        /* fallthrough */
    case 1:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
#ifdef VERSION_EUROPE
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
#endif
            message = 0xD4;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xBA;
            }
            MemCardDialog_SetMessage(message, 0);
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
        case 3:
            gMemCardDialog_OperationState = 3;
            if (gMemCard_wDialogFlags & 0x100) {
                gMemCardDialog_OperationState = 2;
            }
            break;
        case 4:
            gMemCardDialog_OperationState = 4;
            if (gMemCard_wDialogFlags & 0x100) {
                break;
            }
            if (gSaveData_AllowIdentityBypass != 0) {
                break;
            }
            gMemCardDialog_OperationState = 0xE;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        break;
    case 2:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 3;
            func_8008D070(0);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) != 0) {
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
                gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardDialog_SetMessage(0xB9, 0x10);
                break;
            }
            gMemCardDialog_OperationState = 3;
            break;
        }
        if (--gMemCardDialog_RetryCounter != 0) {
            func_8008D070(0);
            break;
        }
        MemCardDialog_SetMessage(0xBB, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)gMemCardDialog_szAllFilesPattern,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (PSXLONG *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        if (MemCard_FindEntry(gMemCardDialog_FileNameBuffer, (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files) >= 0) {
            gMemCardDialog_OperationState = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && gSaveData_AllowIdentityBypass == 0) {
            gMemCardDialog_OperationState = 0xE;
            break;
        }
        message = MemCard_CalcFreeBlocks((struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files);
        needed = gMemCardDialog_BlockCount;
        if (message >= needed) {
            gMemCardDialog_OperationState = 6;
            goto create;
        }
        gText_StagedValues[0].blocks.used = MEM_CARD_BLOCK_COUNT - message;
        gText_StagedValues[0].blocks.needed = needed;
        MemCardDialog_SetMessage(0xDB, 0x18);
        break;
    case 4:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xDE, 0x10);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xDF, 0x20);
            break;
        }
#ifdef VERSION_EUROPE
        if (gDialog_bChoice == 0) {
#else
        if (gDialog_bChoice != 0) {
#endif
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        gMemCardDialog_OperationState = 5;
        MemCardDialog_SetMessage(0xBE, 0);
        break;
    case 5:
#ifdef VERSION_EUROPE
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        if (gMemCardDialog_SyncCommand != 2) {
            break;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 3:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        case 4:
            if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    MemCardDialog_SetMessage(0xDD, 0x18);
                    break;
                }
                MemCardDialog_SetMessage(0xBF, 0x10);
                break;
            }
            gMemCardDialog_OperationState = 1;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        }
#else
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (MemCardFormat(0) != 0) {
                MemCardDialog_SetMessage(0xDD, 0x18);
                break;
            }
            MemCardDialog_SetMessage(0xBF, 0x10);
            break;
        }
        gMemCardDialog_OperationState = 1;
#endif
        break;
    case 6:
    create:
        if (MemCardCreateFile(0, (char *)gMemCardDialog_FileNameBuffer, gMemCardDialog_BlockCount) != 0) {
            gMemCardDialog_OperationState = 0xB;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            gMemCardDialog_TransferOffset = 0;
            gMemCard_pPrimaryTransferCursor -= SAVE_DATA_HEADER_SIZE;
            gMemCardDialog_TransferSize += SAVE_DATA_HEADER_SIZE;
        }
        gMemCardDialog_OperationState = 8;
        goto write;
    case 7:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            if (gMemCard_wDialogFlags & 0x100) {
                gMemCardDialog_OperationState = 8;
                break;
            }
            gMemCardDialog_RetryCounter = 0;
            MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned PSXLONG *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset, 0x480);
            goto io_pending;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            mode = gMemCardDialog_SyncResult;
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (mode != 0) {
                gMemCardDialog_OperationState = 0xD;
                if (mode == 1) {
                    gMemCardDialog_OperationState = 9;
                }
            } else {
                if (gSaveData_AllowIdentityBypass != 0) {
                    goto message_cf;
                }
                if (SaveData_MatchesDuelistAndCurrentSequence(
                        (SaveDataState *)gMemCard_pPrimaryTransferCursor,
                        (SaveDataState *)gLibrary_aCardArtRecord) != 0) {
                    goto message_cf;
                }
                if (gMemCardDialog_RetryCounter != 0) {
                    goto state_e;
                }
                gMemCardDialog_RetryCounter++;
                gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                                (unsigned PSXLONG *)gLibrary_aCardArtRecord,
                                gMemCardDialog_TransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
                goto io_pending;
            }
            break;
        state_e:
            gMemCardDialog_OperationState = 0xE;
            break;
        message_cf:
            MemCardDialog_SetMessage(0xCF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            gMemCardDialog_OperationState = 8;
            goto write;
        }
        gMemCardDialog_OperationState = 0xC;
        break;
    case 8:
    write:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            message = 0xD6;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xD7;
            }
            MemCardDialog_SetMessage(message, 0);
            MemCardWriteFile(0, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned PSXLONG *)gMemCard_pPrimaryTransferCursor,
                             gMemCardDialog_TransferOffset,
                             gMemCardDialog_TransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        status = gMemCardDialog_SyncResult;
        if (status != 0) {
            gMemCardDialog_OperationState = 0xB;
            if (status == 1) {
                gMemCardDialog_OperationState = 9;
            }
            break;
        }
        gMemCardDialog_OperationState = 0xA;
        break;
    case 9:
        message = 0xD2;
        if (gMemCard_wDialogFlags & 0x100) {
            message = 0xBB;
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 10:
        gMemCardDialog_Outcome = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            gSaveData_AllowIdentityBypass = 0;
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
        gMemCardDialog_Outcome = 3;
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
   gMemCardDialog_OperationState is the state and the 0x80/0x40 bits latch the first pass
   through each one; gMemCardDialog_SyncResult carries the previous call's result and
   gMemCardDialog_OperationState is set to the message state on every failure.

   Levers that mattered here:
   - The three jump tables follow the load table in this unit's contiguous
     rodata run starting at 0xC10.
   - The two inner switches are written in the target's block-layout order,
     not numeric order, because gcc emits switch arms in source order. The
     arms that merge into blocks elsewhere in the function (state 9 and state
     0xD) go last.
   - State 7 and state 8 each need their OWN local for gMemCardDialog_SyncResult. Sharing
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

    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            MemCardDialog_SetMessage(0xC9, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 1;
        if (gDialog_bChoice != 0) {
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        /* fallthrough */
    case 1:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD4, 0);
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
        case 3:
            gMemCardDialog_OperationState = 3;
            break;
        case 4:
            gMemCardDialog_OperationState = 4;
            if (gMemCard_wDialogFlags & 0x100) {
                break;
            }
            if (gSaveData_AllowIdentityBypass != 0) {
                break;
            }
            gMemCardDialog_OperationState = 0xE;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        break;
    case 2:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 3;
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            break;
        }
        MemCardDialog_SetMessage(0xB8, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)gMemCardDialog_szAllFilesPattern,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (PSXLONG *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        if (MemCard_FindEntry(gMemCardDialog_FileNameBuffer, (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files) >= 0) {
            gMemCardDialog_OperationState = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && gSaveData_AllowIdentityBypass == 0) {
            gMemCardDialog_OperationState = 0xE;
            break;
        }
        message = MemCard_CalcFreeBlocks((struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files);
        needed = gMemCardDialog_BlockCount;
        if (message >= needed) {
            gMemCardDialog_OperationState = 6;
            goto create;
        }
        gText_StagedValues[0].blocks.used = MEM_CARD_BLOCK_COUNT - message;
        gText_StagedValues[0].blocks.needed = needed;
        MemCardDialog_SetMessage(0xDB, 0x18);
        break;
    case 4:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xDE, 0x10);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xDF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        gMemCardDialog_OperationState = 5;
        MemCardDialog_SetMessage(0xBE, 0);
        break;
    case 5:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        if (gMemCardDialog_SyncCommand != 2) {
            break;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 3:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        case 4:
            if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    MemCardDialog_SetMessage(0xDD, 0x18);
                    break;
                }
                MemCardDialog_SetMessage(0xBF, 0x10);
                break;
            }
            gMemCardDialog_OperationState = 1;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        }
        break;
    case 6:
    create:
        if (MemCardCreateFile(0, (char *)gMemCardDialog_FileNameBuffer, gMemCardDialog_BlockCount) != 0) {
            gMemCardDialog_OperationState = 0xB;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            gMemCardDialog_TransferOffset = 0;
            gMemCard_pPrimaryTransferCursor -= SAVE_DATA_HEADER_SIZE;
            gMemCardDialog_TransferSize += SAVE_DATA_HEADER_SIZE;
        }
        gMemCardDialog_OperationState = 8;
        goto write;
    case 7:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 0;
            MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned PSXLONG *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset, 0x480);
            goto io_pending;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            mode = gMemCardDialog_SyncResult;
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (mode != 0) {
                gMemCardDialog_OperationState = 0xD;
                if (mode == 1) {
                    gMemCardDialog_OperationState = 9;
                }
            } else {
                if (gSaveData_AllowIdentityBypass != 0) {
                    goto message_cf;
                }
                if (SaveData_MatchesDuelistAndCurrentSequence(
                        (SaveDataState *)gMemCard_pPrimaryTransferCursor,
                        (SaveDataState *)gLibrary_aCardArtRecord) != 0) {
                    goto message_cf;
                }
                if (gMemCardDialog_RetryCounter != 0) {
                    goto state_e;
                }
                gMemCardDialog_RetryCounter++;
                gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                                (unsigned PSXLONG *)gLibrary_aCardArtRecord,
                                gMemCardDialog_TransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
                goto io_pending;
            }
            break;
        state_e:
            gMemCardDialog_OperationState = 0xE;
            break;
        message_cf:
            MemCardDialog_SetMessage(0xCF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            gMemCardDialog_OperationState = 8;
            goto write;
        }
        gMemCardDialog_OperationState = 0xC;
        break;
    case 8:
    write:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD6, 0);
            MemCardWriteFile(0, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned PSXLONG *)gMemCard_pPrimaryTransferCursor,
                             gMemCardDialog_TransferOffset,
                             gMemCardDialog_TransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        result = gMemCardDialog_SyncResult;
        if (result != 0) {
            gMemCardDialog_OperationState = 0xB;
            if (result == 1) {
                gMemCardDialog_OperationState = 9;
            }
            break;
        }
        gMemCardDialog_OperationState = 0xA;
        break;
    case 9:
        MemCardDialog_SetMessage(0xD2, 0x18);
        break;
    case 10:
        gMemCardDialog_Outcome = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            gSaveData_AllowIdentityBypass = 0;
            gSaveDataSequence = (s32)((u32)gSaveDataSequence + 1);
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 11:
        MemCardDialog_SetMessage(0xD9, 0x18);
        break;
    case 12:
        MemCardDialog_SetMessage(0xCD, 0x18);
        gMemCardDialog_Outcome = 3;
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

void MemCardDialog_StepSave(void)
{
    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_OperationState = 0;
    }
    MemCardDialog_UpdateSave();
}

#ifndef MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE
#define MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE sizeof(DuelEffectChannel)
#endif

#ifndef MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xC0
#endif

/* The empty callback, trade write-back operation, and modal runtime that
   drive the operation table. MemCardDialog_Update dispatches the save and
   trade operations through gMemCardDialog_apfnSteps; all paths share the dialog flags,
   result words, active channel, and request outcome. */

#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
void MemCardDialog_StepNone(void)
{
}
#else
/* The Japanese and European builds have this step where the US build has
   MemCardDialog_StepNone: MemCardDialog_StepSave's one-time setup, plus
   clearing the transfer offset and raising dialog flag 0x100, before the
   save update runs. */
void MemCardDialog_StepAlternateSave(void)
{
    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_TransferOffset = 0;
        gMemCardDialog_OperationState = 0;
        gMemCard_wDialogFlags |= 0x100;
    }
    MemCardDialog_UpdateSave();
}
#endif

void MemCardDialog_UpdateTradeSave(void)
{
    s32 files;
    u8 *record;

    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        MemCardDialog_SetMessage(MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE, 0);
        gMemCardDialog_OperationState = 0;
    }
    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            do {
            } while (MemCardAccept(gMemCardDialog_Port) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 1:
            MemCardDialog_SetMessage(0xC1, 0x18);
            break;
        case 0:
        case 3:
            gMemCardDialog_OperationState = 1;
            gMemCardDialog_Port ^= 0x10;
            if (gMemCardDialog_Port != 0) {
                gMemCardDialog_OperationState = 0;
            }
            break;
        case 2:
            MemCardDialog_SetMessage(0xDA, 0x18);
            break;
        case 4:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        }
        break;
    case 1:
        gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
        if (MemCardGetDirentry(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (PSXLONG *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            MemCardDialog_SetMessage(0xDA, 0x18);
            break;
        }
        if (files == 0) {
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        }
        gMemCardDialog_Port ^= 0x10;
        if (gMemCardDialog_Port != 0) {
            gMemCardDialog_OperationState = 1;
            break;
        }
        gMemCardDialog_OperationState = 2;
        /* fallthrough */
    case 2:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 0;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            MemCardReadFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned PSXLONG *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset, 0x480);
            goto io_pending;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (gMemCardDialog_SyncResult != 0) {
                MemCardDialog_SetMessage(0xDA, 0x18);
                break;
            }
        }
        record = gMemCard_pPrimaryTransferCursor;
        if (gMemCardDialog_Port != 0) {
            record = gMemCard_pSecondaryTransferCursor;
        }
        if (SaveData_HasSameDuelistCode(
                (SaveDataState *)record,
                (SaveDataState *)gLibrary_aCardArtRecord) == 0) {
            if (gMemCardDialog_RetryCounter != 0) {
                MemCardDialog_SetMessage(0xC3, 0x18);
                break;
            }
            gMemCardDialog_RetryCounter++;
            gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardReadFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned PSXLONG *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
            goto io_pending;
        }
        gMemCardDialog_Port ^= 0x10;
        if (gMemCardDialog_Port != 0) {
            gMemCardDialog_OperationState = 2;
            break;
        }
        gMemCardDialog_OperationState = 3;
        /* fallthrough */
    case 3:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY
                          | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xC4, 0);
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            goto step;
        }
    write:
        gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        if (gMemCardDialog_Port != 0) {
            MemCardWriteFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned PSXLONG *)gMemCard_pSecondaryTransferCursor,
                             gMemCardDialog_TransferOffset, 0x80);
        } else {
            MemCardWriteFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned PSXLONG *)gMemCard_pPrimaryTransferCursor,
                             gMemCardDialog_TransferOffset, 0x80);
        }
    io_pending:
        gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
        break;
    step:
        if (gMemCardDialog_SyncResult != 0) {
            MemCardDialog_SetMessage(0xC5, 0x18);
            break;
        }
        gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        gMemCardDialog_Port ^= 0x10;
        if (gMemCardDialog_Port == 0) {
            gMemCard_pPrimaryTransferCursor += 0x80;
            gMemCard_pSecondaryTransferCursor += 0x80;
            gMemCardDialog_TransferOffset += 0x80;
            gMemCardDialog_TransferSize -= 0x80;
            if (gMemCardDialog_TransferSize == 0) {
                gMemCardDialog_Outcome = 1;
                MemCardDialog_SetMessage(0xC6, 0x18);
                break;
            }
        }
        goto write;
    }
}

s32 MemCardDialog_StepSlide(DisplayObject *object, s32 arg1, s32 arg2,
                            s32 index)
{
    s32 saved_index = index;
    s32 value;

    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)object) == 0) {
        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
    }

    value = object->field_60;
    if (value < 0) {
        value += 0x40;
        if (value >= 0) {
            object->field_6C = 0;
            value = 0;
        }
    } else {
        value -= 0x40;
        if (value <= 0) {
            object->field_6C = 0;
            value = 0;
        }
    }
    object->field_60 = value;

    Widget_SlideSine((DisplayObjectPosition *)object, arg1, arg2, value);

    if (saved_index >= 0) {
        TextBox_SetPos(
#ifdef VERSION_JAPAN
            (DuelEffectChannel *)((u8 *)gText_aChannels +
                                  saved_index * MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE),
#else
            &gText_aChannels[saved_index],
#endif
            (s16)object->field_30.h.field_30,
            (s16)object->field_30.h.field_32);
    }

    return object->field_6C;
}

void MemCardDialog_CreateObject(void)
{
    s32 i = 0;
    DuelEffectChannel *p = gText_aChannels;
    DisplayObject *o;

    gMemCardDialog_ChannelIndex = 0;

    for (; i < DUEL_EFFECT_CHANNEL_COUNT; i++) {
        if (!(p->flags_34 & DUEL_EFFECT_CHANNEL_FLAG_ACTIVE)) {
            gMemCardDialog_ChannelIndex = i;
            break;
        }
        p = (DuelEffectChannel *)(
            (u8 *)p + MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE
        );
    }

    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(o, DIALOG_BOX_X, -0x40, 3, 2, 1, 0xB, 0x20C);
    o->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_SelectOrderingTable1(o);
    DisplayObject_SetDepthOffset(o, 0xF);
    gMemCard_pDialogObject = o;
}

void MemCardDialog_Update(void)
{
    DuelEffectChannel *p;
    s32 f;
    s32 t;
    s32 u;
    s32 c;

    f = gMemCard_wDialogFlags;
    if ((f & MEM_CARD_DIALOG_FLAG_CLOSING) != 0) {
        if (gMemCard_pDialogObject == (DisplayObject *)0) {
            gMemCard_wDialogFlags = 0;
            return;
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x100, gMemCardDialog_ChannelIndex
            ) == 0) {
#ifdef VERSION_JAPAN
            TextBox_Destroy((DuelEffectChannel *)(
                (u8 *)gText_aChannels +
                gMemCardDialog_ChannelIndex * MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE));
#else
            TextBox_Destroy(&gText_aChannels[gMemCardDialog_ChannelIndex]);
#endif
            DisplayObject_ReleaseIfPresent(gMemCard_pDialogObject);
            gMemCard_pDialogObject = (DisplayObject *)0;
        }
        return;
    }
    if ((f & (MEM_CARD_DIALOG_FLAG_OPENED |
              MEM_CARD_DIALOG_FLAG_RESULT_READY)) ==
        (MEM_CARD_DIALOG_FLAG_OPENED | MEM_CARD_DIALOG_FLAG_RESULT_READY)) {
        if ((f & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCard_wDialogFlags =
                f | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            p = TextBox_Create(
                gMemCardDialog_ChannelIndex, gMemCardDialog_MessageCode, DIALOG_BOX_X, 0x50, DIALOG_BOX_WIDTH, 0x30
            );
            DuelEffect_MarkObjectIfActive((MenuRecord *)p);
            p->depth_offset = 0x10;
            if ((gMemCard_wDialogFlags & 0x20) != 0) {
                do {
                    TextBox_UpdateChannels();
                } while (p->prompt_object == 0);
                return;
            }
            if ((gMemCard_wDialogFlags & 0x10) != 0) {
#ifdef VERSION_EUROPE
                p->flags_34 = p->flags_34 | 0x1010;
#else
                p->flags_34 = p->flags_34 | 0x1008;
#endif
                return;
            }
            TextBox_BuildBlocking(p);
            goto b14;
        }
        TextBox_UpdateChannels();
#ifdef VERSION_JAPAN
        p = (DuelEffectChannel *)(
            (u8 *)gText_aChannels +
            gMemCardDialog_ChannelIndex * MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE);
#else
        p = &gText_aChannels[gMemCardDialog_ChannelIndex];
#endif
#ifdef VERSION_EUROPE
        if ((*(s32 *)&p->flags_34 & 0x2010) !=
#else
        if ((*(s32 *)&p->flags_34 & TEXT_BOX_COMPLETION_MASK) !=
#endif
            TEXT_BOX_FLAG_DONE) {
            return;
        }
    b14:
        t = gMemCard_wDialogFlags;
        u = t & 8;
        gMemCard_wDialogFlags =
            t & ~MEM_CARD_DIALOG_FLAG_RESULT_READY;
        if (u == 0) {
            return;
        }
        gMemCard_wDialogFlags = 0;
        goto b25;
    }
    if ((f & MEM_CARD_DIALOG_FLAG_OPENED) == 0) {
        if ((f & MEM_CARD_DIALOG_FLAG_STARTED) == 0) {
            gMemCard_wDialogFlags = f | MEM_CARD_DIALOG_FLAG_STARTED;
            MemCardStart();
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
            func_8008D680();
#endif
            gMemCardDialog_Outcome = 2;
            MemCardDialog_CreateObject();
            gMemCard_pDialogObject->field_60 = -0x400;
            return;
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x50, -1
            ) == 0) {
            gMemCard_wDialogFlags =
                gMemCard_wDialogFlags | MEM_CARD_DIALOG_FLAG_OPENED;
        }
        return;
    }
    if ((f & MEM_CARD_DIALOG_FLAG_IO_PENDING) != 0) {
        c = MemCardSync(
            1, (PSXLONG *)&gMemCardDialog_SyncCommand, (PSXLONG *)&gMemCardDialog_SyncResult
        );
        gMemCardDialog_SyncStatus = c;
        if (c != 1) {
            return;
        }
        gMemCard_wDialogFlags =
            gMemCard_wDialogFlags & ~MEM_CARD_DIALOG_FLAG_IO_PENDING;
    }
    gMemCardDialog_apfnSteps[gMemCardDialog_StepIndex]();
    if (gMemCard_wDialogFlags != 0) {
        return;
    }
b25:
    gMemCard_wDialogFlags =
        gMemCard_wDialogFlags | MEM_CARD_DIALOG_FLAG_CLOSING;
    gMemCard_pDialogObject->field_60 = 0x400;
#ifdef VERSION_JAPAN
    func_8008D6F0();
#endif
    MemCardStop();
#ifdef VERSION_EUROPE
    func_8008D6F0();
#endif
}

int MemCardDialog_Poll(void)
{
    MemCardDialog_Update();
    if (gMemCard_wDialogFlags != 0) {
        return 0;
    }
    return gMemCardDialog_Outcome;
}

void MemCardDialog_Start(s32 step)
{
    gMemCard_wDialogFlags = MEM_CARD_DIALOG_FLAG_ACTIVE;
    gMemCardDialog_StepIndex = step;
    gDialog_bCallbackState = 0;
}

void MemCardDialog_Request(u8 *buf, s32 size, u8 *name, s32 step)
{
    strcpy(gMemCardDialog_FileNameBuffer, name);
    gMemCardDialog_Port = 0;
    gMemCardDialog_TransferSize = size;
    gMemCardDialog_TransferOffset = 0x200;
    gMemCardDialog_BlockCount =
        (size + MEM_CARD_BLOCK_SIZE - 1) / MEM_CARD_BLOCK_SIZE;
    gMemCard_pPrimaryTransferCursor = buf;
    MemCardDialog_Start(step);
}

/* Regional values: the load box's flags (0x1008 -> 0x1010) and the channel
 * flag bit it waits on (0x8 -> 0x10). The European build
 * (src/game/european/) defines its own. */
#ifndef SAVE_DATA_LOAD_BOX_FLAGS
#define SAVE_DATA_LOAD_BOX_FLAGS 0x1008
#endif
#ifndef SAVE_DATA_LOAD_CHANNEL_FLAG
#define SAVE_DATA_LOAD_CHANNEL_FLAG 8
#endif

/* The complete single-player and two-player save-transfer runtime. The first
   three functions request, poll, apply, build, and write the resident save;
   the remaining state machine loads both card slots, then the two wrappers add
   trade or duel setup, and the final request writes the paired trade result
   back. */

#ifndef SAVE_DATA_LOAD_STATUS_ADDRESS
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009B0D1
#endif

#ifndef SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON
#define SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif

#ifndef SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON
#define SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON PAD_BUTTON_CROSS
#endif

void SaveData_RequestLoad(void)
{
    /* The symbolic store changes the target $at scheduling and relocation. */
    *(u8 *)SAVE_DATA_LOAD_STATUS_ADDRESS = 0;
    MemCardDialog_Request(
        gSaveData_aTransferBuffer,
        SAVE_DATA_STATE_SIZE,
        gMemCard_szSaveFileName,
        0
    );
}

s32 SaveData_PollLoad(void) {
    s32 r = MemCardDialog_Poll();
    if (r != 0) {
        if (r == 1) {
            u8 *p = (u8 *)gDuel_awPlayerDeck;
            Util_CopyWords(p, gSaveData_aTransferBuffer, SAVE_DATA_STATE_SIZE);
            SaveData_ApplyRuntimeState((SaveDataState *)p);
        }
        return r;
    }
    return 0;
}

void SaveData_RequestWrite(void){Util_CopyWords(gSaveData_aTransferBuffer,(u8 *)gDuel_awPlayerDeck,SAVE_DATA_STATE_SIZE);SaveData_BuildPayload((SaveDataPayload *)(gSaveData_aTransferBuffer-SAVE_DATA_HEADER_SIZE));MemCardDialog_Request(gSaveData_aTransferBuffer,SAVE_DATA_REPLICATED_STATE_SIZE,gMemCard_szSaveFileName,2);}

#ifdef VERSION_JAPAN
/* Japanese only, with no US counterpart: opens the memory-card dialog at
   step 3 for the 0x8000-byte buffer at 0x80140000, under the file name the
   Japanese image keeps at 0x8001032C ("BISLPMP86398-YUGIOH"). */
void SaveData_RequestOptionsWrite(void)
{
    MemCardDialog_Request((u8 *)0x80140000, 0x8000, gMemCard_szOptionsFileName, 3);
}
#endif

#ifdef VERSION_JAPAN
#define SAVE_DATA_LOAD_PAIR_CHANNEL(base, index) \
    ((DuelEffectChannel *)((u8 *)(base) + (index) * 0x60))
#else
#define SAVE_DATA_LOAD_PAIR_CHANNEL(base, index) (&(base)[index])
#endif

/* The two-player load, validation and write-back runtime. The state machine
   first loads both card slots, then the two wrappers add trade or duel setup,
   and the final request writes the paired trade result back. */

/* MATCH 2026-09-06, first-day function from the m2c draft (257 instructions,
 * the memory-card save state machine: format, load, verify, retry). Jump
 * table on the mode nibble, so only the full build proves it. Levers, in
 * order: the scalar arm for gSaveData_LoadPairState; gInput_wPad1Pressed on its .data
 * VOLATILE arm (retail reloads it for the second bit test and reads it bare);
 * the call arguments gMemCardDialog_ChannelIndex and gSaveData_PairMessageCode hoisted into locals BEFORE the
 * +0x60 store that precedes the sprite call (-6 -> -1); ONE name for the
 * sprite pointer in cases 0 and 0xA, which is what puts it in $s0 (its live
 * range crosses the wait loop's calls in 0xA); the record passed to
 * TextBox_Destroy as its ADDRESS, index-first (`&gText_aChannels[i]`; m2c had read
 * it as a load); case 3's `v = 0` before the gMemCardDialog_Port store reaching a
 * SHARED `return v` through a goto, so the zero stays a variable; and the
 * flags read through a block-local base `q = gText_aChannels` so the 0x34 is a
 * load displacement rather than folded into %lo. */

s32 SaveData_UpdateLoadPair(void)
{
    DuelEffectChannel *o;
    s32 v;
    s32 m;
    DuelEffectChannel *q;
    s32 a;
    s32 b;

    m = gSaveData_LoadPairState & 0xF;
    switch (m) {
    case 0:
        if ((gSaveData_LoadPairState & 0x80) == 0) {
            gSaveData_LoadPairState |= 0x80;
            MemCardDialog_CreateObject();
            a = gMemCardDialog_ChannelIndex;
            gMemCard_pDialogObject->field_60 = -0x400;
            o = TextBox_Create(a, 0xC2, DIALOG_BOX_X, 0x50, DIALOG_BOX_WIDTH, 0x30);
            o->depth_offset = 0x10;
            TextBox_BuildBlocking(o);
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x50, gMemCardDialog_ChannelIndex) == 0) {
            gSaveData_LoadPairState = 1;
        }
        return 0;
    case 1:
        if (gInput_wPad1Pressed & SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON) {
            SD_SEPlayFull(8);
            gSaveData_LoadPairState = 0x82;
            gMemCard_pDialogObject->field_60 = 0x400;
        } else if (gInput_wPad1Pressed & SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON) {
            SD_SEPlayFull(7);
            gSaveData_LoadPairState = 2;
            gMemCard_pDialogObject->field_60 = 0x400;
        }
        return 0;
    case 2:
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x100, gMemCardDialog_ChannelIndex) == 0) {
            TextBox_Destroy(
                SAVE_DATA_LOAD_PAIR_CHANNEL(gText_aChannels, gMemCardDialog_ChannelIndex));
            DisplayObject_ReleaseIfPresent(gMemCard_pDialogObject);
            gMemCard_pDialogObject = (DisplayObject *)0;
            if ((gSaveData_LoadPairState & 0x80) == 0) {
                gSaveData_LoadPairState = 3;
                MemCardDialog_Request(
                    gSaveData_aPairWorkspace,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                return 0;
            }
            return 2;
        }
        return 0;
    case 3:
        v = MemCardDialog_Poll();
        if (v != 0) {
            if (v == 1) {
                if (gSaveData_LoadPairState & 0x40) {
                    if (SaveData_HasSameDuelistCode(
                            (SaveDataState *)gSaveData_aPairWorkspace,
                            (SaveDataState *)(gSaveData_aPairWorkspace + 0x1000)) == 0) {
                        return 1;
                    }
                    gSaveData_LoadPairState = 0xA;
                    return 0;
                }
                gSaveData_LoadPairState |= 0x40;
                MemCardDialog_Request(
                    gSaveData_aSecondPlayerState,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                v = 0;
                gMemCardDialog_Port = 0x10;
                goto done;
            }
            return v;
        }
    done:
        return v;
    case 0xA:
        if ((gSaveData_LoadPairState & 0x80) == 0) {
            gSaveData_LoadPairState |= 0xC0;
            MemCardDialog_CreateObject();
            a = gMemCardDialog_ChannelIndex;
            b = gSaveData_PairMessageCode;
            gMemCard_pDialogObject->field_60 = -0x400;
            o = TextBox_CreateFlagged(a, b, DIALOG_BOX_X, 0x50, DIALOG_BOX_WIDTH, 0x30, SAVE_DATA_LOAD_BOX_FLAGS);
            o->depth_offset = 0x10;
            do {
                TextBox_UpdateChannels();
            } while ((o->flags_34 & TEXT_BOX_FLAG_DONE) == 0);
        }
        if (gSaveData_LoadPairState & 0x40) {
            if (MemCardDialog_StepSlide(
                    gMemCard_pDialogObject, DIALOG_BOX_X, 0x50, gMemCardDialog_ChannelIndex) == 0) {
                gSaveData_LoadPairState &= 0xBF;
            }
            return 0;
        }
        TextBox_UpdateChannels();
        q = gText_aChannels;
        if ((SAVE_DATA_LOAD_PAIR_CHANNEL(q, gMemCardDialog_ChannelIndex)->flags_34 & SAVE_DATA_LOAD_CHANNEL_FLAG) == 0) {
            gSaveData_LoadPairState = 0xB;
        }
        return 0;
    case 0xB:
        if ((gSaveData_LoadPairState & 0x80) == 0) {
            gSaveData_LoadPairState |= 0x80;
            gMemCard_pDialogObject->field_60 = 0x400;
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x100, gMemCardDialog_ChannelIndex) == 0) {
            TextBox_Destroy(
                SAVE_DATA_LOAD_PAIR_CHANNEL(gText_aChannels, gMemCardDialog_ChannelIndex));
            DisplayObject_ReleaseIfPresent(gMemCard_pDialogObject);
            gMemCard_pDialogObject = (DisplayObject *)0;
            return 2;
        }
        return 0;
    }
    return 0;
}
#undef SAVE_DATA_LOAD_PAIR_CHANNEL

s32 SaveData_UpdateTradeLoad(void)
{
    if ((gSaveData_PairSetupState & 0x80) == 0) {
        gSaveData_PairSetupState |= 0x80;
        gSaveData_PairMessageCode = 0x29;
    }
    return SaveData_UpdateLoadPair();
}

s32 SaveData_UpdateDuelLoad(void)
{
    s32 result;
    s32 i;
    u8 *left;
    u8 *right;
    TextStagingValues *state;

    if ((gSaveData_PairSetupState & 128) == 0) {
        gSaveData_PairSetupState |= 128;
        gSaveData_PairMessageCode = 40;
    }
    result = SaveData_UpdateTradeLoad();
    if (result == 1) {
        left = gSaveData_aPairWorkspace;
        right = gSaveData_aPairWorkspace + TWO_PLAYER_SAVE_SLOT_STRIDE;
        i = 0;
        state = gText_StagedValues;
        gSaveData_LoadPairState = 10;
        gSaveData_PairMessageCode = 36;
        for (; i < DECK_SIZE; i++, left += 2, right += 2) {
            if (*(u16 *)left == 0) {
                state->deck_validation.invalid_side = 1;
                return 0;
            }
            if (*(u16 *)right == 0) {
                state->deck_validation.invalid_side = 2;
                return 0;
            }
        }
        Text_SjisToGlyphCodes(
            gText_aFirstPlayerNameGlyphs,
            gSaveData_aFirstPlayerNameSjis,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
        Text_SjisToGlyphCodes(
            gText_aSecondPlayerNameGlyphs,
            gSaveData_aFirstPlayerNameSjis + TWO_PLAYER_SAVE_SLOT_STRIDE,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
    }
    return result;
}

void SaveData_RequestTradeWrite(void)
{
    u8 *p = gSaveData_aTradeWriteStaging;
    u8 *q = p + TWO_PLAYER_SAVE_SLOT_STRIDE;

    SaveData_WritePrimarySecondaryIntegrity(p);
    SaveData_WritePrimarySecondaryIntegrity(q);
    gMemCard_pSecondaryTransferCursor = q;
    MemCardDialog_Request(
        p,
        TWO_PLAYER_SAVE_TRANSFER_SIZE,
        gMemCard_szSaveFileName,
        4
    );
}
