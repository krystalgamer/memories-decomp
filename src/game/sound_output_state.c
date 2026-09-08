#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"

extern s32 func_80045208(u16, s32);

void func_8004503C(short value, unsigned char flag, int unused)
{
    g_SDValue->field_0512 = value;
    g_SDValue->field_0049 = flag;
}

int func_80045054(void)
{
    int select = SpuReadDecodedData(
        (SpuDecodedData *)((u8 *)g_SDValue + 0x53C),
        SPU_CDONLY
    );
    register u8 *choice_state asm("$3") = (u8 *)g_SDValue;
    register short *values asm("$4");
    register int i asm("$6");
    register u8 *loaded asm("$2");
    register u8 *state asm("$5");

    *(int *)(choice_state + 0x538) = select;
    if (select == SPU_DECODED_FIRSTHALF)
        values = *(short **)(choice_state + 0x153C);
    else
        values = *(short **)(choice_state + 0x1540);
    loaded = (u8 *)g_SDValue;
    asm volatile("" : "+r"(loaded));
    i = 0;
    state = loaded;
    *(int *)(state + 0x154C) = 0;
    *(int *)(state + 0x1550) = 0;
    do {
        int value = *values;
        unsigned int square = value * value;
        *(unsigned int *)(state + 0x154C) += square >> 8;
        i++;
        values++;
    } while (i < SD_MIX_SAMPLE_COUNT);
    {
        register int result asm("$2");
        register int flags asm("$3");
        register int other asm("$4");
        state = (u8 *)g_SDValue;
        result = *(short *)(state + 0x154E);
        flags = *(u16 *)(state + 0x40);
        other = *(short *)(state + 0x1552);
        flags &= 3;
        *(int *)(state + 0x154C) = result;
        *(int *)(state + 0x1550) = other;
        if (flags)
            result = 0;
        return result;
    }
}

void func_80045114(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->flags_004A & 0x80) == 0)
        return;
    count = state->command_count;
    if (state->commands[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->commands[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->commands[count - 2].command == 0x11)
                return;
        }
    }
    func_8004503C(-0x20, 0, 1);
    {
        SDCommand local;
        local.command = 0x11;
        SD_EnqueueCommand(&local);
    }
}

s16 func_800451E0(u16 value)
{
    return func_80045208(value, 0x80);
}

#include "sound.h"

extern void func_800464F0(void);
extern void func_80044DA0(void);

s32 func_80045208(u16 arg0, s32 unused)
{
    SDValue *a = g_SDValue;
    u16 code = arg0;
    register u8 **table asm("$2");
    s32 kind;
    SDCommand req;

    if (a->flags_004A & 0x80) {
        if ((a->flags_004A & 0x40) || code <= 0x9FFF) {
            if (arg0 & 0x8000) {
                *(s16 *)((u8 *)a + 0x534) = arg0;
                switch (arg0 & 0xF000) {
                case 0x8000:
                    code = arg0 + 0x8000;
                    table = *(u8 ***)((u8 *)a + 0x51C);
                    kind = 0x50;
                    break;
                case 0x9000:
                    code = arg0 + 0x7000;
                    table = *(u8 ***)((u8 *)a + 0x518);
                    kind = 0x60;
                    break;
                default:
                    code = code + 0x6000;
                    kind = 0x70;
                    table = *(u8 ***)((u8 *)g_SDValue + 0x520);
                    break;
                }
                {
                    register s32 first asm("$16");
                    register u8 *const second asm("$17") = (u8 *)table + 8;

                    first = table ? (s32)*table : (s32)*table;

                    func_800464F0();
                    req.command = 0x24;
                    req.field_0002 = code;
                    req.field_0004 = first;
                    req.field_000C = (s32)second;
                    req.field_0008 = kind;
                    SD_EnqueueCommand(&req);
                    func_80044DA0();
                }
                g_SDValue->flags_0040 = (g_SDValue->flags_0040 | 1) & 0xFFFB;
                return 1;
            }
        }
    }
    return 0;
}

void func_80045334(s32 arg0)
{
    SDCommand req;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    s32 value;
    register s32 code asm("$19");
    register u8 **table asm("$2");
    register s32 kind asm("$18");

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
        code = arg0 + value;
        table = *(u8 ***)((u8 *)a + 0x51C);
        kind = 0x50;
        break;
    case 0x9000:
        code = arg0 + 0x7000;
        table = *(u8 ***)((u8 *)a + 0x518);
        kind = 0x60;
        break;
    default:
        code += 0x6000;
        kind = 0x70;
        b = g_SDValue;
        table = *(u8 ***)((u8 *)b + 0x520);
        break;
    }
    {
        register s32 first asm("$16");
        u8 *const second = (u8 *)table + 8;

        /* The equivalent paths preserve retail's request-store order. */
        first = table ? (s32)*table : (s32)*table;

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

#include "sound.h"

void SD_ClearBusyFlag(void)
{
    g_SDValue->busy = 0;
}
