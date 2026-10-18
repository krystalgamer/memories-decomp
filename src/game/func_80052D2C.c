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
#define D_8009B_MODEL_VISIBLE
#include "../unmatched.h"

#define SVECTOR_VIEW(vector) ((SVECTOR *)(vector))
#define MODEL_SLOT_BYTES(slots, index) \
    ((u8 *)(slots) + (index) * MODEL_SLOT_SIZE)

/* The end of the contiguous model-scene runtime: camera-move setup and scene
   reset/configuration. The four functions share gModel_RView, D_800F2B20,
   tent_ModelTintRequests, model slots, and the D_8009AFxx scene-control state. The
   camera/view correction passes that start the runtime, and the tint request
   pass func_800528AC between them and this run, are in
   model_scene_setup.c. */

/* Starts a camera move from the live view to model-slot or explicit targets.
 * A zero duration writes the selected endpoints back immediately. */
#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_80052D2C)
void func_80052D2C(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    SVECTOR a;
    SVECTOR b;
    SVECTOR t;
    s32 index;
    s32 state;
    s32 flags;

    memset(&b, 0, 8);
    b.vx = gModel_RView.vpx;
    b.vy = gModel_RView.vpy;
    b.vz = gModel_RView.vpz;
    a = b;
    memset(&t, 0, 8);
    t.vx = gModel_RView.vrx;
    t.vy = gModel_RView.vry;
    t.vz = gModel_RView.vrz;
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
            arg1 = (s32)MODEL_SLOT_BYTES(D_800F3A10, index);
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
            arg2 = (s32)MODEL_SLOT_BYTES(D_800F3A10, index);
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

        p->field_02 = MODEL_DEFAULT_PROJECTION;
#ifndef MODEL_CAMERA_MOVE_COMPACT_LAYOUT
        p->field_06 = 8;
#endif
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
        /* arg1/arg2 now hold a slot's field_DD0 position or &a/&b; both
           are SVECTOR-shaped. */
        p->eye.end_x = SVECTOR_VIEW(arg1)->vx;
        p->eye.end_y = SVECTOR_VIEW(arg1)->vy;
        p->eye.end_z = SVECTOR_VIEW(arg1)->vz;
        p->target.start_x = b.vx;
        p->target.start_y = b.vy;
        p->target.start_z = b.vz;
        p->target.end_x = SVECTOR_VIEW(arg2)->vx;
        p->target.end_y = SVECTOR_VIEW(arg2)->vy;
        p->target.end_z = SVECTOR_VIEW(arg2)->vz;
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
        if (flags & 1) {
            gModel_RView.vpx = p->eye.end_x;
            gModel_RView.vpy = p->eye.end_y;
            gModel_RView.vpz = p->eye.end_z;
        }
        if (p->flags & 2) {
            gModel_RView.vrx = p->target.end_x;
            gModel_RView.vry = p->target.end_y;
            gModel_RView.vrz = p->target.end_z;
        }
        if (p->flags & 3) {
            Model_UpdateViewMetrics(0);
        }
        p->flags = 0;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_800530C4)
void Model_ResetSceneView(void)
{
    int old = D_8009AF8C;
    D_8009AF90 = 0;
    D_8009AF8E = 0;
    gModel_wViewPitch = 0;
    gModel_wViewDistance = 0;
    gModel_wViewYaw = 0xC00;
    GsSetProjection(old);
    gModel_RView.vpx = 0;
    gModel_RView.vpy = -350;
    gModel_RView.vpz = -1200;
    gModel_RView.vrx = 0;
    gModel_RView.vry = -350;
    gModel_RView.vrz = 0;
    gModel_RView.rz = 0;
    gModel_RView.super = 0;
    func_80058434(1, 0x80, -85, 1200, 0);
    GsSetRefView2(&gModel_RView);
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
    D_800F5710 = gModel_RView;
    D_8009B480 = D_8009B478_p;
    Model_UpdateBackgroundScroll(1);
    {
        int state = func_8005F174();
        if (state != 1 || func_8005F18C() != state)
            func_80052D2C(0, 0, 0, 0);
    }
    Model_ResetEffectKeyQueue();
}
#endif

#include "../psyq/stdarg.h"
#include "model.h"

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_SET_SLOT_PROPERTIES)
/* The Japanese build sets only the slot record, without the three
   D_8009B488/D_8009B48E/D_8009B490 mirrors. */
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
        gModel_aSlots[idx].field_DF8 = p1;
#ifndef VERSION_JAPAN
        D_8009B488[idx] = p1;
#endif
    }
    if (idx < 2) {
        p2 = va_arg(ap, s32);
        p3 = va_arg(ap, s32);
        p4 = va_arg(ap, s32);
        p5 = va_arg(ap, s32);
        if (p2 >= 0) {
            gModel_aSlots[idx].field_DFA = p2;
        }
        if (p3 >= 0) {
            gModel_aSlots[idx].field_DFC = p3;
        }
        if (p4 >= 0) {
            gModel_aSlots[idx].field_DFE = (p4 != 0);
#ifndef VERSION_JAPAN
            D_8009B48E[idx] = (p4 != 0);
#endif
        }
        if (p5 >= 0) {
            gModel_aSlots[idx].field_DFF = (p5 != 0);
#ifndef VERSION_JAPAN
            D_8009B490[idx] = (p5 != 0);
#endif
        }
    } else {
        D_8009AF88 = &D_80091008[D_800F5678[0] * 0xB2];
    }
    D_8009AF94 = 15;
}
#endif

/* Regional value: the x of the row Model_ResetSceneState clears, 0x200 in the
   US build and 0x280 in the European one, whose wrapper defines its own. */
#ifndef MODEL_TEXTURE_RESET_ROW_X
#define MODEL_TEXTURE_RESET_ROW_X 0x200
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_TEXTURE_RESET)
void Model_ResetSceneState(void){s16 table[256];RECT packet;register s16*p=&table[1];register s32 fill=0xffff;register s32 counter=254;s32 i;table[0]=0;do{*p=fill;counter--;p++;}while(counter>=0);packet.x=MODEL_TEXTURE_RESET_ROW_X;packet.y=0xF0;packet.w=0x100;packet.h=1;while(IsIdleGPU(3)){}while(LoadImage2(&packet,(u32 *)table)){}while(IsIdleGPU(3)){}Model_ResetSlot(0);Model_ResetSlot(1);Model_ResetSlot(2);for(i=0;i<MODEL_TINT_REQUEST_COUNT;i++)tent_ModelTintRequests[i].flags&=0xfffe;D_8009AF9B=0;D_8009AF9C=0;Model_SetScreenYOverride(0x8000);D_8009AF94=0;D_8009AF9A=-1;}
#endif
