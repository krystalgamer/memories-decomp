#include "../types.h"

#include "model.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 w;
} __attribute__((packed)) ModelVector;

extern u8 D_800F3A10[];
extern u16 D_800F56F0[];

extern void func_8008E3D0(void *, s32, s32);
extern s32 func_80086E50(s32);

s32 func_8005A1F4(s32 index)
{
    ModelVector output;
    ModelVector difference;
    u8 *entry = D_800F3A10 + index * MODEL_SLOT_SIZE;

    func_8008E3D0(&difference, 0, 8);
    difference.x = D_800F56F0[0] - *(u16 *)(entry + 0);
    difference.y = D_800F56F0[2] - *(u16 *)(entry + 2);
    difference.z = D_800F56F0[4] - *(u16 *)(entry + 4);
    output = difference;
    return func_80086E50(
        output.x * output.x + output.y * output.y + output.z * output.z
    );
}

s32 func_8005A2E0(s32 index)
{
    ModelVector output;
    ModelVector difference;
    u8 *entry = D_800F3A10 + index * MODEL_SLOT_SIZE;

    func_8008E3D0(&difference, 0, 8);
    difference.x = D_800F56F0[6] - *(u16 *)(entry + 0);
    difference.y = D_800F56F0[8] - *(u16 *)(entry + 2);
    difference.z = D_800F56F0[10] - *(u16 *)(entry + 4);
    output = difference;
    return func_80086E50(
        output.x * output.x + output.y * output.y + output.z * output.z
    );
}
