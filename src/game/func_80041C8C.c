#include "../types.h"

extern s32 (*D_80090FEC[])(u8 *, u8 *);

void func_80041C8C(u8 *ctx)
{
    u8 *p;
    s32 op;
    s32 (**table)(u8 *, u8 *);
    s32 value;

    p = (u8 *)(*(s32 *)(ctx + 0x50) + *(u16 *)(ctx + 0x58));
    op = *p;
    p++;

    if (op >= 0xF0) {
        table = D_80090FEC;
        do {
            if (table[op ^ 0xFF](ctx, p) == -1) {
                return;
            }
            p = (u8 *)(*(s32 *)(ctx + 0x50) + *(u16 *)(ctx + 0x58));
            op = *p;
            p++;
        } while (op >= 0xF0);
    }

    *(u16 *)(ctx + 0x5A) = op;
    value = (p[1] << 8) | p[0];
    *(s32 *)(ctx + 0x4C) = *(s32 *)(ctx + 0x54) + value;
    *(u16 *)(ctx + 0x58) = *(u16 *)(ctx + 0x58) + 3;
}
