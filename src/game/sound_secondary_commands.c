#include "../types.h"

#include "sound.h"

extern int func_8004A0FC();
extern int func_8004A27C();
extern int func_8004A2F8();
extern int func_8004ACE4();

void func_8004B49C(s32 arg0, s32 arg1, u8 arg2)
{
    SDSecondaryState *b;
    SDSecondaryState *c;
    SDSecondaryRecord *e;
    u8 *q;
    u8 *p;
    u8 ok;
    s32 id;
    s32 id2;
    s32 sel;
    s32 v;
    s32 i;
    s32 k;
    s32 m;
    s32 off;

    ok = 0;
    id = arg0 & 0xFF;
    b = D_8009B458;
    e = (SDSecondaryRecord *)b + id;
    sel = arg1 & 0xFF;

    switch (sel) {
    case 6:
        v = e->field_0012;
        e->field_0013 = arg2;
        if (v != 0x14) {
            if (v != 0x1E) {
                func_8004ACE4(e, id);
            }
        }
        break;
    case 7:
        e->field_0003 = arg2;
        ok = 1;
        break;
    case 0xA:
        if (arg2 != 0) {
            e->field_0001 = arg2;
        } else {
            e->field_0001 = 1;
        }
        ok = 1;
        break;
    case 0xB:
        e->field_0005 = arg2;
        ok = 1;
        break;
    case 0x1E:
        b->field_0512 = arg2;
        func_8004A2F8();
        break;
    case 0x30:
        e->field_0006 =
            (e->field_0006 & 0xF) | ((arg2 & 0xF) << 4);
        break;
    case 0x5B:
        e->field_0010 = arg2 & 0x7F;
        break;
    case 0x62:
        e->field_0011 = arg2;
        break;
    case 0x63:
        e->field_0012 = arg2;
        break;

    case 0x7F:
        break;
    }

    if (ok != 0) {
        c = D_8009B458;
        i = 0;
        if (c->object_count > 0) {
            id2 = arg0 & 0xFF;
            off = id2 * 0x18;
            m = 0x180;
            k = i;
            do {
                /* The 0x18-byte record and 0x28-byte object views overlap. */
                q = (u8 *)c + k;
                if (id2 == q[0x183]) {
                    if (q[0x18D] != 0) {
                        func_8004A0FC((u8 *)c + m, (u8 *)c + off);
                        p = (u8 *)D_8009B458 + k;
                        func_8004A27C(i, *(u16 *)(p + 0x194),
                                      *(u16 *)(p + 0x196));
                    }
                }
                m += 0x28;
                k += 0x28;
                c = D_8009B458;
                i++;
            } while (i < c->object_count);
        }
    }
}

void func_8004B6E8(unsigned char index, int value)
{
    register SDSecondaryRecord *entries =
        (SDSecondaryRecord *)D_8009B458;

    entries += index;

    entries->field_0000 = value;
}

void func_8004B70C(unsigned char index, int unused, int value)
{
    register SDSecondaryRecord *entries =
        (SDSecondaryRecord *)D_8009B458;

    entries += index;

    entries->field_0007 = value & 0x7F;
}
