#include "../types.h"
#include "../psyq/libgte.h"
#include "../unmatched.h"
#include "model_effect_state.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "model_copy_slot_u16_values.h"
#include "model.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "func_80058434.h"
#include "model_effect_endpoints.h"
#include "model_effect_requests.h"
#include "model_slot_support.h"
#include "func_8005EBF4.h"
#include "model_transfer_flags.h"
#include "func_8005D994.h"

void func_8005F27C(s32 arg0, s32 arg1, s32 arg2)
{
    u8 *r;
    s32 v;
    s32 d;
    s32 t;
    s32 k;
    s32 u;
    ModelEffectAdjustment sp18;

    r = (u8 *)D_80091570 + arg1 * 8;

    if (D_8009B07B == 1) {
        if (D_8009B07C == 1) {
            return;
        }
    }

    v = (s16)*(u16 *)r;

    if (arg0 < 2) {
        func_80059000(arg0, (s16 *)&sp18);
        if (sp18.max < 0x32) {
            sp18.max = 0x32;
        }
        sp18.max -= 0x12C;
        d = sp18.max;
        if (d != 0) {
            k = 750;
            t = v;
            if (d > 0) {
                t = v / 2;
            }
            u = d * t;
            t = u;
            v += t / k;
        }
    }

    func_8005F3B8(
        arg0, v, *(s16 *)(r + 2), *(s16 *)(r + 4), (SVECTOR *)arg2
    );
}

void func_8005F3B8(int mode, int y, int a, int b, SVECTOR *offset)
{
    unsigned short *p = func_800591FC();
    unsigned short *q = func_80059208();
    if (D_8009B07B == 1 && D_8009B07C == 1)
        return;
    if (mode >= 0) {
        short v[4];
        Model_CopySlotU16Values(mode, (u16 *)v);
        ((int *)p)[3] = v[0];
        ((int *)p)[4] = v[1];
        ((int *)p)[5] = v[2];
    }
    if (offset) {
        int sign = -1;
        if (mode <= 0)
            sign = 1;
        ((int *)p)[3] += offset->vx * sign;
        ((int *)p)[4] += offset->vy;
        ((int *)p)[5] += offset->vz * sign;
    }
    q[0] = y;
    q[1] = (a + MODEL_ANGLE_FULL_TURN +
            (mode <= 0 ? 3 * MODEL_ANGLE_QUARTER_TURN :
                         MODEL_ANGLE_QUARTER_TURN)) %
        MODEL_ANGLE_FULL_TURN;
    q[2] = (b + MODEL_ANGLE_FULL_TURN) % MODEL_ANGLE_FULL_TURN;
    func_80058434(1, MODEL_ANGLE_FULL_TURN, 0, 0, 1);
    func_8005F070(1);
    func_80059EBC(-1);
}

int func_8005F564(void)
{
    if (D_8009B07A < 0) {
        return 0;
    }
    return D_8009B07A++ > 0;
}

void func_8005F588(int value)
{
    if (D_8009B07B != 1 || D_8009B07C != 1) {
        if (value == 0) {
            D_8009B07A = -1;
        } else {
            D_8009B07A = 0;
        }
    }
}

/* If D_8009B07B==1 and D_8009B07C matches it, bail early. Otherwise reads
   D_80091570[arg1].field_00 as a base stat value; if arg0 (level?) < 2, scales
   the stat by a growth ratio derived from func_80059000's output clamped
   to >=50 then offset by -300, divided by 750, and adds the delta. Passes
   the (possibly adjusted) value plus angle/field_04 on to setup_positional_sfx. */
void func_8005F5C8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    ModelEffectCoefficient *rec = &D_80091570[arg1];
    s32 s0;

    if (D_8009B07B == 1 && D_8009B07C == D_8009B07B) {
        return;
    }

    s0 = rec->field_00;

    if (arg0 < 2) {
        ModelEffectAdjustment adjustment;
        s32 v1;

        func_80059000(arg0, (s16 *)&adjustment);
        if (adjustment.max < 0x32) {
            adjustment.max = 0x32;
        }
        adjustment.max -= 300;
        v1 = adjustment.max;
        if (v1 != 0) {
            s32 divisor = 750;
            s32 half = (v1 > 0) ? (s0 / 2) : s0;
            s0 += (v1 * half) / divisor;
        }
    }

    func_8005D994(arg0, s0, rec->angle, rec->field_04, (u8 *)arg2,
                  arg3);
}

void func_8005F714(s32 a, s32 b, s32 c)
{
    ModelEffectEndpoint *x =
        a < 0 ? 0 : &gModel_aEffectEndpoints[a];
    ModelEffectEndpoint *y =
        b < 0 ? 0 : &gModel_aEffectEndpoints[b];
    s32 flag;

    if (D_8009B07B == 1 && D_8009B07C == 1) {
        return;
    }
    flag = (s8)D_8009B07A;
    if (flag < 0) {
        flag = 0;
    } else {
        D_8009B07A++;
        flag = flag > 0;
    }
    func_8005F91C(flag, (u8 *)x, (u8 *)y, c);
}

void func_8005F7B0(s32 value, s32 arg)
{
    /* The aligned array keeps retail's four halfword stores; a packed local
       would expand them into byte stores. */
    s16 d[4] = {value, 0, 0, 5};
    s32 flag;
    s32 next;

    if (D_8009B07B == 1 && D_8009B07C == 1) {
        return;
    }
    flag = (s8)D_8009B07A;
    next = (u8)D_8009B07A;
    if (flag < 0) {
        flag = 0;
    } else {
        D_8009B07A = next + 1;
        flag = flag > 0;
    }
    func_8005F91C(flag, (u8 *)&d, (u8 *)&d, arg);
}

void func_8005F828(s32 count, void *p1, void *p2, s32 arg3)
{
    s8 raw172;
    s32 flag;

    if (D_8009B07B == 1 && D_8009B07C == D_8009B07B) {
        return;
    }

    raw172 = D_8009B07A;
    if (raw172 < 0) {
        flag = 0;
    } else {
        D_8009B07A = (u8)D_8009B07A + 1;
        flag = raw172 > 0;
    }

    if (count == 0) {
        return;
    }

    do {
        func_8005F91C(flag, (u8 *)p1, (u8 *)p2, arg3);

        if (p1 != 0) {
            p1 = (u8 *)p1 + 8;
        }
        if (p2 != 0) {
            p2 = (u8 *)p2 + 8;
        }

        if (p1 != 0 && *(s16 *)((u8 *)p1 + 6) != 0) {
            goto cont;
        }
        if (p2 == 0) {
            return;
        }
        if (*(s16 *)((u8 *)p2 + 6) == 0) {
            return;
        }
cont:
        if (count > 0) {
            count--;
        }
        flag = 1;
    } while (count != 0);
}

void func_8005F91C(s32 arg0, u8 *arg1, u8 *arg2, s32 arg3)
{
    u8 *p;
    u8 *q;
    u8 *r;
    s32 n;
    s32 i;
    s32 one;

    one = 1;
    if (D_8009B07B == one && D_8009B07C == one) {
        return;
    }

    if (arg0 == one) {
        goto m1;
    }
    if (arg0 < 2) {
        if (arg0 == 0) {
            goto m0;
        }
        return;
    }
    if (arg0 == 2) {
        goto m2;
    }
    return;

m0:
    func_80059EBC(-1);
    D_8009B074 = (u8 *)D_800F5788;
    D_8009B078 = 0;
    if (arg1 == (u8 *)0 && arg2 == (u8 *)0 &&
        (arg3 == 0 || arg3 == 0x4000)) {
        return;
    }
    func_8005F070(0);
    D_8009B079 = 1;

m1:
    if (D_8009B078 < 0xA) {
        r = (u8 *)&D_800F5788[D_8009B078];
        if (arg1 != (u8 *)0) {
            *(ModelEffectEndpoint *)r = *(ModelEffectEndpoint *)arg1;
        } else {
            *(u16 *)(r + 6) = 0;
        }
        if (arg2 != (u8 *)0) {
            *(ModelEffectEndpoint *)(r + 8) =
                *(ModelEffectEndpoint *)arg2;
            r[0x26] = 0;
        } else {
            *(u16 *)(r + 0xE) = 0;
            r[0x26] = 0;
        }
        n = D_8009B078;
        *(s16 *)(r + 0x20) = arg3;
        *(s16 *)(r + 0x22) = 0;
        *(s16 *)(r + 0x24) = 0;
        D_8009B078 = n + 1;
    }
    return;

m2:
    q = (u8 *)D_800F5788;
    for (i = 0; i < D_8009B078; i++) {
        p = q;
        q += 0x28;
        func_8005E808(p);
    }
    D_8009B079 = 0;
    D_8009B07A = -1;
}
