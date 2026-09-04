#include "../types.h"

typedef struct {
    u8 pad_00[0x30];
    s16 x;
    s16 y;
} DisplayPositionChild;

typedef struct {
    DisplayPositionChild *children[3];
    u8 pad_0C[0x28];
    s16 x;
    s16 y;
} DisplayPositionGroup;

void func_8003A920(
    DisplayPositionGroup *group,
    s16 x,
    s16 y
)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        if (group->children[i] != 0) {
            group->children[i]->x = x;
            group->children[i]->y = y;
        }
    }
}

void func_8003A95C(DisplayPositionGroup *group, s32 x, s32 y)
{
    group->x = x;
    group->y = y;
    func_8003A920(group, (s16)x, (s16)y);
}
