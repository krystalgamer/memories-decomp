#include "../../types.h"

extern u8 D_800EB0F8[];
extern void func_80035B7C(void *);
extern u8 *func_80035BE4(int, int, int, int, int, int);
extern void func_80039A14(void *);

u8 *Password_CreateMessageBox(int message_id, int flags)
{
    u8 *object;

    func_80035B7C(D_800EB0F8);
    object = func_80035BE4(0, message_id, 0x98, 0x98, 0xA0, 0x40);
    object[0x53] = 1;
    if (flags & 0xF) {
        func_80039A14(object);
    }
    if (flags & 0x80) {
        *(u16 *)(object + 0x34) |= 8;
    }
    return object;
}
