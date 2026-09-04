#include "../types.h"

extern void func_800373C8(u8 *, s32, s32);
extern void *func_8004006C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800404CC(void *, s32, s32, s32, s32, s32, s32, s32);
extern void func_800428EC(void *, s32);
extern void func_80042918(void *);

void func_800374A8(u8 *object)
{
    u8 flags = object[0x51];

    if ((flags & 0x80) == 0) {
        object[0x51] = flags | 0x80;
        func_800373C8(object, 3, 0);
        object[0x51] = 0x82;
    }
}

u8 *Dialog_OpenChoice(u8 *record)
{
    u8 *cursor = func_800400AC(func_8004006C(), 2);

    func_800404CC(
        cursor,
        *(s16 *)(record + 0x3C) + *(s16 *)(record + 0x3E) - 0x10,
        *(s16 *)(record + 0x40) + *(s16 *)(record + 0x42) - 0x10,
        3,
        0,
        0,
        11,
        0x20C
    );
    *(u16 *)(cursor + 8) |= 0x28;
    func_80042918(cursor);
    func_800428EC(cursor, (s8)(record[0x59] + 1));
    return cursor;
}
