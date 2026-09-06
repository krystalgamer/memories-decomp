#include "../../types.h"

typedef struct {
    u16 f0;
    u8 pad2[4];
    u16 f6;
    u8 pad8;
    u8 f9;
    u8 f10;
    u8 pad11;
} Exit;

typedef struct {
    u16 f0;
    u8 pad2[14];
    u8 f16;
    u8 pad17;
    Exit exits[4];
} Location;

extern u8 D_8016A2BC;
extern Location D_80169E54[];
extern u16 D_800C4E68;
extern u16 D_800C4E74;
extern s32 D_8016A2B8;
extern s32 func_8004EB9C(s32);
extern void func_80065B24(s32);

s32 func_801697D0(void)
{
    s32 i;
    s32 r;
    Location *p;
    Exit *q;

    p = &D_80169E54[D_8016A2BC];
    q = p->exits;
    if (D_8016A2BC >= 10) {
        if (func_8004EB9C(71) != 0) {
            if ((D_800C4E68 & 0x20) != 0) {
                func_80065B24(48);
                D_8016A2B8 = 24;
                return 0;
            }
        }
    }
    if ((D_800C4E68 & 0xC0) != 0) {
        if ((r = p->f0) != 0) {
            if (func_8004EB9C(q->f0) != 0) {
                r = 0;
            }
        }
        if (r == 0) {
            if (p->f16 != 0) {
                D_8016A2B8 = 24;
                func_80065B24(48);
                return p->f16;
            }
            func_80065B24(48);
            return D_8016A2BC | 0x8000;
        }
    }
    i = 0;
    while (i < 4) {
        if (q->f9 != 16) {
            if (q->f0 != 0 && func_8004EB9C(q->f0) == 0) {
                goto next;
            }
            if ((D_800C4E74 & q->f6) != 0) {
                D_8016A2B8 = q->f10;
                func_80065B24(6);
                return q->f9;
            }
        }
    next:
        q++;
        i++;
    }
    return -1;
}
