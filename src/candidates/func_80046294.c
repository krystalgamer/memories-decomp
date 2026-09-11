/*
 * Current best under gcc_2_8_1_g8_split: 151/151 instructions, opcode
 * distance 0, five differing positions. Per-arm copy pointers are required
 * because retail uses v0/v1 in the first copy arm and a0/v0 in the 0x20 arm.
 * Exhaustive subset search reaches five only by dropping q and src_base2
 * together; either alone is worse, while the five remaining pins are jointly
 * load-bearing. Residual: four jump-table address-hoist positions and GCC's
 * canonicalized operand order for one commutative addu.
 */
#define G_SDVALUE_IN_DATA
#define FUNC_80049F50_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "../game/sound.h"

void func_80046294(void)
{
    SDValue *p;
    register s32 i asm("a3");
    register s32 j asm("a2");
    register s32 k asm("t0");
    SDValue *q;
    s32 tag;

    p = g_SDValue;
    i = 0;
    if (p->command_count <= 0) {
        goto tail;
    }
    k = 0x30;
    j = i;

    do {
    if (p->command_count == 0) {
        goto tail;
    }
    tag = p->commands.b[j];
    switch (tag) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        p = g_SDValue;
    {
        register SDCommand *dst1 asm("v0");
        register u8 *src_base1 asm("v1");
        SDCommand *src1;

        dst1 = (SDCommand *)((u8 *)p + j);
        dst1 = (SDCommand *)((u8 *)dst1 + 0x80);
        src_base1 = (u8 *)p + k;
        src1 = (SDCommand *)(src_base1 + 0x80);
        *dst1 = *src1;
    }
        p = g_SDValue;
        p->command_count = (u16)p->command_count - 1;
        goto test;
    case 0x20:
        q = g_SDValue;
        if (*(s32 *)&q->commands.b[j + 0x10] != 0x20) {
            goto test;
        }
        p = q;
        p = g_SDValue;
    {
        register SDCommand *dst2 asm("a0");
        u8 *src_base2;
        SDCommand *src2;

        dst2 = (SDCommand *)((u8 *)p + j);
        dst2 = (SDCommand *)((u8 *)dst2 + 0x80);
        src_base2 = (u8 *)p + k;
        src2 = (SDCommand *)(src_base2 + 0x80);
        *dst2 = *src2;
    }
        p = g_SDValue;
        p->command_count = (u16)p->command_count - 1;
        goto test;
    default:
        k += 0x30;
        j += 0x30;
        i += 1;
        goto test;
    }

test:
    p = g_SDValue;
    } while (i < p->command_count && i >= 0);
    goto tail;

tail:
    switch (g_SDValue->field_007C) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        g_SDValue->field_007C = 0;
        g_SDValue->field_007D = 0;
        break;
    }

    p = g_SDValue;
    if ((p->flags_0040 & 0x80) == 0) {
        return;
    }
    if (p->field_157E == -1) {
        return;
    }
    if (func_80049F50() == 1) {
        func_80049C40(g_SDValue->field_157E);
    }
    g_SDValue->flags_0040 &= 0xFF7F;
}
