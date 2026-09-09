#include "../types.h"
#include "func_8004DC38.h"
#include "model.h"
#include "model_slot_state_updates.h"

extern s32 func_80058E1C(void);
extern void func_8005106C(s32);
extern void func_80057AF4(s32, s32, s32);
extern void func_8005F714(s32, s32, s32);

/* Per-frame animation tick for a model slot. When the slot is active and
   has an animation loaded it takes the current animation's length and this
   tick's frame advance, lets the slot-6 idle/loop cases hand off through
   func_80059700, runs the sound trigger for the two duelist slots, advances
   or rewinds the frame counter (state 0x3E plays forward and clamps, other
   states play backward and wrap), and when the animation has run out with
   no hold pending it switches to the queued animation through func_800597C8
   and its follow-ups. Finally, unless a slow 0x23-state animation is on an
   off tick, it updates every attached part through func_8004DC38 at the
   new frame. */
void func_800556E8(s32 index) {
    ModelSlot *m;
    s32 anim;
    s32 length;
    s32 speed;
    s32 i;
    s32 frame;
    s32 lim;

    m = &D_800F2C40[index];
    if (m->field_E1F == 0) {
        return;
    }
    if (*(u32 *)((u8 *)m + 0xDD8) != 0) {
        anim = m->field_BF5;
        length = m->field_7C4[anim].field_00 << 4;
        speed = m->field_E0D * func_80058E1C();
        if (anim == 6) {
            lim = length - speed;
            if ((m->field_E16 == 0x3E && m->field_E06 >= lim) ||
                (m->field_E16 == 0x3C && m->field_E06 < 2)) {
                func_80059700(index, 0);
            }
        }
        if (index < 2) {
            func_8005106C(index);
        }
        if (m->field_E16 != 0x23) {
            if (m->field_E16 == 0x3E) {
                s32 v;

                if (m->field_E06 >= length) {
                    m->field_E06 = 0;
                }
                v = m->field_E06 + speed;
                if (v >= length) {
                    m->field_E06 = length;
                } else {
                    m->field_E06 = v;
                }
            } else {
                s32 w;

                if (m->field_E06 == 0) {
                    m->field_E06 = length;
                }
                w = m->field_E06 - speed;
                if (w <= 0) {
                    m->field_E06 = 0;
                } else {
                    m->field_E06 = w;
                }
            }
        }
        if (((u8 *)m)[0xE10] == 0) {
            if (m->field_E16 == 0x3E ? m->field_E06 >= length : m->field_E06 == 0) {
                if (m->field_DC0[7] == 6) {
                    ((u8 *)m)[0xBF4] = 2;
                }
                if (((u8 *)m)[0xBF4] != ((u8 *)m)[0xBF6]) {
                    func_800597C8(index, ((u8 *)m)[0xBF4], 0);
                    ((u8 *)m)[0xBF6] = ((u8 *)m)[0xBF4];
                    if (((u8 *)m)[0xE0F] != 0) {
                        func_80057AF4(index, 0, 0);
                    }
                    if (m->field_DC0[7] == 6) {
                        func_8005F714(-1, index + 2, *(u16 *)((u8 *)m + 0x8B0));
                        m->field_DC0[7] = 0;
                        ((u8 *)m)[0xBF4] = 1;
                    }
                }
            }
        }
    }
    if (m->field_E16 != 0x23) {
        if (m->field_E0D == 0x10) {
            if (func_80058E1C() == 2) {
                return;
            }
        }
    }
    frame = m->field_E06;
    if (m->field_E1B != 0) {
        i = 0;
        do {
            func_8004DC38((u8 *)m, i, m->field_BF5, frame);
            i++;
        } while (i < m->field_E1B);
    }
}
