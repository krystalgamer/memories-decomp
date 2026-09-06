#include "../types.h"
#include "model.h"

typedef struct { short x, y, z, w; } Vec;
extern unsigned short *func_800591FC(void), *func_80059208(void);
extern unsigned char D_8009B07B, D_8009B07C;
extern void Model_CopySlotU16Values(int, short *);
extern void func_80058434(int, int, int, int, int);
extern void func_8005F070(int), func_80059EBC(int);

void func_8005F3B8(int mode, int y, int a, int b, Vec *offset)
{
    unsigned short *p = func_800591FC();
    unsigned short *q = func_80059208();
    if (D_8009B07B == 1 && D_8009B07C == 1)
        return;
    if (mode >= 0) {
        short v[4];
        Model_CopySlotU16Values(mode, v);
        ((int *)p)[3] = v[0];
        ((int *)p)[4] = v[1];
        ((int *)p)[5] = v[2];
    }
    if (offset) {
        int sign = -1;
        if (mode <= 0)
            sign = 1;
        ((int *)p)[3] += offset->x * sign;
        ((int *)p)[4] += offset->y;
        ((int *)p)[5] += offset->z * sign;
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
