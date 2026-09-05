#include "../types.h"

#include "model.h"

extern u8 D_800F56F0[];
extern u8 D_8009AF94;

extern void GsSetRefView2(void *);
extern void func_8004EB00(void);
extern void func_8004FE2C(void);
extern void func_800507D0(void);
extern void func_800540B4(s32);
extern void func_800559D4(s32);
extern void func_8004DE24(void);
extern void func_8004E9A0(void);
extern void func_800528AC(void);
extern void func_800556E8(s32);
extern void func_8005DBA4(void);
extern void func_80051A48(void);
extern s32 func_8005FB08(void);
extern s32 func_8005FC1C(s32);
extern void func_8005A53C();

void func_80059CE4(void)
{
    u8 *state = D_800F56F0;

    GsSetRefView2(state);
    if (D_8009AF94 == 15) {
        func_8004EB00();
    }
    if (D_8009AF94 == 19) {
        func_8004FE2C();
    }
    if (D_8009AF94 == 20) {
        func_800507D0();
    }
    GsSetRefView2(state);
    func_800540B4(0);
    func_800540B4(1);
    func_800540B4(2);
    func_800559D4(0);
    func_800559D4(1);
    func_8004DE24();
    func_8004E9A0();
    func_800528AC();
    func_800556E8(0);
    func_800556E8(1);
    func_800556E8(2);
    if (!func_8005FB08()) {
        func_8005DBA4();
    }
    func_80051A48();
}

void func_80059DD8(s32 index)
{
    ModelSlot *base = &D_800F2C40[index];
    ModelSlotHeadEntry *slot = base->field_000;
    s32 (*callback)(s32) = func_8005FC1C;
    u8 *node;
    u8 *next;
    s32 i;

    if (base->field_E1F == 0) {
        return;
    }
    for (i = 0; i < base->field_E1A; i++, slot++) {
        node = slot->field_04;
        if (node != (u8 *)0) {
            while (1) {
                if (*(s32 *)(node + 8) != 0) {
                    func_8005A53C(
                        callback,
                        node + 12,
                        *(s32 *)(node + 4),
                        *(s32 *)(node + 8)
                    );
                }
                next = *(u8 **)node;
                if (next == (u8 *)-1) {
                    break;
                }
                node = next;
            }
        }
    }
}
