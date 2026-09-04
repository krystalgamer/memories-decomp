#include "../types.h"
#include "model.h"
extern void func_800871D0(void);
extern void func_8008A4A0(u8 *, u8 *);
extern void func_800855D0(u8 *);
extern void func_80087970(u8 *, s32, s32 *);
extern void func_80087274(void);

s32 func_800593D0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 sp10[32];
    s32 sp30[2];
    u8 *p;
    u8 *e;
    u8 *q;
    u8 *base;

    p = (u8 *)&D_800F2C40[arg0];
    e = p + (arg1 + 1) * 8;
    q = *(u8 **)(e + 4);
    q = *(u8 **)(q + 4);
    base = *(u8 **)(q + 8);

    func_800871D0();
    func_8008A4A0(
        *(u8 **)(p + 0xD14) + arg1 * MODEL_SLOT_DATA_ENTRY_SIZE,
        sp10
    );
    func_800855D0(sp10);
    func_80087970(base + arg2 * 8, arg3, sp30);
    func_80087274();
    return sp30[0];
}
