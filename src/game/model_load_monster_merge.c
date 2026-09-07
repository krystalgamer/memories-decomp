#include "../types.h"
#include "model.h"
#include "file_transfer.h"

extern volatile s32 D_8009B0F4;
extern u8 D_800114F8[];
extern u8 gFile_szModelMrgPath[];
extern u8 D_80091008[];

void SD_KeyOffVoiceSlots(void);
void func_800137E4(void);
void func_8004CB0C(s32 slot, s32 arg1, s32 arg2, s32 arg3);
void func_800577B0(void);
void func_80056D7C(void);
void func_80057544(void);

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
 * Duel slots (0 and 1) load out of the model MRG.  Its records are 0x114 bytes
 * and are indexed by a compacted model id, so the three id ranges that have no
 * record - 0x12C to 0x15D, 0x28A to 0x2BB and 0x2D0 - are rejected and every
 * id above such a gap is biased down by the gap's width.  Id 0x309 is the one
 * special case and comes from its own file.  Any other slot loads a 0x74-byte
 * record selected through the 0xB2-byte table at D_80091008.
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
        model = D_800F2C40[slot].field_DF8;
    } else {
        D_800F2C40[slot].field_DF8 = model;
    }
    if (flags == 0) {
        SD_KeyOffVoiceSlots();
    }
    func_800137E4();
    func_8004CB0C(slot, 0, 0, arg6);
    /* +0xE1D, which model.h still covers with pad_E1C. */
    D_800F2C40[slot].pad_E1C[1] = flags;
    if (slot < 2) {
        if (model == 0x309) {
            transfer = File_TryRequestAsyncTransfer(
                1, D_800114F8, 0x3B4, 0x113, func_800577B0, 0, 0
            );
            D_8009B0F4 = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            D_800F2C40[slot].field_E14 = 0;
            return 0;
        }
        if (model < 0 || model > 0x2D1
            || (model >= 0x12C && model <= 0x15D)
            || (model >= 0x28A && model <= 0x2BB)
            || model == 0x2D0) {
            result++;
        } else {
            if (model >= 0x2D1) {
                model--;
            }
            if (model >= 0x2BC) {
                model -= 0x32;
            }
            if (model >= 0x15E) {
                model -= 0x32;
            }
            transfer = File_TryRequestAsyncTransfer(
                2, gFile_szModelMrgPath, model * 0x114, 0x114, func_80056D7C,
                0, 0
            );
            if (p2 >= 0) {
                D_800F2C40[slot].field_DFA = p2;
            }
            if (p3 >= 0) {
                D_800F2C40[slot].field_DFC = p3;
            }
            if (p4 >= 0) {
                D_800F2C40[slot].field_DFE = p4 != 0;
            }
            if (p5 >= 0) {
                D_800F2C40[slot].field_DFF = p5 != 0;
            }
            transfer->callback_data = (void *)slot;
            transfer->position = D_800F2C40[slot].field_DFE;
            D_8009B0F4 = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            D_800F2C40[slot].field_E14 = 0;
        }
    } else {
        if (model >= 0 && model < 7) {
            model = D_80091008[model * 0xB2 + 0xA0];
            if (model >= 0 && model < 7) {
                goto found;
            }
        }
        result++;
        goto done;
found:
        transfer = File_TryRequestAsyncTransfer(
            1, D_800114F8, model * 0x74 + 0x88, 0x74, func_80057544, 0, 0
        );
        D_8009B0F4 = transfer->status_flags
            | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
        D_800F2C40[slot].field_E14 = 0;
    }
done:
    return result;
}
