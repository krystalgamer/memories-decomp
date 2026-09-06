#include "../types.h"
#include "model.h"

extern s8 D_8009B07A;
extern u8 D_8009B07B;
extern u8 D_8009B07C;

extern u8 *func_80059208(void);
extern void func_8008E3D0(void *dst, s32 value, s32 size);
extern void Model_CopySlotU16Values(s32 value, u8 *data);
extern void func_8005F91C(s32 flag, u8 *b, u8 *a, s32 arg);

void func_8005D994(
    s32 arg0,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    u8 *arg4,
    s32 arg5
) {
    u16 a[4];
    u16 b[4];
    u8 *s;
    s32 one;
    s32 m;
    s32 t;
    s32 q;
    s32 r;
    s32 x;
    s32 first;

    s = func_80059208();
    func_8008E3D0(a, 0, 8);
    a[3] = arg0 | 0x80;
    one = 1;

    if (D_8009B07B == one && D_8009B07C == one) {
        return;
    }

    if (arg0 == 2) {
        Model_CopySlotU16Values(2, (u8 *)a);
        a[3] = one;
    }

    if (arg4 != (u8 *)0) {
        m = -1;
        if (arg0 <= 0) {
            m = 1;
        }
        a[0] = a[0] + *(s16 *)(arg4 + 0) * m;
        a[1] = a[1] + *(u16 *)(arg4 + 2);
        a[2] = a[2] + *(s16 *)(arg4 + 4) * m;
    }

    b[0] = arg1;
    x = *(s16 *)(s + 2);
    t = arg2 + MODEL_ANGLE_FULL_TURN;
    if (arg0 <= 0) {
        x = x - 0xC00;
    } else {
        x = x - 0x400;
    }
    b[1] = (t - x) - (t - x) / MODEL_ANGLE_FULL_TURN *
        MODEL_ANGLE_FULL_TURN;
    r = b[1];
    if (*(s16 *)&b[1] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        b[1] = r - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&b[1] < -MODEL_ANGLE_HALF_TURN) {
        b[1] = b[1] + MODEL_ANGLE_FULL_TURN;
    }

    q = arg3 + MODEL_ANGLE_FULL_TURN;
    b[2] = (q - q / MODEL_ANGLE_FULL_TURN * MODEL_ANGLE_FULL_TURN) -
        *(u16 *)(s + 4);
    if (*(s16 *)&b[2] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        b[2] = b[2] - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&b[2] < -MODEL_ANGLE_HALF_TURN) {
        b[2] = b[2] + MODEL_ANGLE_FULL_TURN;
    }

    first = *(s8 *)&D_8009B07A;
    x = first;
    b[3] = 4;
    if (x < 0) {
        first = 0;
    } else {
        D_8009B07A = D_8009B07A + 1;
        first = x > 0;
    }
    func_8005F91C(first, (u8 *)b, (u8 *)a, arg5);
}
