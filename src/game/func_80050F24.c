#include "../types.h"
#include "model.h"

extern void func_80057AF4(s32, s32, s32);
extern void func_80059F18(s32, s32, s32, s32);
extern void func_8005F180(s32);

void func_80050F24(s32 arg0)
{
    ModelSlot *p;
    ModelSlotCF8Block *q;
    s32 index;
    s32 value;

    p = &D_800F2C40[arg0];
    q = &D_800F2C40[arg0].field_CF8;
    index = p->field_DFE + 3;
    value = p->field_7C4[index].field_00;
    if (value != 0) {
        func_80057AF4(arg0, index, 1);
    }
    value = value * 2 / 3;
    if (value < 60) {
        value = 60;
    }
    if (q->field_0C[p->field_DFE] != 0) {
        value = q->field_0C[p->field_DFE];
    }
    func_80059F18(1, -1, arg0 ^ 1, value);
    func_8005F180(((q->field_0A[p->field_DFE] & 0x80) == 0) << 1);
    if ((q->field_0A[p->field_DFE] & 0x40) != 0) {
        func_8005F180(1);
    }
}
