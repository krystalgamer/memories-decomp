#include "../types.h"

extern s32 D_8009B0D8;
extern volatile s32 D_8009B0D8_volatile asm("D_8009B0D8");

extern s32 func_80039AAC(u8 *);
extern void func_80039AD4(u8 *);

void func_80039AFC(u8 *record)
{
    if (func_80039AAC(record) == 0) {
        record[21] = 2;
        *(s32 *)(record + 4) = 0;
        record[20] = 32;
    }
    record[20] = record[20] - D_8009B0D8 * 2;
    if ((record[19] & 64) == 0) {
        record[4] = record[4] + D_8009B0D8 * 16;
        if ((s8)record[4] < 0) {
            record[4] = 128;
            record[20] = 16;
            record[19] |= 64;
        }
        record[6] = record[4];
    } else {
        record[5] = record[5] + D_8009B0D8 * 16;
        if ((s8)record[5] < 0) {
            record[20] = 0;
            record[21] = 0;
            record[19] = 0;
        }
        record[7] = record[5];
    }
}

void func_80039BE0(u8 *p)
{
    s32 v;

    if (!func_80039AAC(p)) {
        p[0x15] = 2;
        *(u32 *)(p + 4) = 0x80808080;
        p[0x14] = 0;
    }
    if (!(p[0x13] & 0x40)) {
        v = p[4] - (D_8009B0D8_volatile << 4);
        if (v <= 0) {
            p[0x13] |= 0x40;
            v = 0;
        }
        p[4] = v;
        p[5] = v;
    } else {
        v = p[6] - (D_8009B0D8_volatile << 4);
        if (v <= 0) {
            func_80039AD4(p);
            v = 0;
        }
        p[6] = v;
        p[7] = v;
    }
}

void func_80039C94(u8 *arg0) {
    if (func_80039AAC(arg0) == 0) {
        s32 a;
        s32 b;

        arg0[0x15] = 1;
        a = *(u16 *)(arg0 + 0xC);
        b = *(u16 *)(arg0 + 0xE);
        arg0[8] = 0;
        arg0[9] = 0;
        arg0[0xA] = 0;
        arg0[4] = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(arg0[0x13] & 0x40)) {
        s32 v = arg0[4] - 1;

        arg0[4] = v;

        if ((u8)v == 0) {
            arg0[0x13] |= 0x40;
        }
    } else {
        s32 v = arg0[8] + 4;

        arg0[0xA] = v;
        arg0[9] = v;
        arg0[8] = v;

        if (v >= 0x40) {
            func_80039AD4(arg0);
        }
    }
}
