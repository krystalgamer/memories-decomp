#include "../types.h"

typedef struct {
    s16 a;
    s16 b;
    s16 c;
    s16 d;
} Data;

extern u8 D_80091550[];
extern u8 D_8009B07A;
extern u8 D_8009B07B;
extern u8 D_8009B07C;

extern void func_8005F91C(s32, void *, void *, s32);

void func_8005F714(s32 a, s32 b, s32 c)
{
    void *x = a < 0 ? 0 : D_80091550 + a * 8;
    void *y = b < 0 ? 0 : D_80091550 + b * 8;
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
    Data d = {value, 0, 0, 5};
    s32 flag;
    s32 next;

    if (D_8009B07B == 1 && D_8009B07C == 1) {
        return;
    }
    flag = (s8)D_8009B07A;
    next = D_8009B07A;
    if (flag < 0) {
        flag = 0;
    } else {
        D_8009B07A = next + 1;
        flag = flag > 0;
    }
    func_8005F91C(flag, &d, &d, arg);
}
