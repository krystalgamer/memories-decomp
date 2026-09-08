#include "../../types.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"

u8 *func_801680B4(s32 x, s32 y)
{
    u8 *object;

    object = func_800400AC(func_8004002C(), 2);
    func_800404CC(object, x, y, 0, 0, 0, 0x17, 0x101);
    *(u16 *)(object + 8) =
        *(u16 *)(object + 8) | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    return object;
}
