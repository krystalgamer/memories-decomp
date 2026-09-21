#include "../types.h"
#include "text_constants.h"
#include "duel_effect.h"
#include "text_stream_read_u16_le.h"
#include "../unmatched.h"

#define TEXT_STREAM_OWNER(object) ((TextStreamOwner *)(object))

void func_80038388(DuelEffectChannel *object)
{
    object->field_38 = TextStream_ReadU16LE(object);
}

void func_800383B0(DuelEffectChannel *object)
{
    object->field_60 = 0;
    object->field_61 = TextStream_ReadU16LE(object);
}

u32 *func_800383DC(DuelEffectChannel *a0) {
    DuelEffectChannel *a3 = a0;
    s32 a2 = D_8009B32E;
    u32 v1;
    u8 counter;
    u32 *slot;

    if (a2 > 0xCFFF) {
        v1 = ((u32)D_801C0000 & TEXT_BANK_ADDRESS_MASK) +
             D_801C0000[a2 - 0xD000];
    } else if (a2 > (TEXT_GLOBAL_STRING_ID_BASE - 1)) {
        v1 = ((u32)D_801D5800 & TEXT_BANK_ADDRESS_MASK) +
             D_801D5800[a2 - TEXT_GLOBAL_STRING_ID_BASE];
    } else {
        if (a2 >= 0x500) {
            a2 -= 0x100;
        }
        v1 = ((u32)D_801B0000 & TEXT_BANK_ADDRESS_MASK) + D_801C0000[a2];
    }

    counter = *(u8 *)&a3->stream_58 + 1;
    *(u8 *)&a3->stream_58 = counter;
    slot = (u32 *)&TEXT_STREAM_OWNER(a3)->streams[(s8)counter];
    *slot = v1;
    return slot;
}
