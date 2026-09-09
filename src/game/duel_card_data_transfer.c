#include "../types.h"
#include "card_constants.h"
#include "file_transfer.h"

/* Walks the sorted card-ID boundary list while advancing through the loaded
   card-data blocks. Mode 0 initializes the current range; mode 1 advances to
   the next unique card. */

void Duel_StepCardDataTransfer(FileTransferDescriptor *o, int mode) {
    s16 *p = o->callback_data;

    if (mode == 0) {
        o->field_30.h.counter = *p;
        o->mode = 0x800;
        D_8009B0F4 &= ~0x230000;
        o->value_08 = o->value_0C = o->position;
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
        o->mode = 0x800;
        o->result = 1;
        D_8009B0F4 &= ~0x230000;
        o->value_08 = o->value_0C = o->position;
        o->done = 1;
    } else {
        o->mode = 0x800;
        o->result = 2;
        D_8009B0F4 &= ~0x30000;
        D_8009B0F4 |= 0x200000;
        o->done = 1;
    }
}
