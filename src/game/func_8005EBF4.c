#include "../types.h"
#include "../psyq/memory.h"

typedef struct {
    u8 bytes[0x28];
} Key;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u16 w;
} Coeff;

extern u8 D_800F5768[];
extern Key D_800F5788[];
extern u8 D_8009B078;

extern void func_8005FBC4(s32, s32, s32, s32, Coeff *, s32);

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
    pts[0] = (s16 *)(D_800F5768 + k * 8);
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
