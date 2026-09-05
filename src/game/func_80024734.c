#include "../types.h"

extern u8 D_8017808C[];
extern u8 D_80178130[];
extern u8 D_8015C424[];
extern u32 D_8009B0F4;

extern void Util_CopyWords(void *, void *, s32);
extern void func_8008E400(void *, s32, s32, void *);
extern s32 Util_CompareS16(s16 *, s16 *);
extern s32 func_800245EC(s32, s32);
extern u8 *func_80014EEC(s32, u8 *, s32, s32, void *, s32, s32);

void func_80024734(void)
{
    u8 *source = D_8017808C;
    u8 *output;
    u8 *result;
    s32 i;
    s32 previous;
    s32 count;
    u8 *table;
    s32 value;

    Util_CopyWords(source, source - 0xA4, 0xA0);
    func_8008E400(source, 0x50, 2, Util_CompareS16);

    output = source + 0xA4;
    previous = 0;
    for (i = 0; i < 0x50; i++) {
        value = *(u16 *)source;
        if (value != previous) {
            *(u16 *)output = value;
            previous = value;
            output += 2;
        }
        source += 2;
    }

    *(u16 *)output = 0xFFFF;
    table = D_8015C424;
    count = *(u16 *)(table + 0x1BD0C);
    result = func_80014EEC(
        0, (u8 *)0, count - 1, previous - count + 1,
        func_800245EC, 0, 0
    );
    *(s32 *)(result + 0x38) = (s32)D_80178130;
    *(s32 *)(result + 0x3C) = (s32)table;
    D_8009B0F4 = *(s32 *)(result + 0x2C) | 0x10;
}
