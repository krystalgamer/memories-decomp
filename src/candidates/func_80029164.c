/* Reclassified from matching_c (#3859). This was
 * src/game/duel_effect_resource_setup.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0). Under gcc_2_8_1_g0_split, a single
 * threshold, it is 31 of 31 instructions with 6 differing, opcode distance
 * 0. The source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../game/card_constants.h"
#include "../game/file_transfer.h"
#include "../game/duel_effect_resource_record.h"
#include "../game/duel_effect_resource_setup.h"

FileTransferDescriptor *func_80029164(s32 slot, s32 value)
{
    u8 *base = (u8 *)D_800EA0E8;
    u8 *record = &base[slot * 64];
    FileTransferDescriptor *object;

    *(u16 *)(record + 0x30) = value;
    object = File_TryRequestAsyncTransfer(
        0, 0, (value - 1) * 7 + CARD_COUNT, 7, func_800289BC, 0, 0);
    object->callback_data = (void *)slot;
    D_8009B0F4 =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return object;
}
