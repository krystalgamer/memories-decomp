#include "../types.h"
#include "model_scene_setup.h"
#include "model_transfer_flags.h"
#include "model_update_view_metrics.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "func_80058434.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h; camera_view.h above
   supplies libgs.h. */
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_scene_states.h"
#include "model_transfer_state.h"

extern unsigned short D_8009AF8C, D_8009AF90, D_8009AF8E;
extern ModelBytes8 D_8009B478_p asm("D_8009B478");
extern ModelBytes8 D_8009B480;
extern GsCOORDUNIT D_800F56A0;
extern void func_800857C0(int);
extern void func_8004E7B0(int), func_80052D2C(int, int, int, int);

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

extern s16 D_8009B488[3];
extern u8 D_8009B48E[2];
extern u8 D_8009B490[2];
extern u8 *D_8009AF88;
extern u16 D_800F5678[];
extern u8 D_80091008[];

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

extern u8 D_8009AF9B;extern s8 D_8009AF9A;extern s32 D_8009AF9C;extern void func_8005611C(int),func_80059AE0(int);void func_800533D8(void){s16 table[256],packet[4];register s16*p=&table[1];register s32 fill=0xffff;register s32 counter=254;s32 i;table[0]=0;do{*p=fill;counter--;p++;}while(counter>=0);packet[0]=0x200;packet[1]=0xF0;packet[2]=0x100;packet[3]=1;while(IsIdleGPU(3)){}while(LoadImage2((RECT *)packet,(u32 *)table)){}while(IsIdleGPU(3)){}func_8005611C(0);func_8005611C(1);func_8005611C(2);for(i=0;i<10;i++)D_800F2B50[i].flags&=0xfffe;D_8009AF9B=0;D_8009AF9C=0;func_80059AE0(0x8000);D_8009AF94=0;D_8009AF9A=-1;}
