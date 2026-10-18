#include "../types.h"

#define MODEL_SLOT_SETUP_EXPLICIT_TRANSFER_ARGS
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#define D_8009B118_IS_POINTER_IN_DATA
#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#include "../unmatched.h"
#include "model_record_tables.h"
#include "sound.h"
#include "model.h"
#include "file_names.h"
#include "file_transfer.h"
#include "file_transfer_steps.h"
#include "model_slot_setup.h"
#include "../psyq/libetc.h"
#include "../psyq/stdio.h"
#include "model_control.h"
#include "model_graphics_state.h"
#include "high_memory_addresses.h"
#include "model_slot_row_tables.h"
#include "model_link_slot_animation_rows.h"
#include "model_slot_support.h"
#include "model_slot_updates.h"
#include "model_apply_texture_tint.h"
#include "model_transfer_flags.h"
#include "func_8004DC38.h"
#include "sound_voice_data.h"
#include "script_state.h"
#include "model_load_step.h"
#include "model_word_memory.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "duel_load_package_stage.h"

/* Starts the asynchronous transfer that fills one model slot with a monster
 * merge record, and records the slot's display properties while the request is
 * in flight.
 *
 * The slot argument carries a flag in bit 0x80: when it is clear the sound
 * driver's voices are keyed off first, and the flag itself is stored at
 * +0xE1D.  A negative model id means "reuse the id already in the slot" and a
 * non-negative one is written back to +0xDF8, exactly as
 * Model_SetSlotProperties does; the remaining properties follow the same
 * "negative means leave alone" convention it uses for +0xDFA to +0xDFF.
 *
 * Duel slots (0 and 1) load out of the model MRG. Each compacted model entry
 * occupies MODEL_MRG_SECTOR_COUNT sectors, so the three id ranges that have no
 * record - MODEL_MRG_FIRST_GAP_START through the byte before
 * MODEL_MRG_FIRST_GAP_END, MODEL_MRG_SECOND_GAP_START through the byte before
 * MODEL_MRG_SECOND_GAP_END, and MODEL_MRG_SINGLE_GAP_ID - are rejected and
 * every id above such a gap is biased down by MODEL_MRG_GAP_SIZE.
 * MODEL_SPECIAL_BATTLE_ID is the one special case and comes from its own
 * file. Any other slot loads MODEL_AUX_SECTOR_COUNT sectors selected
 * through the MODEL_AUX_LOOKUP_RECORD_SIZE-byte table at D_80091008.
 *
 * Returns zero once a transfer has been requested and one when the model id
 * has no record to request. */
s32 Model_LoadMonsterMerge(s32 slot, s32 model, s32 p2, s32 p3, s32 p4,
                           s32 p5, s32 arg6)
{
    FileTransferDescriptor *transfer;
    s32 flags;
    s32 result;

    result = 0;
    flags = slot & 0x80;
    slot &= 0x7F;
    if (model < 0) {
        model = gModel_aSlots[slot].field_DF8;
    } else {
        gModel_aSlots[slot].field_DF8 = model;
    }
    if (flags == 0) {
        SD_KeyOffVoiceSlots();
    }
    File_WaitForTransfers();
    Model_InitSlot(slot, 0, 0, arg6);
    gModel_aSlots[slot].field_E1D = flags;
    if (slot < 2) {
        if (model == MODEL_SPECIAL_BATTLE_ID) {
            transfer = File_TryRequestAsyncTransfer(
                1, D_800114F8, MODEL_SPECIAL_BATTLE_FILE_START_SECTOR,
                MODEL_SPECIAL_BATTLE_FILE_SECTOR_COUNT,
                func_800577B0, 0, 0
            );
            D_8009B0F4_abs = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            gModel_aSlots[slot].field_E14 = 0;
            return 0;
        }
        if (model < 0 || model >= MODEL_MRG_ID_END
            || (model >= MODEL_MRG_FIRST_GAP_START
                && model < MODEL_MRG_FIRST_GAP_END)
            || (model >= MODEL_MRG_SECOND_GAP_START
                && model < MODEL_MRG_SECOND_GAP_END)
            || model == MODEL_MRG_SINGLE_GAP_ID) {
            result++;
        } else {
            if (model >= MODEL_MRG_LAST_ID) {
                model--;
            }
            if (model >= MODEL_MRG_SECOND_GAP_END) {
                model -= MODEL_MRG_GAP_SIZE;
            }
            if (model >= MODEL_MRG_FIRST_GAP_END) {
                model -= MODEL_MRG_GAP_SIZE;
            }
            transfer = File_TryRequestAsyncTransfer(
                2, gFile_szModelMrgPath, model * MODEL_MRG_SECTOR_COUNT,
                MODEL_MRG_SECTOR_COUNT, func_80056D7C,
                0, 0
            );
            if (p2 >= 0) {
                gModel_aSlots[slot].field_DFA = p2;
            }
            if (p3 >= 0) {
                gModel_aSlots[slot].field_DFC = p3;
            }
            if (p4 >= 0) {
                gModel_aSlots[slot].field_DFE = p4 != 0;
            }
            if (p5 >= 0) {
                gModel_aSlots[slot].field_DFF = p5 != 0;
            }
            transfer->callback_data = (void *)slot;
            transfer->position = gModel_aSlots[slot].field_DFE;
            D_8009B0F4_abs = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            gModel_aSlots[slot].field_E14 = 0;
        }
    } else {
        if (model >= 0 && model < 7) {
            model = D_80091008[
                model * MODEL_AUX_LOOKUP_RECORD_SIZE
                + MODEL_AUX_LOOKUP_VALUE_OFFSET];
            if (model >= 0 && model < 7) {
                goto found;
            }
        }
        result++;
        goto done;
found:
        transfer = File_TryRequestAsyncTransfer(
            1, D_800114F8,
            model * MODEL_AUX_SECTOR_COUNT + MODEL_AUX_FILE_START_SECTOR,
            MODEL_AUX_SECTOR_COUNT, func_80057544, 0, 0
        );
        D_8009B0F4_abs = transfer->status_flags
            | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
        gModel_aSlots[slot].field_E14 = 0;
    }
done:
    return result;
}

static inline s16 load_phase(ModelSlot *slot)
{
    return slot->field_E14;
}

void Model_AdvanceSlotLoad(s32 index)
{
    ModelSlot *slot = &gModel_aSlots[index];
#ifndef VERSION_JAPAN
    s32 started = VSync(1);
#endif
    s32 state = slot->field_E14;
    s32 work;
    s32 amount;
    s32 offset;

    if (state == 0 || state == 255) {
        return;
    }
    switch (load_phase(slot)) {
    case 1: {
        u8 *payload = (u8 *)D_80010008;
        s32 size = 0xC000;
        switch (index) {
        case 0:
            payload = D_80010000;
            break;
        case 1:
            payload = D_80010004;
            break;
        }
        if (*(s32 *)payload != 0) {
            size = *(s32 *)payload;
        }
        Model_InitSlot(index, payload, size, -1);
        break;
    }
    case 2:
        Model_BuildSlotAnimationTables(index);
        break;
    case 3:
        Model_LinkSlotAnimationRows(index);
        break;
    case 4: {
        ModelSlot *p = &gModel_aSlots[index];
        s32 sum = 0;
        s32 i, count, byte, missing;
        if (index < 2) {
            i = sum;
            if (sum < p->field_E1B) {
                missing = 0xFFFF;
                count = p->field_E1B;
                do {
                    if (p->field_2C8[1][i] != missing) {
                        byte = i / 8;
                        if ((p->field_BEC[byte] >> (i - byte * 8)) & 1) {
                            sum += 0x14;
                        } else {
                            sum += 0xC;
                        }
                    }
                    i++;
                } while (i < count);
            }
        }
        amount = (p->field_DF0 = (s32)p->field_DE0 + sum);
        p->field_DF4 = amount + p->field_E02 * 4;
        func_8005A4C4(p, 0, 0, 0, index == 1 ? 0x800 : 0);
        break;
    }
    case 5: {
        ModelSlot *p = &gModel_aSlots[index];
        ModelSlotPart *G32 *part = p->field_1E0;
        s32 i;
        for (i = 0; i < p->field_E1B; i++) {
            (*part)->ii = 0xFFFF;
            (*part)->ti = (*part)->start;
            (*part)->aframe = 0xFFFF;
            (*part)->sid = (*part)->start_sid;
            (*part)->rframe = 0;
            (*part)->speed = 0x10;
            part++;
        }
        break;
    }
    case 6:
        slot->field_E1F = 1;
        func_800590DC(index);
        slot->field_E1F = 0;
        break;
    case 7: {
        ModelSlot *p = &gModel_aSlots[index];
        amount = p->field_E0D * 2;
        offset = amount;
        switch (p->field_E16) {
        case 0x23:
            offset = 0;
            func_8005A468(index, offset);
            break;
        case 0x3E:
            offset = amount;
            func_8005A468(index, offset);
            break;
        case 0x3C:
            offset = -offset;
            func_8005A468(index, offset);
            break;
        }
        if (index >= 2) {
            slot->field_E1F = 1;
        }
        break;
    }
    case 8:
        Model_ApplyTextureTint(index, slot->field_E0C, slot->field_E0A);
        break;
    case 9: {
        ModelSlot *p = &gModel_aSlots[index];
        u8 *context;
        ModelControlHandler handler;
        s32 selected;
        s32 position;
        s32 i;
        work = ((ModelControlCommandView *)p)->commands[2];
        context = (u8 *)p->field_DE8;
        D_8009AFA0 = index;
        if (work >= 0) {
            if (index != 0) {
                handler = (ModelControlHandler)(D_80010010 + 4);
            } else {
                handler = (ModelControlHandler)(D_8001000C + 4);
            }
            func_8005F198(1);
            handler(context, ((ModelControlCommandView *)p)->commands[2] % 1000);
            func_8005F198(0);
        }
        selected = p->field_BF5;
        p->field_E0E = 6;
        position = p->field_E06;
        for (i = 0; i < p->field_E1B; i++) {
            s32 current = p->field_BF5;
            if (selected != 0) {
                p->field_1E0[i]->sid = selected;
                current = selected;
            }
            Model_SeekSlotPartAnimation(p, i, current, position);
        }
        if (selected != 0) {
            p->field_BF5 = selected;
        }
        break;
    }
    case 10:
        if (slot->field_E1D == 0) {
            SD_LoadSequenceBankPair(
                index, (u32 *)(D_801A8000 + (index << 11))
            );
        }
        break;
    case 11:
        if (slot->field_E1D == 0) {
            slot->field_E1E = 1;
        }
        slot->field_E1F = 1;
        break;
    }
#ifndef VERSION_JAPAN
    work = VSync(1);
    printf(D_80011594, slot->field_E14, work - started);
#endif
    {
        s32 next;
        if (slot->field_E1F != 0) {
            next = 255;
        } else {
            next = slot->field_E14 + 1;
        }
        slot->field_E14 = next;
    }
}

/*
 * Contiguous model file-transfer phase callbacks. The first handles MRG
 * transfer phases for the two duel slots. The entry slot and
 * alternate-palette selector are captured before dispatch; the finish stage
 * reloads the slot overrides after the copy and optional state callback.
 *
 * All 1,992 text bytes and the 17-entry table match the uniform G8 profile.
 * Separate cleared/reloaded flag captures retain the upload-stage ordering.
 * The direct image argument and post-call source cursor share the same
 * staging buffer without extending the cursor's lifetime across the upload.
 */
#ifndef MODEL_TEXTURE_STAGE3_RECT_X
#define MODEL_TEXTURE_STAGE3_RECT_X(index) ((index) << 8)
#endif
#ifndef MODEL_TEXTURE_STAGE3_RECT_Y
#define MODEL_TEXTURE_STAGE3_RECT_Y(index) 0xF8
#endif
#ifndef MODEL_TEXTURE_PALETTE_RECT_X
#define MODEL_TEXTURE_PALETTE_RECT_X 0x200
#endif
#ifndef MODEL_TEXTURE_TAIL_ROW_Y
#define MODEL_TEXTURE_TAIL_ROW_Y 0xF0
#endif

void func_80056D7C(FileTransferDescriptor *d, s32 stage)
{
    RECT rect0;
    RECT rect1;
    s32 index;
    s32 alternate;
    ModelSlot *slot;

    index = (s32)d->callback_data;
    alternate = d->position;
    slot = &gModel_aSlots[index];
    switch (stage) {
    case 0: {
        u8 *destination;
        if (index == 0) {
            destination = D_80010000;
        } else {
            destination = D_80010004;
        }
        d->value_08 = d->value_0C = (u32)destination;
        d->phase_size = 96 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    }
    case 1:
        d->field_30.h.counter = index << 8;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        d->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        d->phase_size = 48 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs |= 0x10000;
        d->done = 2;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;
    case 2:
        d->value_08 = d->value_0C = (u32)D_801DD000;
        d->phase_size = 2 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 3: {
        u32 mask;
        u8 *source;
        RECT *rect;
        do { rect = &rect0; } while (0);
        rect0.x = MODEL_TEXTURE_STAGE3_RECT_X(index);
        rect0.y = MODEL_TEXTURE_STAGE3_RECT_Y(index);
        rect0.w = 0x100;
        rect0.h = 8;
        LoadImage2(rect, (u32 *)D_801DD000);
        source = D_801DD000;
        do { mask = 0xFFDCFFFF; } while (0);
        source += 2 * FILE_SECTOR_SIZE;
        d->value_08 = d->value_0C = (u32)source;
        d->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= mask;
        d->done = 1;
        break;
    }
    case 4:
        if (alternate == 0) {
            rect1.x = MODEL_TEXTURE_PALETTE_RECT_X;
            rect1.y = index + 0xF2;
            rect1.w = 0x100;
            rect1.h = 1;
            LoadImage2(&rect1, (u32 *)D_801DE000);
            d->field_30.h.counter = (index << 8) + 0xC0;
            d->w = 0x40;
            d->h = 0x10;
            {
                u32 cleared, loaded, enabled;
                cleared = D_8009B0F4_abs & 0xFFDDFFFF;
                D_8009B0F4_abs = cleared;
                do { loaded = D_8009B0F4_abs; } while (0);
                do { enabled = 0x10000; } while (0);
                d->field_30.h.field_32 = 0x100;
                D_8009B0F4_abs = loaded | enabled;
            }
            d->done = 2;
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            d->value_08 = (u32)D_8009B118;
            d->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        } else {
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 6:
        if (alternate == 1) {
            rect1.x = MODEL_TEXTURE_PALETTE_RECT_X;
            rect1.y = index + 0xF2;
            rect1.w = 0x100;
            rect1.h = 1;
            LoadImage2(&rect1, (u32 *)D_801DD000);
            d->field_30.h.counter = (index << 8) + 0xC0;
            d->w = 0x40;
            d->h = 0x10;
            {
                u32 cleared, loaded, enabled;
                cleared = D_8009B0F4_abs & 0xFFDDFFFF;
                D_8009B0F4_abs = cleared;
                do { loaded = D_8009B0F4_abs; } while (0);
                do { enabled = 0x10000; } while (0);
                d->field_30.h.field_32 = 0x100;
                D_8009B0F4_abs = loaded | enabled;
            }
            d->done = 2;
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            d->value_08 = (u32)D_8009B118;
            d->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        } else {
            d->phase_size = 16 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 7:
        if (alternate == 0 && index == 0) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010014;
            d->done = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 8:
        if (alternate == 0 && index == 1) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010018;
            d->done = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 9:
        if (alternate == 1 && index == 0) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010014;
            d->done = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 10:
        if (alternate == 1 && index == 1) {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010018;
            d->done = 1;
        } else {
            d->phase_size = 10 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 11:
        if (index == 0) {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = (u32)D_8001000C;
            d->done = 1;
        } else {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 12:
        if (index == 1) {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = (u32)D_80010010;
            d->done = 1;
        } else {
            d->phase_size = 2 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 13:
        if (slot->field_E1D != 0) {
            d->phase_size = FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        } else {
            d->value_08 = d->value_0C =
                (u32)(D_801A8000 + index * FILE_SECTOR_SIZE);
            d->phase_size = FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->done = 1;
        }
        break;
    case 14:
        if (slot->field_E1D != 0) {
            u32 mask;
            do { mask = 0xFFFCFFFF; } while (0);
            d->phase_size = 50 * FILE_SECTOR_SIZE;
            D_8009B0F4_abs &= mask;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        } else {
            d->done = 3;
            d->field_30.word = index * 0x19000 + 0xD810;
            d->phase_size = 50 * FILE_SECTOR_SIZE;
            d->value_08 = (u32)D_8009B118;
            d->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        }
        break;
    case 5:
    case 15:
        d->value_08 = d->value_0C = (u32)D_801DD000;
        d->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 16: {
        u8 *source;
        source = D_801DD000;
        Model_CopyWords(
            (s32 *)slot->sound_entries,
            (const s32 *)source,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotCF8BlockWords *)&slot->field_CF8 =
            ((ModelTransferMetadata *)source)->field_CF8;
        if (slot->field_CF8.prefix.values.field_08 & 0x20200000) {
            func_80059284(index, 4);
        }
        if (slot->field_DFA < 0xFFFF) {
            slot->field_CF8.prefix.values.field_00 = slot->field_DFA;
        }
        if (slot->field_DFC < 0xFFFF) {
            slot->field_CF8.prefix.values.field_02 = slot->field_DFC;
        }
        if (slot->field_E1D != 0) {
            slot->field_CF8.field_10 = -1;
            slot->field_CF8.field_14 = -1;
            slot->field_CF8.field_18 = -1;
        }
        slot->field_E14 = 1;
        break;
    }
    }
}

/* Two transfer-phase callbacks of the FreeDuel_LoadPackageStage family, kept
   in one translation unit because they are the same routine over different
   assets: each takes the descriptor plus a phase index, and each switch arm
   programs the same fields -- phase_size, the value_08/value_0C source
   window, done, and the field_30 halfword pair -- while clearing and setting
   the same bits of D_8009B0F4_abs.

   Both own a .rodata jump table, and the two tables are adjacent in the image
   (0x1F84 for the six-case switch, 0x1F9C for the eleven-case one). Together
   with func_80056D7C's 17-entry table and the retail zero word after it, the
   complete object owns the 0x8C-byte .rodata run at 0x1F3C. Definition order
   is address order and must stay that way. */

void func_80057544(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;

    switch (mode) {
    case 0:
        object->phase_size = 18 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010008;
        object->value_08 = D_80010008;
        object->done = 1;
        break;

    case 1:
        object->field_30.h.counter = 0x200;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;

    case 2:
        object->value_0C = (s32)D_801DD800;
        object->value_08 = (s32)D_801DD800;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        rect0.x = MODEL_TEXTURE_PALETTE_RECT_X;
        rect0.y = 0xF1;
        rect0.w = 0x100;
        rect0.h = 1;
        LoadImage2(&rect0, (u32 *)D_801DD800);
        object->value_0C = (s32)(D_801DD800 - FILE_SECTOR_SIZE);
        object->value_08 = (s32)(D_801DD800 - FILE_SECTOR_SIZE);
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 4:
        rect1.x = MODEL_TEXTURE_PALETTE_RECT_X;
        rect1.y = 0xF4;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)D_801DD000);
        object->field_30.h.counter = 0x380;
        object->field_30.h.field_32 = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 32 * FILE_SECTOR_SIZE;
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;

    case 5:
        D_800F5694[0] = 1;
        break;
    }
}

void func_800577B0(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;
    ModelSlot *dst;
    u8 *src;

    switch (mode) {
    case 0:
        object->phase_size = 96 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = (s32)D_80010000;
        object->value_08 = (s32)D_80010000;
        object->done = 1;
        break;

    case 1:
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->field_30.h.counter = 0;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 96 * FILE_SECTOR_SIZE;
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;

    case 2:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->phase_size = 2 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
#ifdef VERSION_EUROPE
        /* The European build stores the row's x first. */
        rect0.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
        rect0.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        rect0.w = 0x100;
#else
        rect0.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        rect0.w = 0x100;
        rect0.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
#endif
        rect0.h = 8;
        LoadImage2(&rect0, (u32 *)D_801DD000);
        object->phase_size = 10 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010014;
        object->value_08 = D_80010014;
        object->done = 1;
        break;

    case 4:
        object->phase_size = 10 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010018;
        object->value_08 = D_80010018;
        object->done = 1;
        break;

    case 6:
        rect1.x = MODEL_TEXTURE_STAGE3_RECT_X(1);
        rect1.y = MODEL_TEXTURE_TAIL_ROW_Y;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)D_801DD000);
        object->field_30.h.counter = 0x180;
        object->w = 0x40;
        object->field_30.h.field_32 = 0x100;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 8 * FILE_SECTOR_SIZE;
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;

    case 7:
        object->value_0C = (s32)D_801A8000;
        object->value_08 = (s32)D_801A8000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 8:
        object->done = 3;
        object->field_30.word = 0xD810;
        object->phase_size = 50 * FILE_SECTOR_SIZE;
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;

    case 5:
    case 9:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 10:
        dst = gModel_aSlots;
        src = D_801DD000;
        Model_CopyWords(
            (s32 *)dst->sound_entries,
            (const s32 *)src,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotCF8BlockWords *)&dst->field_CF8 =
            ((ModelTransferMetadata *)src)->field_CF8;
        dst->field_CF8.field_10 = -1;
        dst->field_CF8.field_14 = -1;
        dst->field_CF8.field_18 = -1;
        dst->field_CF8.prefix.values.field_00 = 0;
        dst->field_CF8.prefix.values.field_02 = 0;
        dst->field_E14 = 1;
        break;
    }
}
