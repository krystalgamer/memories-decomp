#include "../types.h"
#include "func_8005106C.h"
#include "model_effect_requests.h"
#include "model_transfer_flags.h"
#include "sound_output_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/stdio.h"
#include "model.h"
#include "func_80057AF4.h"

void func_80050F24(s32 arg0)
{
    ModelSlot *p;
    ModelSlotCF8Block *q;
    s32 index;
    s32 value;

    p = &D_800F2C40[arg0];
    q = &D_800F2C40[arg0].field_CF8;
    index = p->field_DFE + 3;
    value = p->field_750[index].max;
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

/* "e%03ld(%ld)=%d\n" */
extern u8 D_80011508[];

extern s32 func_80058E1C(void);
extern void func_8005A6A8(s32, s32, u8 *, u8 *);
extern void SD_SEPlay(s32, s32, s32);

void func_8005106C(s32 index) {
    u8 buf[0x50];
    u8 out[8];
    ModelSlot *m;
    ModelSlotSoundEntry *e;
    s32 anim;
    s32 length;
    s32 speed;
    s32 speed2;
    s32 id;
    s32 phase;
    s32 tag;
    s32 show;
    s32 i;
    s32 value;
    s32 cur;
    s32 code;
    u16 flag;

    m = &D_800F2C40[index];
    if (m->field_E1F == 0) {
        return;
    }
    if (((u8 *)m)[0xDC3] >= 2) {
        if ((*(u32 *)((u8 *)m + 0xDC0) & 0xFFFFFF) == 0) {
            return;
        }
    }
    if (((u8 *)m)[0xE1E] == 0) {
        return;
    }
    anim = m->field_BF5;
    length = m->field_750[anim].max << 4;
    speed = m->field_E0D * func_80058E1C();
    e = m->sound_entries;
    tag = (index << 8) + 0x4000;
    if (length != 0) {
        phase = m->field_E06 % length;
    } else {
        phase = 0;
    }
    cur = m->field_DFE;
    value = *(s32 *)((s32)m + (cur << 2) + 0xD08);
    show = 0;
    if (value >= 0) {
        if (((u8 *)m)[0xE0E] != 8) {
            if (((u8 *)m)[0xE0E] != 2 || cur + 3 == anim) {
                show = 1;
            }
        }
        if (show) {
            s32 cur2;
            s32 value2;

            speed2 = m->field_E0D * func_80058E1C();
            cur2 = m->field_DFE;
            phase = *(u16 *)((u8 *)m + 0xE08);
            *(u16 *)((u8 *)m + 0xE08) = phase + speed2;
            value2 = *(s32 *)((s32)m + (cur2 << 2) + 0xD08);
            anim = m->field_DFE + 3;
            sprintf(buf, D_80011508, value2 / 1000, value2 % 1000, *(volatile u16 *)((u8 *)m + 0xE08) >> 4);
            FntPrint(buf);
        }
    }
    if (show == 0) {
        if (m->field_E16 == 0x23) {
            return;
        }
    }
    func_8005A6A8(*(s16 *)&m->field_DD0[0], *(s16 *)&m->field_DD0[2], &out[0], &out[1]);
    i = 0;
loop:
    flag = e->flags & 0x8000;
    id = e->id;
    if (flag) {
        code = flag | id;
    } else {
        code = (u16)tag | id;
    }
    if (e->frame == 0) {
        return;
    }
    if (e->frame == anim) {
        if (flag != 0 && phase == 0) {
            func_80045334(code);
        }
        if ((e->flags & 0x7FFF) >= phase && (e->flags & 0x7FFF) < phase + speed) {
            SD_SEPlay(code, 0xFF, 0);
        }
    }
    i++;
    e++;
    if (i < 0x40) {
        goto loop;
    }
}
