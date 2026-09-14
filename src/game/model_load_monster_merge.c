#include "../types.h"
#include "../unmatched.h"
#include "model_record_tables.h"
#include "sound.h"
#include "model.h"
#include "file_names.h"
#include "file_transfer.h"
#include "file_transfer_steps.h"
#define MODEL_SLOT_SETUP_EXPLICIT_TRANSFER_ARGS
#include "model_slot_setup.h"

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
        model = D_800F2C40[slot].field_DF8;
    } else {
        D_800F2C40[slot].field_DF8 = model;
    }
    if (flags == 0) {
        SD_KeyOffVoiceSlots();
    }
    File_WaitForTransfers();
    func_8004CB0C(slot, 0, 0, arg6);
    D_800F2C40[slot].field_E1D = flags;
    if (slot < 2) {
        if (model == MODEL_SPECIAL_BATTLE_ID) {
            transfer = File_TryRequestAsyncTransfer(
                1, D_800114F8, MODEL_SPECIAL_BATTLE_FILE_START_SECTOR,
                MODEL_SPECIAL_BATTLE_FILE_SECTOR_COUNT,
                func_800577B0, 0, 0
            );
            D_8009B0F4_abs = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            D_800F2C40[slot].field_E14 = 0;
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
            D_8009B0F4_abs = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            D_800F2C40[slot].field_E14 = 0;
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
        D_800F2C40[slot].field_E14 = 0;
    }
done:
    return result;
}
