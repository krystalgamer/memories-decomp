#include "../types.h"
#include "duel_side_state.h"
#include "display_object_layout.h"
#include "duel_card_layout.h"
#include "duel_card.h"
#include "duel_card_turn_animations.h"
#include "duel_card_display_state.h"
#include "duel_apply_card_object_flags.h"
#include "display_object_lifecycle.h"

/* The mirror of func_800229F4: the card turn-back animation step. Picks
 * the mode (+0x2E) from the card record's flags when func_80042B98 says
 * the object is idle, then runs one of three mode arms -- a countdown that
 * sets the record's 0x400 flag (mode 0), and two rotate-then-settle arms
 * (modes 1 and 2) stepping the angle at +0x2A by +0x28 until the +0x60
 * counter runs out -- and resets through func_80017E3C. The arms are laid
 * out as goto targets in retail's own address order and the shared reset
 * block is reached by goto from all three. */
void func_80022674(DuelCardTurnObject *p) {
    DuelCardRecord *e;
    s32 m;
    s32 v;
    s16 c;
    s32 f;

    e = &D_801A7AD8[p->record_index];
    if (func_80042B98((DisplayObjectLifecycle *)p) == 0) {
        m = e->flags;
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        p->mode = v;
    }
    v = p->mode;
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
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->counter = 8;
        e->flags = e->flags | 0x400;
        p->tint.packed = 0x8000;
        p->flags =
            p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if ((e->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
            p->tint.channel[2] = 0xC0;
        }
    }
    c = p->counter - 1;
    p->counter = c;
    if (c == 0) {
        return;
    }
reset:
    p->phase = 0;
    p->field_24 = 0;
    func_80017E3C((DuelCardDisplayObject *)p);
    return;
m1:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->tint.packed = 0;
        p->counter = 4;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt1;
    }
    if (D_8009B1D5 != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    e->flags = e->flags | 0x400;
    p->tint.packed = 0x4000;
    p->angle.signed_value = 0x4080;
    p->counter = 4;
    p->flags = p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
    return;
alt1:
    p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    p->tint.channel[1] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[1] = 0;
    goto reset;
m2:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->tint.packed = 0xC000;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->counter = 4;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt2;
    }
    if (D_8009B1D5 != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    e->flags = e->flags | 0x400;
    p->angle.signed_value = -0x3F80;
    p->tint.packed = 0xC000C0;
    p->counter = 4;
    p->flags = p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
    return;
alt2:
    p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    p->tint.channel[0] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[0] = 0;
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
void func_800229F4(DuelCardTurnObject *p) {
    DuelCardRecord *e;
    s32 m;
    s32 v;
    u32 s1;
    u32 s2;
    s32 d;
    s16 c;
    s32 f;

    e = &D_801A7AD8[p->record_index];
    if (func_80042B98((DisplayObjectLifecycle *)p) == 0) {
        m = e->flags;
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        p->mode = v;
    }
    v = p->mode;
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
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->counter = 8;
    }
    c = p->counter - 1;
    p->counter = c;
    if (c > 0) {
        return;
    }
    e->flags = e->flags & 0xFBFF;
    p->tint.channel[2] = 0x80;
    p->tint.channel[0] = 0;
    p->tint.channel[1] = 0;
    p->tint.packed = 0x800000;
    if ((e->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
        p->tint.channel[1] = 0xC0;
    }
reset:
    p->phase = 0;
    p->field_24 = 0;
    Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)p);
    return;
m1:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->tint.packed = 0;
        p->counter = 4;
        p->flags =
            p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt1;
    }
    s1 = p->angle.unsigned_value + p->angle_step;
    p->angle.unsigned_value = s1;
    p->tint.channel[1] = s1 >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    d = D_8009B1D5;
    e->flags = e->flags & 0xFBFF;
    if (d != 0) {
        p->tint.packed = 0xC00000;
        p->angle.signed_value = -0x3F80;
    } else {
        p->tint.packed = 0x400000;
        p->angle.signed_value = 0x4080;
    }
    p->counter = 4;
    return;
alt1:
    if (D_8009B1D5 != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[2] = 0;
    goto reset;
m2:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->tint.packed = 0xC00000;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->counter = 4;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt2;
    }
    s2 = p->angle.unsigned_value - p->angle_step;
    p->angle.unsigned_value = s2;
    p->tint.channel[0] = s2 >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    d = D_8009B1D5;
    e->flags = e->flags & 0xFBFF;
    if (d != 0) {
        p->tint.packed = 0xC0C000;
        p->angle.signed_value = -0x3F80;
    } else {
        p->tint.packed = 0x40C000;
        p->angle.signed_value = 0x4080;
    }
    p->counter = 4;
    return;
alt2:
    if (D_8009B1D5 != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[0] = 0;
    goto reset;
}
