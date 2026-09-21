#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_state.h"

void func_8004503C(s16 value, u8 flag, s32 unused)
{
    g_SDValue->field_0512 = value;
    g_SDValue->field_0049 = flag;
}

#include "sound_buffer_init.h"

/*
 * func_80045054: decoded output measurement without register pins
 *
 * The 192-byte routine matches all 48 instructions under the unit's
 * gcc_2_8_1_g0 profile. The same-symbol alias below retains its measured
 * volatile pointer view while the surrounding functions keep sound.h's
 * ordinary declaration.
 *
 * Three source properties replace the historical eight register bindings and
 * compiler barrier. The volatile pointer view retains the snapshot-to-state
 * handoff; removing it loses one instruction. Returning the level directly on
 * the unmuted path gives the required return-register allocation. Finally, the
 * single-iteration level-read scope keeps the signed high-halfword read before
 * the flag read; flattening it exchanges the two words at +0x90 and +0x94.
 *
 * The routine preserves the selected CD half, all 256 signed sample squares,
 * the unsigned eight-bit shift of each square, both accumulator
 * initializations and final publications, and the low-two-bit output gate.
 */
extern SDValue *volatile g_SDValue_output_level asm("g_SDValue");

s32 func_80045054(void)
{
    s32 select = SpuReadDecodedData(
        (SpuDecodedData *)g_SDValue_output_level->buffer_053C, SPU_CDONLY
    );
    SDValue *choice_state = g_SDValue_output_level;
    s16 *values;
    s32 i;
    SDValue *loaded;
    SDValue *state;

    choice_state->decoded_half = select;
    if (select == SPU_DECODED_FIRSTHALF) {
        values = (s16 *)choice_state->buffer_ptrs_153C[0];
    } else {
        values = (s16 *)choice_state->buffer_ptrs_153C[1];
    }
    loaded = g_SDValue_output_level;
    i = 0;
    state = loaded;
    state->output_level.sum = 0;
    state->field_1550.sum = 0;
    do {
        s32 value = *values;
        u32 square = value * value;
        state->output_level.sum += square >> 8;
        i++;
        values++;
    } while (i < SD_MIX_SAMPLE_COUNT);
    {
        s32 result;
        s32 flags;
        s32 other;
        state = g_SDValue_output_level;
        do {
            result = state->output_level.halves[1];
        } while (0);
        flags = state->flags_0040;
        other = state->field_1550.halves[1];
        flags &= 3;
        state->output_level.sum = result;
        state->field_1550.sum = other;
        if (!flags) {
            return result;
        }
        return 0;
    }
}

void func_80045114(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->flags_004A & 0x80) == 0)
        return;
    count = state->command_count;
    if (state->commands.c[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->commands.c[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->commands.c[count - 2].command == 0x11)
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

s16 func_800451E0(u16 value, s32 unused)
{
    return func_80045208(value, 0x80);
}

s32 func_80045208(u16 arg0, s32 unused)
{
    SDValue *a = g_SDValue;
    u16 code = arg0;
    u8 **table;
    s32 kind;
    u8 *second;
    SDCommand req;

    if (a->flags_004A & 0x80) {
        if ((a->flags_004A & 0x40) || code <= 0x9FFF) {
            if (arg0 & 0x8000) {
                *(s16 *)&a->field_0534 = arg0;
                switch (arg0 & 0xF000) {
                case 0x8000:
                    code = arg0 + 0x8000;
                    table = (u8 **)a->bank_0518[1];
                    second = (u8 *)&table[2];
                    kind = 0x50;
                    break;
                case 0x9000:
                    code = arg0 + 0x7000;
                    table = (u8 **)a->bank_0518[0];
                    second = (u8 *)&table[2];
                    kind = 0x60;
                    break;
                default:
                    code = code + 0x6000;
                    kind = 0x70;
                    table = (u8 **)g_SDValue->bank_0518[2];
                    second = (u8 *)&table[2];
                    break;
                }
                {
                    s32 first;

                    first = table ? (second ? (s32)*table : (s32)*table) : (s32)*table;

                    func_800464F0();
                    req.command = 0x24;
                    req.field_0002 = code;
                    req.field_0004 = first;
                    req.field_000C = (s32)second;
                    req.field_0008 = kind;
                    SD_EnqueueCommand(&req);
                    SD_ResetCdPan();
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
    *(s16 *)&a->field_0534 = arg0;
    switch (value) {
    case 0x8000:
        value = arg0 + value;
        code = value;
        table = (u8 **)a->bank_0518[1];
        second = (u8 *)&table[2];
        kind = 0x50;
        break;
    case 0x9000:
        code = arg0 + 0x7000;
        table = (u8 **)a->bank_0518[0];
        second = (u8 *)&table[2];
        kind = 0x60;
        break;
    default:
        code += 0x6000;
        kind = 0x70;
        b = g_SDValue;
        table = (u8 **)b->bank_0518[2];
        second = (u8 *)&table[2];
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

void SD_ClearBusyFlag(void)
{
    g_SDValue->busy = 0;
}
