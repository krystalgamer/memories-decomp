#include "../types.h"

typedef struct {
    unsigned char padding[0x10];
    int x;
    int y;
    int z;
    int x_offset;
    int y_offset;
    int z_offset;
} Object;

extern void GsSetRefView2(int *, int, int, int);

void func_800134E0(Object *object, int x, int y, int z)
{
    int *vector = &object->x;

    x += vector[3];
    vector[0] = x;
    y += vector[4];
    z += vector[5];
    vector[1] = y;
    vector[2] = z;
    GsSetRefView2(vector, x, y, z);
}
