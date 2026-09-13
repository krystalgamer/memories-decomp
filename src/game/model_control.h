#ifndef MEMORIES_DECOMP_MODEL_CONTROL_H
#define MEMORIES_DECOMP_MODEL_CONTROL_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "model.h"

/* A loaded module's control entry follows its first word. */
typedef s32 (*ModelControlHandler)(u8 *context, s32 command);

/* Non-owning view of the signed command words at +0xD08 through +0xD10.
 * The final word is also the separately tested primary command. */
typedef struct {
    u8 prefix[0xD08];
    s32 commands[3];
} ModelControlCommandView;

typedef char ModelControlCommandOffset[
    (u32)&((ModelControlCommandView *)0)->commands ==
        (u32)&((ModelSlot *)0)->field_CF8 + 0x10 ? 1 : -1
];
typedef char ModelControlCommandViewFitsSlot[
    sizeof(ModelControlCommandView) <= sizeof(ModelSlot) ? 1 : -1
];

void func_800559D4(s32 index);

#endif
