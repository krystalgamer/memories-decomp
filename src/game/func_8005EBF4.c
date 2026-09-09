#include "../types.h"
#include "model_update_view_metrics.h"
#include "camera_view.h"
#include "../psyq/memory.h"
#include "func_8005EBF4.h"
#include "func_8005FBC4.h"
#include "model_copy_slot_u16_values.h"
#include "model_transfer_flags.h"

extern Key D_800F5788[];

extern u8 *D_8009B074;

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
        u8 *table = D_8009B074;
        if (table != (u8 *)0) {
            s32 i = 0;
            s32 offset = 16;
            u8 *entry = table;
            for (; i < 2; offset += 8, i++, entry += 8) {
                s32 kind = *(s16 *)(entry + 6);
                if (kind < 4) {
                    if (kind >= 2)
                        Model_CopySlotU16Values(*(s16 *)entry, (u16 *)(table + offset));
                }
            }
        }
    }
}
