#ifndef MEMORIES_DECOMP_MODEL_TRANSFER_STATE_H
#define MEMORIES_DECOMP_MODEL_TRANSFER_STATE_H

#include "../types.h"

/* One row of the eight-byte transfer request table func_8005FB30 walks: the
   model id it is asked to stage and the state it reports back. The four
   bytes between them are what the caller left there. */
typedef struct {
    s16 id;
    u8 pad_02[4];
    s16 state;
} ModelTransferItem;

typedef char ModelTransferItem_size_must_be_8[
    sizeof(ModelTransferItem) == 8 ? 1 : -1
];

void func_8005FAE4(void);
s32 func_8005FB08(void);
s32 func_8005FB14(void);
void func_8005FB30(u8 *data);

#endif
