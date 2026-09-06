#include "../types.h"
#include "../psyq/libgte.h"

extern void func_80089060(void *, void *);
void func_8005922C(u8 *object, void *source)
{
    func_80089060(object + 0x44, object + 4);
    if (source != 0) ScaleMatrix((MATRIX *)(object + 4), source);
    *(int *)object = 0;
}
