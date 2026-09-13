/*
 * Legacy card/dialog update, distinct from MemCardDialog_Update. The selected
 * record is captured before polling and UI callbacks; flag, progress and
 * directory data retain their measured live reads. The directory lookup's
 * signed return value is consumed, not a void-call register accident.
 *
 * All 2,128 text bytes and the two dispatch tables with their alignment gap
 * match the existing uniform G8 split profile. The inline write phase keeps
 * its captured amount separate from the read phase until their common tail.
 * Named message/bits values retain the conditional publication of stage 8.
 *
 * The final sector word is read before publishing the XOR checksum. Its
 * signed bias/shift preserves division by 64 truncated toward zero. Raw
 * buffer addresses and the final Make/Fail diagnostic fallthrough are retail
 * behavior, not new storage or a revised card protocol.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/stdio.h"
#include "mem_card.h"
#define MEM_CARD_DIRECTORY_ABSOLUTE
#include "mem_card_directory.h"
#include "mem_card_begin_request.h"
#include "io_event_helpers.h"
#include "duel_effect.h"
#include "duel_effect_create_channel.h"
#include "func_80039794.h"
#include "util_memory.h"
#include "save_data.h"
#define GDIALOG_CHOICE_IN_DATA
#include "dialog_choice.h"
#include "../unmatched.h"

#define MEM_CARD_LEGACY_CALL_WITH_SLOT
#include "mem_card_legacy_update.h"

static inline void MemCard_AdvanceWriteTransfer(u8 *state, u8 *slot, s32 amount)
{
    *(s16 *)(state + 0x12) = *(u16 *)(state + 0x12) - amount;
    if (*(s16 *)(state + 0x12) <= 0) {
        D_8009B3D5 = 0;
    } else {
        *(s32 *)slot += amount;
        *(u16 *)(state + 0x14) += amount;
        if (*(s16 *)(state + 0x12) < *(s16 *)(state + 0x16)) {
            *(u16 *)(state + 0x16) = *(u16 *)(state + 0x12);
        }
    }
}

void func_8003DC1C(void)
{
    u8 *state = D_800EF6D0;
    s32 slot_offset = D_8009B3DD * 0x268;
    u8 *slot_base = state + 0x1C;
    u8 *slot = slot_offset + slot_base;
    s32 port = 0;
    s32 result;
    s32 index;

    if (D_8009B3CC & 0x2000) {
        for (index = 0; index < gMemCard_nDirEntries; index++) {
            FntPrint(D_8009AF6C, &gMemCard_pDirEntries[index]);
        }
        FntPrint(D_80010378, slot[0xC]);
    }
    result = -1;
    if (D_8009B3CC & 0x4000) {
        result = func_80044838(1, &D_8009B3E4, &D_8009B3C8);
    }
    if ((D_8009B3CC & 0x1000) && result != 0 && result == 1) {
        switch (D_8009B3E4) {
        case 2:
            D_8009B3CC |= 0x2000;
            switch (D_8009B3C8) {
            case 3:
                D_8009B3CC |= 0x800;
                /* fallthrough */
            case 0:
                slot[0xC] = (u8)gMemCard_nFreeBlocks;
                D_8009B3D5 = 0;
                break;
            case 4:
                D_8009B3CC |= 2;
                D_8009B3D5 = 1;
                break;
            case 1:
            default:
                D_8009B3D5 = 2;
                break;
            }
            break;
        case 3:
            if (D_8009B3C8 != 0) {
                goto transfer_error;
            }
            index = *(s16 *)(state + 0x16);
            *(s16 *)(state + 0x12) = *(u16 *)(state + 0x12) - index;
            if (*(s16 *)(state + 0x12) <= 0) {
                D_8009B3D5 = 0;
                break;
            }
            *(s32 *)slot += index;
            goto advance_chunk;
        case 4:
            if (D_8009B3C8 != 0) {
                goto transfer_error;
            }
            index = *(s16 *)(state + 0x16);
            MemCard_AdvanceWriteTransfer(state, slot, index);
            break;
advance_chunk:
            *(u16 *)(state + 0x14) += index;
            if (*(s16 *)(state + 0x12) < *(s16 *)(state + 0x16)) {
                *(u16 *)(state + 0x16) = *(u16 *)(state + 0x12);
            }
            break;
transfer_error:
            D_8009B3D5 = 1;
            break;
        case 8:
        case 11:
        case 12:
            D_8009B3D5 = 1;
            if (D_8009B3C8 == 0) {
                D_8009B3D5 = 0;
            }
            break;
        }
    }
    if (D_8009B3C1 & 0xF) {
        result = 1;
        D_80090F88[D_8009B3C1 & 0xF]((MenuRecord *)state, slot);
    }
    if (result >= 0) {
        return;
    }
    if (D_8009B3CC & 0x400) {
        DuelEffectChannel *channel = &D_800EB0F8[state[0x1A]];
        func_80039794();
        if ((*(u32 *)&channel->flags_34 & 0x2008) == 0x2000) {
            D_8009B3CC &= 0xFBFF;
        }
        return;
    }
    switch (D_8009B3CF & 0xF) {
    case 0:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            D_8009B3C1 = 1;
            break;
        }
        if (gDialog_bChoice != 0) {
            D_8009B3C1 = 2;
            break;
        }
        MemCard_Init(1);
        MemCard_InitIOEvents();
        D_8009B3CF = 1;
        D_8009B3CC |= 0x4000;
        /* fallthrough */
    case 1:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            D_8009B3CC |= 0x1000;
            MemCard_ReqLoadDirectory(port);
            break;
        }
        D_8009B3CF = 4;
        /* fallthrough */
    case 4:
        if (!(D_8009B3CC & 0x2000)) {
            break;
        }
        D_8009B3CC &= 0xEFFF;
        if (D_8009B3D5 == 2) {
            DuelEffect_CreateChannel(0xD2, 1);
            goto wait_message;
        }
        if (!(D_8009B3C7 & 1)) {
            D_8009B3CF = 5;
        } else {
            D_8009B3CF = 7;
        }
        break;
    case 5:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            index = MemCard_FindLoadedEntry(gMemCard_szSaveFileName);
            if (index < 0 || (D_8009B3CC & 2)) {
                DuelEffect_CreateChannel(0xD3, 1);
                goto wait_message;
            }
            MemCard_ReqReadSector(port, 0x80210000,
                *(s32 *)((u8 *)&gMemCard_pDirEntries[index] + 0x20) / 64);
            break;
        }
        if (D_8009B3D5 != 0) {
            DuelEffect_CreateChannel(0xD8, 1);
            goto wait_message;
        }
        D_8009B3CF = 6;
        /* fallthrough */
    case 6:
        if (!(D_8009B3CF & 0x80)) {
            u8 *name = gMemCard_szSaveFileName;
            D_8009B3CF |= 0x80;
            index = MemCard_FindLoadedEntry(name);
            if (index < 0 || (D_8009B3CC & 2)) {
                DuelEffect_CreateChannel(0xD3, 1);
                goto wait_message;
            }
            DuelEffect_CreateChannel(0x80D5, 0);
            slot[0xD] = index;
            D_8009B3CC |= 0x1000;
            MemCard_ReqReadFile(port, (s32)name, 0x80200000, 0x200, 0x1E00);
            break;
        }
        if (D_8009B3D5 != 1) {
            DuelEffect_CreateChannel(0xD7, 1);
        } else {
            DuelEffect_CreateChannel(0xD8, 1);
        }
        goto wait_message;
    case 7:
        if (!(D_8009B3CF & 0x80)) {
            u8 *name;
            D_8009B3CF |= 0x80;
            if (D_8009B3CC & 2) {
                D_8009B3CF = 9;
                break;
            }
            name = gMemCard_szSaveFileName;
            if (MemCard_FindLoadedEntry(name) < 0) {
                D_8009B3CF = 10;
                break;
            }
            DuelEffect_CreateChannel(0x80D6, 0);
            Util_CopyWords((u8 *)0x80300000, gSaveData_aHeaderTemplate, 0x200);
            Util_FillMemory((u8 *)0x80300200, 0x86, 0x800);
            MemCard_ReqWriteFile(port, (s32)name, 0x80300000, 0, 0xA00);
            D_8009B3CC |= 0x1000;
            break;
        }
        {
            s32 message = 0xD7;
            s32 bits;
            if (D_8009B3D5 == 1) {
                message = 0xD9;
                bits = 1;
            } else {
                bits = 1;
                D_8009B3CF = 8;
            }
            DuelEffect_CreateChannel(message, bits);
        }
        do { D_8009B3CC |= 0x400; } while (0);
        D_8009B3CF = 8;
        /* fallthrough */
    case 8:
        if (!(D_8009B3CF & 0x80)) {
            s32 checksum;
            s32 i;
            D_8009B3CF |= 0x80;
            index = MemCard_FindLoadedEntry(gMemCard_szSaveFileName);
            checksum = 0;
            D_8021007E = 1;
            D_8021007D = 0xFF;
            D_8021007C = 0xFF;
            D_8021007B = 0xFF;
            D_8021007A = 0xFF;
            for (i = 0; i < 0x7F; i++) {
                checksum ^= ((u8 *)0x80210000)[i];
            }
            {
                s32 sector = gMemCard_pDirEntries[index].head;
                D_8021007F = checksum;
                if (sector < 0) {
                    sector += 63;
                }
                MemCard_ReqWriteSector(port, 0x80210000, sector >> 6);
            }
            break;
        }
        if (D_8009B3D5 != 0) {
            DuelEffect_CreateChannel(0xD9, 1);
wait_message:
            D_8009B3CC |= 0x400;
        }
        break;
    case 9:
    case 10:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            if (slot[0xC] >= state[0x10]) {
                MemCard_ReqCreateFile(port, (s32)gMemCard_szSaveFileName, state[0x10]);
            }
            break;
        }
        if (D_8009B3D5 == 0) {
            printf(D_80010398);
        }
        printf(D_800103A4);
        break;
    }
}
