#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/memory.h"

#include "model.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 w;
} __attribute__((packed)) ModelVector;

extern u8 D_800F3A10[];
extern u16 D_800F56F0[];

s32 func_8005A1F4(s32 index)
{
    ModelVector output;
    ModelVector difference;
    u8 *entry = D_800F3A10 + index * MODEL_SLOT_SIZE;

    memset(&difference, 0, 8);
    difference.x = D_800F56F0[0] - *(u16 *)(entry + 0);
    difference.y = D_800F56F0[2] - *(u16 *)(entry + 2);
    difference.z = D_800F56F0[4] - *(u16 *)(entry + 4);
    output = difference;
    return SquareRoot0(
        output.x * output.x + output.y * output.y + output.z * output.z
    );
}

s32 func_8005A2E0(s32 index)
{
    ModelVector output;
    ModelVector difference;
    u8 *entry = D_800F3A10 + index * MODEL_SLOT_SIZE;

    memset(&difference, 0, 8);
    difference.x = D_800F56F0[6] - *(u16 *)(entry + 0);
    difference.y = D_800F56F0[8] - *(u16 *)(entry + 2);
    difference.z = D_800F56F0[10] - *(u16 *)(entry + 4);
    output = difference;
    return SquareRoot0(
        output.x * output.x + output.y * output.y + output.z * output.z
    );
}
