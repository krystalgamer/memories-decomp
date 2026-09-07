#include "../types.h"
#include "card_constants.h"

/* Duel effect sequencer step. Walks a list of s16 counts; each call in
   mode 1 advances the counter, finishing the current entry when it hits
   *list (result 1) or handing off with bit 21 of the busy mask raised
   (result 2). Mode 0 re-arms from the current entry.
   D_8009B0F4 is the duel busy/pending bitmask (also touched by the CD-IRQ
   callback chain, hence volatile). */
struct Seq {
    char pad0[0x8];
    u32 val8;      /* 0x08 */
    u32 valC;      /* 0x0C */
    char pad1[0xC];
    u32 mode1c;    /* 0x1C */
    char pad2[0x10];
    u16 counter;   /* 0x30 */
    char pad3[0x6];
    s16 *list;     /* 0x38 */
    u32 pos;       /* 0x3C */
    u32 result;    /* 0x40 */
    char pad4[0x2];
    u8 done;       /* 0x46 */
};

extern volatile u32 D_8009B0F4;

void func_800245EC(struct Seq *o, int mode) {
    s16 *p = o->list;

    if (mode == 0) {
        o->counter = *p;
        o->mode1c = 0x800;
        D_8009B0F4 &= ~0x230000;
        o->val8 = o->valC = o->pos;
        o->done = 1;
        return;
    }
    if (mode == 1) {
        p++;
        o->list = p;
        o->pos += DUEL_CARD_DATA_BLOCK_SIZE;
    }
    o->counter++;
    if (o->counter == *p) {
        o->mode1c = 0x800;
        o->result = 1;
        D_8009B0F4 &= ~0x230000;
        o->val8 = o->valC = o->pos;
        o->done = 1;
    } else {
        o->mode1c = 0x800;
        o->result = 2;
        D_8009B0F4 &= ~0x30000;
        D_8009B0F4 |= 0x200000;
        o->done = 1;
    }
}
