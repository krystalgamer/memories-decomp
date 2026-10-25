#define D_8009B0F4 gFile_dwTransferState
#include "../../types.h"
#include "../card_constants.h"
#include "../duel_card_data_transfer.h"
#include "../file_transfer.h"
#include "../file_transfer_state.h"

void Duel_StepCardDataTransfer(FileTransferDescriptor *o, int mode)
{
    s16 *p = o->callback_data;

    if (mode == 0) {
        o->field_30.h.counter = *p;
        o->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= ~0x230000;
        o->sector_destination = o->alternate_sector_destination = o->position;
        o->done = 1;
        return;
    }
    if (mode == 1) {
        p++;
        o->callback_data = p;
        o->position += DUEL_CARD_DATA_BLOCK_SIZE;
    }
    o->field_30.h.counter++;
    if (o->field_30.h.counter == *p) {
        o->phase_size = FILE_SECTOR_SIZE;
        o->phase_index = 1;
        gFile_dwTransferState &= ~0x230000;
        o->sector_destination = o->alternate_sector_destination = o->position;
        o->done = 1;
    } else {
        o->phase_size = FILE_SECTOR_SIZE;
        o->phase_index = 2;
        gFile_dwTransferState &= ~0x30000;
        gFile_dwTransferState |= 0x200000;
        o->done = 1;
    }
}
