#include "../types.h"
#include "func_8005106C.h"
#include "model_effect_requests.h"
#include "model_transfer_flags.h"
#include "sound_output_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/stdio.h"
#include "model.h"
#include "model_control.h"
#include "func_80058E1C.h"
#include "model_control_slot_animation.h"
#include "sound.h"
#include "sd_calc_spatial_volume_pan.h"

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
        Model_ControlSlotAnimation(arg0, index, 1);
    }
    value = value * 2 / 3;
    if (value < 60) {
        value = 60;
    }
    if (q->field_0C[p->field_DFE] != 0) {
        value = q->field_0C[p->field_DFE];
    }
    func_80059F18(1, -1, arg0 ^ 1, value);
    func_8005F180(
        ((q->prefix.bytes.field_0A[p->field_DFE] & 0x80) == 0) << 1
    );
    if ((q->prefix.bytes.field_0A[p->field_DFE] & 0x40) != 0) {
        func_8005F180(1);
    }
}

/* "e%03ld(%ld)=%d\n" */
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
    if (m->field_DC0[3] >= 2) {
        if ((*(u32 *)m->field_DC0 & 0xFFFFFF) == 0) {
            return;
        }
    }
    if (m->field_E1E == 0) {
        return;
    }
    anim = m->field_BF5;
    length = m->field_750[anim].max << 4;
    speed = m->field_E0D * Model_GetFrameStep();
    e = m->sound_entries;
    tag = (index << 8) + 0x4000;
    if (length != 0) {
        phase = m->field_E06 % length;
    } else {
        phase = 0;
    }
    cur = m->field_DFE;
    value = ((ModelControlCommandView *)m)->commands[cur];
    show = 0;
    if (value >= 0) {
        if (m->field_E0E != 8) {
            if (m->field_E0E != 2 || cur + 3 == anim) {
                show = 1;
            }
        }
        if (show) {
            s32 cur2;
            s32 value2;

            speed2 = m->field_E0D * Model_GetFrameStep();
            cur2 = m->field_DFE;
            phase = m->field_E08;
            m->field_E08 = phase + speed2;
            value2 = ((ModelControlCommandView *)m)->commands[cur2];
            anim = m->field_DFE + 3;
            /* Retail loads field_E08 again after the store above; read
               plainly, GCC reuses the value it just stored instead. */
            sprintf(buf, D_80011508, value2 / 1000, value2 % 1000, *(volatile u16 *)&m->field_E08 >> 4);
            FntPrint(buf);
        }
    }
    if (show == 0) {
        if (m->field_E16 == 0x23) {
            return;
        }
    }
    SD_CalcSpatialVolumePan(
        m->field_DD0[0], m->field_DD0[2], &out[0], &out[1]
    );
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
