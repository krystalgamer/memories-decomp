#include "../types.h"
#include "duel_card_layout.h"

extern u8 D_8009B1D5;
extern u8 D_801A7AD8[];

s32 func_80042B98(u8 *arg0);
void func_80017E3C(u8 *arg0);
void Duel_ApplyCardObjectFlags(u8 *arg0);

/* The mirror of func_800229F4: the card turn-back animation step. Picks
 * the mode (+0x2E) from the card record's flags when func_80042B98 says
 * the object is idle, then runs one of three mode arms -- a countdown that
 * sets the record's 0x400 flag (mode 0), and two rotate-then-settle arms
 * (modes 1 and 2) stepping the angle at +0x2A by +0x28 until the +0x60
 * counter runs out -- and resets through func_80017E3C. The arms are laid
 * out as goto targets in retail's own address order and the shared reset
 * block is reached by goto from all three. */
void func_80022674(u8 *p) {
    u8 *e;
    s32 m;
    s32 v;
    s16 c;
    s32 f;

    e = D_801A7AD8 + p[0x6A] * DUEL_CARD_RECORD_SIZE;
    if (func_80042B98(p) == 0) {
        m = *(u16 *)(e + 0x16);
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        *(s16 *)(p + 0x2E) = v;
    }
    v = *(s16 *)(p + 0x2E);
    if (v == 1) {
        goto m1;
    }
    if (v >= 2) {
        goto ge2;
    }
    if (v == 0) {
        goto m0;
    }
    return;
ge2:
    if (v == 2) {
        goto m2;
    }
    return;
m0:
    f = p[0x6C];
    if ((f & 0x40) == 0) {
        p[0x6C] = f | 0x40;
        *(u16 *)(p + 0x60) = 8;
        *(u16 *)(e + 0x16) = *(u16 *)(e + 0x16) | 0x400;
        *(s32 *)(p + 0x20) = 0x8000;
        *(u16 *)(p + 8) = *(u16 *)(p + 8) | 4;
        if ((*(u16 *)(e + 0x16) & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
            p[0x22] = 0xC0;
        }
    }
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c == 0) {
        return;
    }
reset:
    p[0x6C] = 0;
    *(s32 *)(p + 0x24) = 0;
    func_80017E3C(p);
    return;
m1:
    f = p[0x6C];
    if ((f & 0x40) == 0) {
        p[0x6C] = f | 0x40;
        *(u16 *)(p + 0x28) = 0x1000;
        *(u16 *)(p + 0x2A) = 0x80;
        *(s32 *)(p + 0x20) = 0;
        *(u16 *)(p + 0x60) = 4;
    }
    if ((p[0x6C] & 0x20) != 0) {
        goto alt1;
    }
    if (D_8009B1D5 != 0) {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) - *(u16 *)(p + 0x28);
    } else {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) + *(u16 *)(p + 0x28);
    }
    p[0x22] = *(u16 *)(p + 0x2A) >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x6C] = p[0x6C] | 0x20;
    *(u16 *)(e + 0x16) = *(u16 *)(e + 0x16) | 0x400;
    *(s32 *)(p + 0x20) = 0x4000;
    *(s16 *)(p + 0x2A) = 0x4080;
    *(u16 *)(p + 0x60) = 4;
    *(u16 *)(p + 8) = *(u16 *)(p + 8) | 4;
    return;
alt1:
    *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) - *(u16 *)(p + 0x28);
    p[0x21] = *(u16 *)(p + 0x2A) >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x21] = 0;
    goto reset;
m2:
    f = p[0x6C];
    if ((f & 0x40) == 0) {
        p[0x6C] = f | 0x40;
        *(s32 *)(p + 0x20) = 0xC000;
        *(u16 *)(p + 0x28) = 0x1000;
        *(u16 *)(p + 0x2A) = 0x80;
        *(u16 *)(p + 0x60) = 4;
    }
    if ((p[0x6C] & 0x20) != 0) {
        goto alt2;
    }
    if (D_8009B1D5 != 0) {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) - *(u16 *)(p + 0x28);
    } else {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) + *(u16 *)(p + 0x28);
    }
    p[0x22] = *(u16 *)(p + 0x2A) >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x6C] = p[0x6C] | 0x20;
    *(u16 *)(e + 0x16) = *(u16 *)(e + 0x16) | 0x400;
    *(s16 *)(p + 0x2A) = -0x3F80;
    *(s32 *)(p + 0x20) = 0xC000C0;
    *(u16 *)(p + 0x60) = 4;
    *(u16 *)(p + 8) = *(u16 *)(p + 8) | 4;
    return;
alt2:
    *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) + *(u16 *)(p + 0x28);
    p[0x20] = *(u16 *)(p + 0x2A) >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x20] = 0;
    goto reset;
}

/* Card flip/turn animation step on a duel card object: picks the animation
 * mode (+0x2E) from the card's record flags when func_80042B98 says the
 * object is idle, then runs one of three mode arms -- a plain countdown
 * (mode 0), and two rotate-then-settle arms (modes 1 and 2) that step the
 * angle at +0x2A by +0x28 until the +0x60 counter runs out, mirror it
 * according to D_8009B1D5, and reset through Duel_ApplyCardObjectFlags.
 * The arms are laid out as goto targets in retail's own address order and
 * the shared reset block is reached by goto from both. */
void func_800229F4(u8 *p) {
    u8 *e;
    s32 m;
    s32 v;
    u32 s1;
    u32 s2;
    s32 d;
    s16 c;
    s32 f;

    e = D_801A7AD8 + p[0x6A] * DUEL_CARD_RECORD_SIZE;
    if (func_80042B98(p) == 0) {
        m = *(u16 *)(e + 0x16);
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        *(s16 *)(p + 0x2E) = v;
    }
    v = *(s16 *)(p + 0x2E);
    if (v == 1) {
        goto m1;
    }
    if (v >= 2) {
        goto ge2;
    }
    if (v == 0) {
        goto m0;
    }
    return;
ge2:
    if (v == 2) {
        goto m2;
    }
    return;
m0:
    f = p[0x6C];
    if ((f & 0x40) == 0) {
        p[0x6C] = f | 0x40;
        *(u16 *)(p + 0x60) = 8;
    }
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c > 0) {
        return;
    }
    *(u16 *)(e + 0x16) = *(u16 *)(e + 0x16) & 0xFBFF;
    p[0x22] = 0x80;
    p[0x20] = 0;
    p[0x21] = 0;
    *(s32 *)(p + 0x20) = 0x800000;
    if ((*(u16 *)(e + 0x16) & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
        p[0x21] = 0xC0;
    }
reset:
    p[0x6C] = 0;
    *(s32 *)(p + 0x24) = 0;
    Duel_ApplyCardObjectFlags(p);
    return;
m1:
    f = p[0x6C];
    if ((f & 0x40) == 0) {
        p[0x6C] = f | 0x40;
        *(u16 *)(p + 0x28) = 0x1000;
        *(u16 *)(p + 0x2A) = 0x80;
        *(s32 *)(p + 0x20) = 0;
        *(u16 *)(p + 0x60) = 4;
        *(u16 *)(p + 8) = *(u16 *)(p + 8) | 4;
    }
    if ((p[0x6C] & 0x20) != 0) {
        goto alt1;
    }
    s1 = *(u16 *)(p + 0x2A) + *(u16 *)(p + 0x28);
    *(u16 *)(p + 0x2A) = s1;
    p[0x21] = s1 >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x6C] = p[0x6C] | 0x20;
    d = D_8009B1D5;
    *(u16 *)(e + 0x16) = *(u16 *)(e + 0x16) & 0xFBFF;
    if (d != 0) {
        *(s32 *)(p + 0x20) = 0xC00000;
        *(s16 *)(p + 0x2A) = -0x3F80;
    } else {
        *(s32 *)(p + 0x20) = 0x400000;
        *(s16 *)(p + 0x2A) = 0x4080;
    }
    *(u16 *)(p + 0x60) = 4;
    return;
alt1:
    if (D_8009B1D5 != 0) {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) + *(u16 *)(p + 0x28);
    } else {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) - *(u16 *)(p + 0x28);
    }
    p[0x22] = *(u16 *)(p + 0x2A) >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x22] = 0;
    goto reset;
m2:
    f = p[0x6C];
    if ((f & 0x40) == 0) {
        p[0x6C] = f | 0x40;
        *(s32 *)(p + 0x20) = 0xC00000;
        *(u16 *)(p + 0x28) = 0x1000;
        *(u16 *)(p + 0x2A) = 0x80;
        *(u16 *)(p + 0x60) = 4;
    }
    if ((p[0x6C] & 0x20) != 0) {
        goto alt2;
    }
    s2 = *(u16 *)(p + 0x2A) - *(u16 *)(p + 0x28);
    *(u16 *)(p + 0x2A) = s2;
    p[0x20] = s2 >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x6C] = p[0x6C] | 0x20;
    d = D_8009B1D5;
    *(u16 *)(e + 0x16) = *(u16 *)(e + 0x16) & 0xFBFF;
    if (d != 0) {
        *(s32 *)(p + 0x20) = 0xC0C000;
        *(s16 *)(p + 0x2A) = -0x3F80;
    } else {
        *(s32 *)(p + 0x20) = 0x40C000;
        *(s16 *)(p + 0x2A) = 0x4080;
    }
    *(u16 *)(p + 0x60) = 4;
    return;
alt2:
    if (D_8009B1D5 != 0) {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) + *(u16 *)(p + 0x28);
    } else {
        *(u16 *)(p + 0x2A) = *(u16 *)(p + 0x2A) - *(u16 *)(p + 0x28);
    }
    p[0x22] = *(u16 *)(p + 0x2A) >> 8;
    c = *(u16 *)(p + 0x60) - 1;
    *(u16 *)(p + 0x60) = c;
    if (c != 0) {
        return;
    }
    p[0x20] = 0;
    goto reset;
}
