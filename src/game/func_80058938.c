#include "../types.h"
#include "func_80058938.h"
#include "model.h"

void func_80058938(s32 a0, s32 a1, ModelTintColor a2, ModelTintColor a3,
                   s32 a4, s32 bits)
{
    ModelSlot *slot;
    register ModelTintRequest *r asm("$8");
    register ModelTintRequest *end asm("$3");
    u8 *base;
    register s32 off asm("$9");
    register s32 fill asm("$13");
    register s32 fifth asm("$14");
    register s32 source asm("$11");
    register s32 f asm("$2");
    s32 mode;
    s32 sel;

    slot = &D_800F2C40[a0];
    mode = (a0 & 1) << 1;
    sel = (a1 & 1) << 2;
    fill = 0xFF;
    off = 0;
    base = (u8 *)D_800F2B50;
    r = D_800F2B50;
    fifth = a4;
    source = bits;
    end = r + 10;
    do {
        if ((*(u8 *)r & 1) == 0) {
            f = r->flags;
            f = ((f | 1) & 0xFFFD) | mode;
            f = (f & 0xFFFB) | sel;
            mode = 0;
            r->flags = f;
            f &= 0xFF07;
            sel = off;
            f |= (slot->field_BF5 & 0x1F) << 3;
            r->flags = f;
            r->field_0A = slot->field_E06;
            r->elapsed = 0;
            f = fifth * 2;
            r->duration = f;
            r->start = a2;
            r->end = a3;
            for (; mode < 8; mode++) {
                base[mode + sel + 1] = source ? *(u8 *)(source + mode) : fill;
            }
            break;
        }
        off += 0x18;
        r++;
    } while ((s32)r < (s32)end);
}
