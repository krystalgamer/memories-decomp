#include "../types.h"
#include "model_view_adjustments.h"
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

extern s16 D_800F2B4A[];
extern s32 Model_HasInsufficientBufferSpace(s32, s32);
extern s32 func_80059AA8(s32, s32);
extern ModelSlot D_800F2C40_alias[] asm("D_800F2C40");
extern u8 D_8009AF98;
extern unsigned short D_8009AF8C, D_8009AF90, D_8009AF8E;
extern ModelBytes8 D_8009B478_p asm("D_8009B478");
extern ModelBytes8 D_8009B480;
extern void func_800857C0(int);
extern void func_8004E7B0(int);

/* The contiguous model-scene runtime: camera/view correction, tint request
   processing, camera-move setup, and scene reset/configuration. The seven
   functions share D_800F56F0, D_800F2B20, D_800F2B50, model slots, and the
   D_8009AFxx scene-control state. */

static __inline__ ModelTintRequest *make_table_base(void)
{
    register union { u64 d; struct { u32 lo; u32 hi; } w; } wide asm("$2");
    register volatile u32 addr asm("$2");
    addr = (u32)D_800F2B50;
    wide.w.lo = addr;
    return (ModelTintRequest *)(u32)wide.d;
}

void func_80052528(void) {
    u8 *b;
    s32 x;
    s32 f;
    s32 v;
    s32 lo;
    s32 c;
    s32 d;

    if (*(s16 *)&D_8009B47C >= 0x801) {
        b = (u8 *)&D_800F56F0;
        x = *(s32 *)(b + 0x10);
        f = 0;
        if (x >= -0xC7 && *(s32 *)(b + 4) < x) {
            if (x < -0x64) {
                f = (-200 - x) * MODEL_FIXED_ONE / -100;
            } else {
                f = MODEL_FIXED_ONE;
            }
        }

        v = (f * 3755 + (MODEL_FIXED_ONE - f) * 3968) /
            MODEL_FIXED_ONE;
        lo = v - 0x10;

        do {
            c = *(s16 *)&D_8009B47C;
            d = v + 0x10;
        } while (0);

        if (d < c) {
            d = d - c;
            if (d < -1) {
                d = -1;
            }
            func_80058434(1, 0, d, 0, 0);
        } else if (c < 0xEAB) {
            func_80058434(1, 0, 0xEAB - c, 0, 0);
        } else if (c < lo) {
            d = lo - c;
            if (d >= 5) {
                d = 4;
            }
            func_80058434(1, 0, d, 0, 0);
        }
    } else {
        func_80058434(1, 0, -4, 0, 0);
    }
}

void func_80052694(s32 arg0) {
    ModelCameraMove *e;
    ModelCameraMove *g;
    s32 n;
    s32 lo;
    s32 hi;
    s32 z;
    s32 d;
    s32 t;

    n = D_800F2B4A[0];
    lo = 0x2BC;
    hi = 0x3E8;
    z = 0;

    if (n >= 0) {
        lo = func_8005F1B8(n, lo);
        if (lo >= hi) {
            hi = func_8005F1B8(n, hi);
        }
    }

    if (func_80051350(1, 0x96, 0) != 0) {
        e = &D_800F2B20;
        if (e->field_04 < 0x10) {
            e->field_04 = e->field_04 + 1;
        }
        e->flags = e->flags & 0xFE;
    } else {
        g = &D_800F2B20;
        if (g->field_06 < g->field_04) {
            g->field_04 = g->field_04 - 1;
        }
    }

    func_80052528();

    t = *(s16 *)&D_8009B478;
    if (t < 0x12C) {
        if (t < 0x96) {
            d = 0x96 - t;
            func_80058434(1, z, 0, d, 0);
        } else {
            d = 0x12C - t;
            if (d >= 0xB) {
                d = 0xA;
            }
            func_80058434(1, z, 0, d, 0);
        }
    } else if (t < lo) {
        d = lo - t;
        if (d >= 6) {
            d = 5;
        }
        func_80058434(1, 0, 0, d, 0);
    }

    if (arg0 != 0) {
        d = *(s16 *)&D_8009B478;
        if (hi < d) {
            goto last;
        }
        if (lo < d) {
            d = lo - d;
            if (d < -0xA) {
                d = -0xA;
            }
            func_80058434(1, 0, 0, d, 0);
        }
    }

    if (hi < *(s16 *)&D_8009B478) {
last:
        d = hi - *(s16 *)&D_8009B478;
        if (d < -0x14) {
            d = -0x14;
        }
        func_80058434(1, 0, 0, d, 0);
    }

    func_80052528();
}

/* Per-frame tint pass over the ten requests at D_800F2B50. A live request
 * interpolates its start colour towards its end colour, redraws the slot with
 * the requested part id, then restores the slot and advances the request. */
void func_800528AC(void)
{
    /* File scope changes the later varargs function and grows it by 0xC. */
    register const u32 hard_zero asm("$0");
    ModelTintColor save;
    ModelTintColor col;
    s32 i;
    s32 j;
    s32 k;
    register u32 v asm("$18");
    s32 keep;
    s32 old;
    u8 side;
    u16 sav06;
    s32 off;
    u16 lo;
    u16 hi;
    u16 a;
    register s32 aa asm("$19");
    s32 sv;
    ModelSlot *slot;
    ModelTintRequest *e;
    ModelTintRequest *table;

    table = make_table_base();
    for (i = 0, off = 0; i < MODEL_TINT_REQUEST_COUNT; off += 0x18, i++) {
        e = &table[i];
        if ((*(u8 *)e & 1) == 0) {
            continue;
        }
        if (D_800F2C40[(e->flags >> 1) & 1].field_E1F == 0) {
            continue;
        }
        if (Model_HasInsufficientBufferSpace((e->flags >> 1) & 1, e->start.b3)) {
            goto tail;
        }
        v = e->flags;
        a = e->field_0A;
        lo = e->elapsed;
        hi = e->duration;
        side = (v >> 1) & 1;
        v = (v >> 3) & 0x1F;
        {
            register s32 zero_arg asm("$5") = hard_zero;
            {
                register u32 model_base asm("$9") = (u32)D_800F2C40_alias;
                slot = (ModelSlot *)((u32)side * sizeof(ModelSlot) + model_base);
            }
            sav06 = slot->field_E06;
            keep = slot->field_BF5;
            old = func_80059AA8(side, zero_arg);
        }
        save = *(ModelTintColor *)slot->field_DC0;
        col.b3 = e->start.b3;
        col.b0 = ({ register s32 p asm("$3"); p = e->start.b0 * (hi - lo); p / hi; })
                 + ({ register s32 p asm("$3"); p = e->end.b0 * lo; p / hi; });
        col.b1 = ({ register s32 p asm("$3"); p = e->start.b1 * (hi - lo); p / hi; })
                 + ({ register s32 p asm("$3"); p = e->end.b1 * lo; p / hi; });
        {
            register s32 p asm("$3");
            register s32 out asm("$4");
            register s32 q asm("$16");
            p = e->start.b2 * (hi - lo);
            out = p / hi;
            p = e->end.b2 * lo;
            q = p / hi;
            out += q;
            col.b2 = out;
        }
        *(ModelTintColor *)slot->field_DC0 = col;

        aa = a;
        for (j = 0; j < slot->field_E1B; j++) {
            s32 t = slot->field_BF5;
            if (v != 0) {
                *((u8 *)slot->field_1E0[j] + 0xC) = v;
                t = v;
            }
            func_8004DC38((u8 *)slot, j, t, aa);
        }
        if (v != 0) {
            slot->field_BF5 = v;
        }

        {
            register s32 render_side asm("$4");
            render_side = side;
            {
                register s32 global_ptr asm("$3");
                register u32 global_flag asm("$2");
                register s32 global_base asm("$9") =
                    (s32)((u8 *)D_800F2B50 + 1);
                global_ptr = off + global_base;
                global_flag = (e->flags >> 2) & 1;
                D_8009AF9C = global_ptr;
                D_8009AF9B = global_flag;
                func_800540B4(render_side);
            }
        }
        D_8009AF9C = 0;
        D_8009AF9B = 0;

        sv = sav06;
        for (k = 0; k < slot->field_E1B; k++) {
            s32 t = slot->field_BF5;
            if (keep != 0) {
                *((u8 *)slot->field_1E0[k] + 0xC) = keep;
                t = keep;
            }
            func_8004DC38((u8 *)slot, k, t, sv);
        }
        if (keep != 0) {
            slot->field_BF5 = keep;
        }

        func_80059AA8(side, old);
        *(ModelTintColor *)slot->field_DC0 = save;
tail:
        e->elapsed += func_80058E1C();
        if (e->elapsed >= e->duration) {
            e->flags &= 0xFFFE;
        }
    }
}

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
extern u8 D_8009B48E[2];
extern u8 D_8009B490[2];
extern u8 *D_8009AF88;
extern u16 D_800F5678[];

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
