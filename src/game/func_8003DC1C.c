#include "../types.h"
#define GDIALOG_CHOICE_IN_DATA
#define MEM_CARD_DIRECTORY_IN_DATA
#define MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/stdio.h"
#include "mem_card_directory.h"
#include "mem_card_work.h"
#include "io_event_helpers.h"
#include "duel_effect_create_channel.h"
#include "dialog_choice.h"
#include "func_80039794.h"
#include "util_memory.h"
#include "save_data.h"

void func_8003DC1C(void)
{
    MemCardWorkRoot *root = &D_800EF6D0.root;
    MemCardWorkSlot *slot = &D_800EF6D0.slots[D_8009B3DD];
    s32 zero = 0;
    s32 status;
    s32 index;
    s32 offset;
    s32 remaining;
    u8 *cursor;
    s32 message;
    s32 checksum;
    u8 *name;

    if (D_8009B3CC & 0x2000) {
        index = zero;
        if (index < gMemCard_nDirEntries) {
            do {
                FntPrint(D_8009AF6C, &gMemCard_pDirEntries[index]);
                index++;
            } while (index < gMemCard_nDirEntries);
        }
        FntPrint(D_80010378, slot->free_blocks);
    }
    status = -1;
    if (D_8009B3CC & 0x4000)
        status = func_80044838(1, &D_8009B3E4, &D_8009B3C8);
    if ((D_8009B3CC & 0x1000) && status != 0 && status == 1) {
        switch (D_8009B3E4) {
        case 2:
            D_8009B3CC |= 0x2000;
            switch (D_8009B3C8) {
            case 3:
                D_8009B3CC |= 0x800;
            case 0:
                slot->free_blocks = *(u8 *)&gMemCard_nFreeBlocks;
                goto success;
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
            if (D_8009B3C8 == 0) {
                index = root->chunk;
                remaining = (u16)root->remaining - index;
                root->remaining = remaining;
                if ((s32)((u32)remaining << 16) > 0) {
                    slot->cursor += index;
                    root->offset += index;
                    if (root->remaining < root->chunk)
                        root->chunk = (u16)root->remaining;
                } else {
                    D_8009B3D5 = 0;
                }
            } else {
                D_8009B3D5 = 1;
            }
            break;
        case 4:
            if (D_8009B3C8 == 0) {
                index = root->chunk;
                remaining = (u16)root->remaining - index;
                root->remaining = remaining;
                if ((s32)((u32)remaining << 16) > 0) {
                    slot->cursor += index;
                    root->offset += index;
                    if (root->remaining < root->chunk)
                        root->chunk = (u16)root->remaining;
                } else {
                    D_8009B3D5 = 0;
                }
            } else {
                D_8009B3D5 = 1;
            }
            break;
        case 8:
        case 11:
        case 12:
            D_8009B3D5 = 1;
            if (D_8009B3C8)
                break;
success:
            D_8009B3D5 = 0;
            break;
        }
    }
    if (D_8009B3C1 & 0xF) {
        status = 1;
        D_80090F88[D_8009B3C1 & 0xF](root, slot);
    }
    if (status >= 0)
        return;
    if (D_8009B3CC & 0x400) {
        DuelEffectChannel *text = &D_800EB0F8[root->text_index];
        func_80039794();
        if ((*(u32 *)((u8 *)text + 0x34) & 0x2008) == 0x2000)
            D_8009B3CC &= ~0x400;
        return;
    }
    switch (D_8009B3CF & 0xF) {
    case 0:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            D_8009B3C1 = 1;
            break;
        }
        if (gDialog_bChoice) {
            D_8009B3C1 = 2;
            break;
        }
        MemCard_Init(1);
        MemCard_InitIOEvents();
        D_8009B3CF = 1;
        D_8009B3CC |= 0x4000;
    case 1:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            D_8009B3CC |= 0x1000;
            MemCard_ReqLoadDirectory(zero);
            break;
        }
        D_8009B3CF = 4;
    case 4:
        if (!(D_8009B3CC & 0x2000))
            break;
        D_8009B3CC &= ~0x1000;
        if (D_8009B3D5 == 2) {
            message = 0xD2;
            goto show_message;
        }
        if (!(D_8009B3C7 & 1))
            D_8009B3CF = 5;
        else
            D_8009B3CF = 7;
        break;
    case 5:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            index = MemCard_FindLoadedEntry(gMemCard_szSaveFileName);
            if (index < 0 || (D_8009B3CC & 2)) {
                message = 0xD3;
                goto show_message;
            }
            MemCard_ReqReadSector(zero, MEM_CARD_WORK_FRAME_ADDRESS,
                *(s32 *)((u8 *)gMemCard_pDirEntries + 40 * index + 0x20) / 64);
            break;
        }
        if (D_8009B3D5) {
            message = 0xD8;
            goto show_message;
        }
        D_8009B3CF = 6;
    case 6:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            name = gMemCard_szSaveFileName;
            index = MemCard_FindLoadedEntry(name);
            message = 0xD3;
            if (index < 0 || (D_8009B3CC & 2))
                goto show_message;
            DuelEffect_CreateChannel(0x80D5, 0);
            slot->entry_index = index;
            D_8009B3CC |= 0x1000;
            MemCard_ReqReadFile(zero, (s32)name, MEM_CARD_WORK_READ_ADDRESS, 0x200, 0x1E00);
            break;
        }
        message = 0xD7;
        if (D_8009B3D5 == 1)
            message = 0xD8;
        goto show_message;
    case 7: {
        s32 wait;
        s32 result;
        if (!(D_8009B3CF & 0x80)) {
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
            Util_CopyWords((u8 *)MEM_CARD_WORK_WRITE_ADDRESS, gSaveData_aHeaderTemplate, 0x200);
            Util_FillMemory((u8 *)MEM_CARD_WORK_WRITE_BODY_ADDRESS, 0x86, 0x800);
            MemCard_ReqWriteFile(zero, (s32)name, MEM_CARD_WORK_WRITE_ADDRESS, 0, 0xA00);
            D_8009B3CC |= 0x1000;
            break;
        }
        result = D_8009B3D5;
        if (result == 1) {
            message = 0xD9;
            wait = result;
        } else {
            message = 0xD7;
            wait = 1;
            D_8009B3CF = 8;
        }
        DuelEffect_CreateChannel(message, wait);
        remaining = D_8009B3CC | 0x400;
        D_8009B3CC = remaining;
        remaining = 8;
        D_8009B3CF = remaining;
    }
    case 8:
        if (!(D_8009B3CF & 0x80)) {
            s32 i;
            s32 sector;
            D_8009B3CF |= 0x80;
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
            D_8021007F = checksum;
            if (sector < 0)
                sector += 63;
            MemCard_ReqWriteSector(zero, MEM_CARD_WORK_FRAME_ADDRESS, sector >> 6);
            break;
        }
        if (!D_8009B3D5)
            break;
        message = 0xD9;
show_message:
        DuelEffect_CreateChannel(message, 1);
        D_8009B3CC |= 0x400;
        break;
    case 9:
    case 10:
        if (!(D_8009B3CF & 0x80)) {
            D_8009B3CF |= 0x80;
            if (slot->free_blocks >= root->blocks)
                MemCard_ReqCreateFile(zero, (s32)gMemCard_szSaveFileName, root->blocks);
            break;
        }
        if (!D_8009B3D5)
            printf(D_80010398);
        printf(D_800103A4);
        break;
    }
}
