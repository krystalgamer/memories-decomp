#include "../types.h"
#include "model.h"
#include "model_slot_state_updates.h"
#include "func_80057AF4.h"

extern ModelSlot D_800F2C40[];

extern void func_8005B620(u8 *, u8 *, s32);

void func_80057AF4(s32 index, s32 anim, s32 flag) {
    register ModelSlot *m asm("$17");
    register s32 off asm("$2");
    u8 **parts;
    u8 *dst;
    u8 *base;
    u16 *src;
    s32 i;
    s32 prev;
    s32 count;
    register s32 t asm("$2");

    off = index * 0xE20;
    {
        register u8 *tbl asm("$3") = (u8 *)D_800F2C40;

        m = (ModelSlot *)(tbl + off);
    }
    dst = m->field_DE0;
    base = m->field_DDC;
    parts = (u8 **)m->field_1E0;
    if (m->field_E1F == 0) {
        return;
    }
    switch (anim) {
    case -1:
        m->field_E10 = 0;
        for (i = 0; i < m->field_E1B; parts++, i++) {
            *(u16 *)(*parts + 8) = 0xFFFF;
        }
        break;
    case 0: {
        s32 entry;

        if (m->field_E0F == 0) {
            return;
        }
        for (i = 0; i < m->field_E1B; i++) {
            {
                register s32 o asm("$4");

                o = i * 2;
                o += m->field_E0F * 116;
                entry = *(u16 *)((u8 *)m + o + 712);
            }
            src = (u16 *)((u8 *)m->field_DD8 + entry * 4);
            if (entry != 0xFFFF) {
                t = i;
                if (i < 0) {
                    t = i + 7;
                }
                count = 3;
                t >>= count;
                if ((m->field_BEC[t] >> (i - (t << count))) & 1) {
                    count = 5;
                }
                func_8005B620(base + src[0] * 4, dst, count);
                dst += count * 4;
            }
        }
        m->field_E0F = 0;
        break;
    }
    default: {
        s32 entry;

        prev = m->field_E0F;
        if (anim != m->field_DFE + 3) {
            m->field_E0E = 2;
        }
        if (m->field_750[anim].max == 0) {
            return;
        }
        if (prev != 0) {
            if (flag == 0) {
                return;
            }
            m->field_BF6 = m->field_BF4;
            func_80057AF4(index, 0, 0);
        }
        if (anim == m->field_DFE + 3 || m->field_DF8 == 0x309) {
            m->field_E0E = 7;
        }
        m->field_E0F = anim;
        if (prev == anim) {
            func_800597C8(index, 0, 0);
            return;
        }
        m->field_E10 = 1;
        for (i = 0; i < m->field_E1B; parts++, i++) {
            {
                register s32 o asm("$4");

                o = i * 2;
                o += m->field_E0F * 116;
                entry = *(u16 *)((u8 *)m + o + 712);
            }
            src = (u16 *)((u8 *)m->field_DD8 + entry * 4);
            if (entry != 0xFFFF) {
                t = i;
                if (i < 0) {
                    t = i + 7;
                }
                count = 3;
                t >>= count;
                if ((m->field_BEC[t] >> (i - (t << count))) & 1) {
                    count = 5;
                }
                *(u16 *)(*parts + 8) = ((u8 *)src - (u8 *)m->field_DD8) >> 2;
                func_8005B620(dst, base + src[0] * 4, count);
                dst += count * 4;
            }
        }
        break;
    }
    }
}
