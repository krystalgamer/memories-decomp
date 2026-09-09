#include "../types.h"
#include "display_object_position.h"

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
