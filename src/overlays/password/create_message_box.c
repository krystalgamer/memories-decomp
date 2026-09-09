#include "../../types.h"
#include "../../game/text_box_runtime.h"
#include "shop.h"

extern void func_80035B7C(DuelEffectChannel *);
extern void *func_80035BE4(s32, s32, s32, s32, s32, s32);

DuelEffectChannel *Password_CreateMessageBox(int message_id, int flags)
{
    DuelEffectChannel *object;

    func_80035B7C(D_800EB0F8);
    object = func_80035BE4(0, message_id, 0x98, 0x98, 0xA0, 0x40);
    object->field_53 = 1;
    if (flags & 0xF) {
        func_80039A14((u8 *)object);
    }
    if (flags & 0x80) {
        object->flags_34 |= 8;
    }
    return object;
}
