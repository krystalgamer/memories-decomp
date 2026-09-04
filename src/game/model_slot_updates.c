#include "../types.h"

#include "model.h"

extern void func_8005922C(void *, void *);

void func_8005A468(s32 arg0, s32 arg1)
{
    ModelSlot *rec = &D_800F2C40[arg0];
    u8 **p;
    s32 i;

    p = (u8 **)rec->field_1E0;
    i = 0;
    if (rec->field_E1B != 0) {
        do {
            (*p)[0xD] = arg1;
            p++;
            i++;
        } while (i < rec->field_E1B);
    }
}

void func_8005A4C4(unsigned char *e, int a, int b, int c, int d)
{
    if (*(void **)(e + 0xD18)) {
        *(short *)(*(unsigned char **)(e + 0xD18) + 0x44) = 0;
        *(short *)(*(unsigned char **)(e + 0xD18) + 0x46) = d;
        *(short *)(*(unsigned char **)(e + 0xD18) + 0x48) = 0;
        *(int *)(*(unsigned char **)(e + 0xD18) + 0x18) = a;
        *(int *)(*(unsigned char **)(e + 0xD18) + 0x1C) = b;
        *(int *)(*(unsigned char **)(e + 0xD18) + 0x20) = c;
    }
    func_8005922C(*(void **)(e + 0xD18), 0);
}
