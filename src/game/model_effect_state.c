#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/memory.h"
#include "../unmatched.h"
#include "model_effect_coefficients.h"
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
#include "model_transfer_state.h"
#include "func_8005D994.h"
#include "func_8005FBC4.h"

/* Evaluates one channel of the keyframe ring that func_8005F91C fills and
   func_8005F070 seeds. `cur` points at a 0x28-byte key inside D_800F5788, so
   `cur - D_800F5788` is the ring index; the three keys the segment spans are
   that index and the next two modulo the live key count in D_8009B078. Keys
   whose 0x8-byte channel record has kind 1 at +6 terminate the segment, and
   the tail is padded with the last real key. The four control points handed to
   func_8005FBC4 are the current pose at D_800F5768 followed by those three
   keys, except that a non-zero `den` also replaces the first with the previous
   key when that one is kind 1. With `den` zero the divisor is instead the sum
   of the distinct keys' +0x22 durations. Each of the three components is then
   Horner-evaluated at scale/den and added to the constant term. */
void func_8005EBF4(Key *cur, s32 k, s32 scale, s32 den, s16 *out)
{
    Key *keys[3] = {
        &D_800F5788[(cur - D_800F5788) % D_8009B078],
        &D_800F5788[(cur - D_800F5788 + 1) % D_8009B078],
        &D_800F5788[(cur - D_800F5788 + 2) % D_8009B078]
    };
    s16 *pts[4];
    Coeff co[3];
    s32 i;
    s32 j;

    memset(pts, 0, 16);
    pts[0] = (s16 *)((u8 *)D_800F5768 + k * 8);
    for (i = 1; i < 3; i++) {
        u8 *e;

        e = (u8 *)keys[i] + k * 8;
        if (*(s16 *)(e + 6) != 1) {
            break;
        }
    }
    j = i - 1;
    if (i < 3) {
        for (; i < 3; i++) {
            keys[i] = keys[j];
        }
    }
    for (i = 0; i < 3; i++) {
        pts[i + 1] = (s16 *)((u8 *)keys[i] + k * 8);
    }
    if (den != 0) {
        Key *kp;
        s16 *p;

        kp = &D_800F5788[(cur - D_800F5788 + D_8009B078 - 1) % D_8009B078];
        p = (s16 *)((u8 *)kp + k * 8);
        if (*(s16 *)((u8 *)p + 6) == 1) {
            pts[0] = p;
        }
        i = 0;
    } else {
        den = *(u16 *)((u8 *)keys[0] + 0x22);
        for (i = 1; i < 3; i++) {
            if (keys[i - 1] != keys[i]) {
                den += *(u16 *)((u8 *)keys[i] + 0x22);
            }
        }
        i = 0;
    }
    for (i = 0; i < 3; i++) {
        s32 v;

        func_8005FBC4(pts[0][i], pts[1][i], pts[2][i], pts[3][i], &co[i], 2);
        v = co[i].x * scale / den;
        v = (v + co[i].y) * scale / den;
        v = (v + co[i].z) * scale / den;
        out[i] = co[i].w + v;
    }
}

/* Refreshes the current pose at D_800F5768 that the evaluator above uses as
   its first control point, from the two source records func_800591FC and
   func_80059208 return, and optionally replays up to two channel records
   from D_8009B074 into the model slots. This is the seed half of the ring
   the evaluator reads, which is why the two share a translation unit. */
void func_8005F070(s32 enabled)
{
    u8 *left = func_800591FC();
    u8 *right = func_80059208();

    *(u16 *)&D_800F5768[0].vx = *(u16 *)(left + 0);
    *(u16 *)&D_800F5768[0].vy = *(u16 *)(left + 4);
    *(u16 *)&D_800F5768[0].vz = *(u16 *)(left + 8);
    *(u16 *)&D_800F5768[1].vx = *(u16 *)(left + 12);
    *(u16 *)&D_800F5768[1].vy = *(u16 *)(left + 16);
    *(u16 *)&D_800F5768[1].vz = *(u16 *)(left + 20);
    *(u16 *)&D_800F5768[2].vx = *(u16 *)(right + 0);
    *(u16 *)&D_800F5768[2].vy = *(u16 *)(right + 2);
    *(u16 *)&D_800F5768[2].vz = *(u16 *)(right + 4);
    if (enabled != 0) {
        Key *key = D_8009B074;
        if (key != (Key *)0) {
            s32 i = 0;
            s32 offset = 16;
            u8 *entry = (u8 *)key;
            for (; i < 2; offset += 8, i++, entry += 8) {
                s32 kind = *(s16 *)(entry + 6);
                if (kind < 4) {
                    if (kind >= 2)
                        Model_CopySlotU16Values(
                            *(s16 *)entry, (u16 *)((u8 *)key + offset)
                        );
                }
            }
        }
    }
}

s32 func_8005F174(void)
{
    return D_8009B07B;
}

void func_8005F180(s32 value)
{
    D_8009B07B = value;
}

s32 func_8005F18C(void)
{
    return D_8009B07C;
}

void func_8005F198(s32 value)
{
    D_8009B07C = value;
}

ModelEffectCoefficient *func_8005F1A4(s32 index)
{
    return &D_80091570[index];
}

s32 func_8005F1B8(s32 level, s32 value)
{
    ModelEffectAdjustment local;
    s16 delta;

    if (level >= 2) {
        return value;
    }

    func_80059000(level, (s16 *)&local);

    if (local.max < 50) {
        local.max = 50;
    }

    local.max -= 300;
    delta = local.max;

    if (delta != 0) {
        s32 divisor = 750;
        s32 half = value;

        if (delta > 0) {
            half = (s32)(value + ((u32)value >> 31)) >> 1;
        }
        value += (delta * half) / divisor;
    }

    return value;
}

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
    func_8005F91C(flag, x, y, c);
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
    func_8005F91C(
        flag, (ModelEffectEndpoint *)&d, (ModelEffectEndpoint *)&d, arg
    );
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
        func_8005F91C(
            flag,
            (ModelEffectEndpoint *)p1,
            (ModelEffectEndpoint *)p2,
            arg3
        );

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

void func_8005F91C(
    s32 arg0,
    ModelEffectEndpoint *arg1,
    ModelEffectEndpoint *arg2,
    s32 arg3
)
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
    D_8009B074 = D_800F5788;
    D_8009B078 = 0;
    if (arg1 == (ModelEffectEndpoint *)0 &&
        arg2 == (ModelEffectEndpoint *)0 &&
        (arg3 == 0 || arg3 == 0x4000)) {
        return;
    }
    func_8005F070(0);
    D_8009B079 = 1;

m1:
    if (D_8009B078 < 0xA) {
        Key *key = &D_800F5788[D_8009B078];
        r = (u8 *)key;
        if (arg1 != (ModelEffectEndpoint *)0) {
            key->requested[0] = *arg1;
        } else {
            key->requested[0].kind = 0;
        }
        if (arg2 != (ModelEffectEndpoint *)0) {
            key->requested[1] = *arg2;
            key->ready = 0;
        } else {
            key->requested[1].kind = 0;
            key->ready = 0;
        }
        n = D_8009B078;
        key->magnitude = arg3;
        key->radius = 0;
        key->progress = 0;
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

void func_8005FAE4(void)
{
    D_8009B074 = 0;
    D_8009B078 = 0;
    D_8009B079 = 0;
    D_8009B07A = -1;
    D_8009B07B = 0;
    D_8009B07C = 0;
}

int func_8005FB08(void)
{
    return D_8009B078 == 0;
}

int func_8005FB14(void)
{
    int value = 0;

    if (D_8009B078 != 0) {
        value = (s32)D_8009B074;
    }
    return value;
}

void func_8005FB30(Key *key)
{
    u8 *data = (u8 *)key;
    int i;

    if (!data) {
        data = (u8 *)D_8009B074;
    }
    if (!data) {
        return;
    }
    for (i = 0; i < 2; i++) {
        ModelTransferItem *item = (ModelTransferItem *)(data + i * 8);

        if (item->state < 4) {
            if (item->state >= 2) {
                Model_CopySlotU16Values(item->id, (u16 *)(data + 0x10 + i * 8));
            }
        }
    }
}

void func_8005FBC4(int a, int b, int c, int d, Coeff *out, int scale)
{
    int x = (b - c) * scale;
    int y = (a + c - b * 2) * scale;
    int z = (b - a) * scale;
    out->w = a;
    out->x = x - a + d;
    out->y = y;
    out->z = z;
}
