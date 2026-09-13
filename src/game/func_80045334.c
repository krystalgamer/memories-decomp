#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_state.h"

#include "sound_buffer_init.h"

void func_80045334(s32 arg0)
{
    SDCommand req;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    s32 value;
    u16 code;
    u8 **table;
    s32 kind;
    u8 *second;

    a = g_SDValue;
    code = arg0;
    if ((a->flags_004A & 0x80) == 0) {
        return;
    }
    if ((a->flags_004A & 0x40) == 0) {
        if ((u32)(code & 0xFFFF) > 0x9FFF) {
            return;
        }
    }
    if ((arg0 & 0x8000) == 0) {
        return;
    }
    value = arg0 & 0xF000;
    *(s16 *)((u8 *)a + 0x534) = arg0;
    switch (value) {
    case 0x8000:
        value = arg0 + value;
        code = value;
        table = *(u8 ***)((u8 *)a + 0x51C);
        second = (u8 *)table + 8;
        kind = 0x50;
        break;
    case 0x9000:
        code = arg0 + 0x7000;
        table = *(u8 ***)((u8 *)a + 0x518);
        second = (u8 *)table + 8;
        kind = 0x60;
        break;
    default:
        code += 0x6000;
        kind = 0x70;
        b = g_SDValue;
        table = *(u8 ***)((u8 *)b + 0x520);
        second = (u8 *)table + 8;
        break;
    }
    {
        s32 first;

        /* The equivalent paths preserve retail's request-store order. */
        first = table ? (second ? (s32)*table : (s32)*table) : (s32)*table;

        func_800464F0();
        req.command = 0x21;
        req.field_0002 = code;
        req.field_0004 = first;
        req.field_000C = (s32)second;
        req.field_0008 = kind;
        SD_EnqueueCommand(&req);
    }
    c = g_SDValue;
    c->flags_0040 = (c->flags_0040 | 1) & 0xFFFB;
}

