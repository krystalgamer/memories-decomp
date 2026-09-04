#include "../types.h"

typedef struct {
    s16 field_00;
    s16 field_02;
    u8 pad_04[0x6C];
} LocalEntry;

typedef struct {
    s16 field_00;
    s16 field_02;
    u8 pad_04[4];
    s16 field_08;
    u8 pad_0A[0x14];
    s16 field_1E;
} LocalObject;

extern u16 D_8009B410;
extern u16 D_800EFE38[];
extern LocalEntry D_800EFE48[];

void func_8004020C(LocalObject *object)
{
    s32 first = object->field_00;
    s32 second = object->field_02;

    D_8009B410++;

    if (first < 0) {
        s16 index = object->field_1E;
        D_800EFE38[index] = second;
        if (second >= 0) {
            D_800EFE48[second].field_00 = -1;
        }
    } else {
        D_800EFE48[first].field_02 = second;
        if (second >= 0) {
            D_800EFE48[second].field_00 = first;
        }
    }

    object->field_08 = 0;
}
