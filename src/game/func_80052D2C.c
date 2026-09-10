#include "../types.h"
#include "model_view_adjustments.h"
#include "model_has_insufficient_buffer_space.h"
#include "func_8004E7B0.h"
#include "func_80059AA8.h"
#include "model_effect_coefficients.h"
#include "func_8004DC38.h"
#include "func_80058E1C.h"
#include "func_800528AC.h"
#include "func_80052D2C.h"
#include "model_record_tables.h"
#include "model_scene_setup.h"
#include "model_transfer_flags.h"
#include "model_state_setters.h"
#include "model_slot_setup.h"
#include "model_update_view_metrics.h"
#include "model_graphics_state.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "func_80058434.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/memory.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h; camera_view.h above
   supplies libgs.h. */
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_scene_states.h"
#include "model_transfer_state.h"
#include "../unmatched.h"

extern ModelBytes8 D_8009B478_p asm("D_8009B478");
extern ModelBytes8 D_8009B480;
extern void func_800857C0(int);

/* The end of the contiguous model-scene runtime: camera-move setup and scene
   reset/configuration. The four functions share D_800F56F0, D_800F2B20,
   D_800F2B50, model slots, and the D_8009AFxx scene-control state. The
   camera/view correction passes that start the runtime are in
   model_scene_setup.c, and the tint request pass between them and this run,
   func_800528AC, is now a candidate in src/candidates/func_800528AC.c. */

/* Starts a camera move from the live view to model-slot or explicit targets.
 * A zero duration writes the selected endpoints back immediately. */
void func_80052D2C(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    SVECTOR a;
    SVECTOR b;
    SVECTOR t;
    s32 index;
    s32 state;
    s32 flags;

    memset(&b, 0, 8);
    b.vx = *(u16 *)&D_800F56F0.vpx;
    b.vy = *(u16 *)&D_800F56F0.vpy;
    b.vz = *(u16 *)&D_800F56F0.vpz;
    a = b;
    memset(&t, 0, 8);
    t.vx = *(u16 *)&D_800F56F0.vrx;
    t.vy = *(u16 *)&D_800F56F0.vry;
    t.vz = *(u16 *)&D_800F56F0.vrz;
    b = t;
    state = func_8005F174();
    if (state == 1) {
        if (func_8005F18C() == state) {
            return;
        }
    }
    if (arg1 != 0 || arg0 <= 0) {
        if (arg1 > 0) {
            index = arg1 - 1;
            arg1 = (s32)(D_800F3A10 + index * MODEL_SLOT_SIZE);
            if (arg3 >= 0) {
                D_800F2B20.eye.pair_slot = index ^ 1;
            } else {
                D_800F2B20.eye.pair_slot = -1;
            }
            D_800F2B20.eye.slot = index;
        } else {
            ModelCameraMove *p = &D_800F2B20;

            p->eye.slot = -1;
            p->eye.pair_slot = -1;
        }
    }
    if (arg2 != 0 || arg0 <= 0) {
        if (arg2 > 0) {
            index = arg2 - 1;
            arg2 = (s32)(D_800F3A10 + index * MODEL_SLOT_SIZE);
            if (arg3 >= 0) {
                D_800F2B20.target.pair_slot = index ^ 1;
            } else {
                D_800F2B20.target.pair_slot = -1;
            }
            D_800F2B20.target.slot = index;
        } else {
            ModelCameraMove *p = &D_800F2B20;

            p->target.slot = -1;
            p->target.pair_slot = -1;
        }
        if (arg0 < 0) {
            goto have_flags;
        }
    }
    D_800F2B20.mode = arg0;
have_flags:
    {
        ModelCameraMove *p = &D_800F2B20;

        p->field_02 = 0x12C;
        p->field_06 = 8;
        p->field_04 = 8;
        p->flags = 0;
        if (arg1 != 0) {
            p->flags = 1;
        } else {
            arg1 = (s32)&a;
        }
    }
    if (arg2 != 0) {
        ModelCameraMove *p = &D_800F2B20;

        p->flags = p->flags | 2;
    } else {
        arg2 = (s32)&b;
    }
    {
        ModelCameraMove *p = &D_800F2B20;

        p->eye.start_x = a.vx;
        p->eye.start_y = a.vy;
        p->eye.start_z = a.vz;
        p->eye.end_x = *(u16 *)(arg1 + 0);
        p->eye.end_y = *(u16 *)(arg1 + 2);
        p->eye.end_z = *(u16 *)(arg1 + 4);
        p->target.start_x = b.vx;
        p->target.start_y = b.vy;
        p->target.start_z = b.vz;
        p->target.end_x = *(u16 *)(arg2 + 0);
        p->target.end_y = *(u16 *)(arg2 + 2);
        p->target.end_z = *(u16 *)(arg2 + 4);
        arg3 = (arg3 < 0 ? -arg3 : arg3) * 2;
        if (arg3 > 0xFFFE) {
            p->duration = 0xFFFF;
        } else {
            p->duration = arg3;
        }
    }
    {
        ModelCameraMove *p = &D_800F2B20;

        flags = p->flags;
        p->elapsed = 0;
        if (flags != 0) {
            D_8009AF98 = 0;
        }
        if (arg3 != 0) {
            return;
        }
        /* Byte-address reads retain the alias dependency needed by retail. */
        if (flags & 1) {
            u8 *raw = (u8 *)p;

            D_800F56F0.vpx = *(s16 *)(raw + 0x14);
            D_800F56F0.vpy = *(s16 *)(raw + 0x16);
            D_800F56F0.vpz = *(s16 *)(raw + 0x18);
        }
        if (p->flags & 2) {
            s32 *q = (s32 *)&D_800F56F0;
            u8 *raw = (u8 *)p;

            q[3] = *(s16 *)(raw + 0x24);
            q[4] = *(s16 *)(raw + 0x26);
            q[5] = *(s16 *)(raw + 0x28);
        }
        if (p->flags & 3) {
            Model_UpdateViewMetrics(0);
        }
        p->flags = 0;
    }
}

void func_800530C4(void)
{
    int old = D_8009AF8C;
    D_8009AF90 = 0;
    D_8009AF8E = 0;
    D_8009B47C = 0;
    D_8009B478 = 0;
    D_8009B47A = 0xC00;
    func_800857C0(old);
    D_800F56F0.vpx = 0;
    D_800F56F0.vpy = -350;
    D_800F56F0.vpz = -1200;
    D_800F56F0.vrx = 0;
    D_800F56F0.vry = -350;
    D_800F56F0.vrz = 0;
    D_800F56F0.rz = 0;
    D_800F56F0.super = 0;
    func_80058434(1, 0x80, -85, 1200, 0);
    GsSetRefView2(&D_800F56F0);
    {
        GsCOORDUNIT *o = &D_800F56A0;
        o->rot.vz = 0;
        o->rot.vy = 0;
        o->rot.vx = 0;
        o->matrix.t[2] = 0;
        o->matrix.t[1] = 0;
        o->matrix.t[0] = 0;
        o->super = 0;
        RotMatrix_gte(&o->rot, &o->matrix);
        o->flg = 0;
    }
    D_800F5710 = D_800F56F0;
    D_8009B480 = D_8009B478_p;
    func_8004E7B0(1);
    {
        int state = func_8005F174();
        if (state != 1 || func_8005F18C() != state)
            func_80052D2C(0, 0, 0, 0);
    }
    func_8005FAE4();
}

#include "../psyq/stdarg.h"
#include "model.h"

extern s16 D_8009B488[MODEL_SLOT_COUNT];

void Model_SetSlotProperties(s32 idx, ...)
{
    va_list ap;
    s32 p1;
    s32 p2;
    s32 p3;
    s32 p4;
    s32 p5;

    va_start(ap, idx);
    p1 = va_arg(ap, s32);
    if (p1 >= 0) {
        D_800F2C40[idx].field_DF8 = p1;
        D_8009B488[idx] = p1;
    }
    if (idx < 2) {
        p2 = va_arg(ap, s32);
        p3 = va_arg(ap, s32);
        p4 = va_arg(ap, s32);
        p5 = va_arg(ap, s32);
        if (p2 >= 0) {
            D_800F2C40[idx].field_DFA = p2;
        }
        if (p3 >= 0) {
            D_800F2C40[idx].field_DFC = p3;
        }
        if (p4 >= 0) {
            D_800F2C40[idx].field_DFE = (p4 != 0);
            D_8009B48E[idx] = (p4 != 0);
        }
        if (p5 >= 0) {
            D_800F2C40[idx].field_DFF = (p5 != 0);
            D_8009B490[idx] = (p5 != 0);
        }
    } else {
        D_8009AF88 = &D_80091008[D_800F5678[0] * 0xB2];
    }
    D_8009AF94 = 15;
}

void func_800533D8(void){s16 table[256],packet[4];register s16*p=&table[1];register s32 fill=0xffff;register s32 counter=254;s32 i;table[0]=0;do{*p=fill;counter--;p++;}while(counter>=0);packet[0]=0x200;packet[1]=0xF0;packet[2]=0x100;packet[3]=1;while(IsIdleGPU(3)){}while(LoadImage2((RECT *)packet,(u32 *)table)){}while(IsIdleGPU(3)){}func_8005611C(0);func_8005611C(1);func_8005611C(2);for(i=0;i<MODEL_TINT_REQUEST_COUNT;i++)D_800F2B50[i].flags&=0xfffe;D_8009AF9B=0;D_8009AF9C=0;func_80059AE0(0x8000);D_8009AF94=0;D_8009AF9A=-1;}
