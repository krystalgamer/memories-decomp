#include "../types.h"
#include "card_constants.h"
#include "file_transfer.h"

/* Duel effect sequencer step. Walks a list of s16 counts; each call in
   mode 1 advances the counter, finishing the current entry when it hits
   *list (result 1) or handing off with bit 21 of the busy mask raised
   (result 2). Mode 0 re-arms from the current entry.
   D_8009B0F4 is the duel busy/pending bitmask (also touched by the CD-IRQ
   callback chain, hence volatile). */
extern volatile u32 D_8009B0F4;

void func_800245EC(FileTransferDescriptor *o, int mode) {
    s16 *p = o->callback_data;

    if (mode == 0) {
        o->counter = *p;
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
    o->counter++;
    if (o->counter == *p) {
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
