#include "../../types.h"
#define D_8009B118_IS_POINTER_IN_DATA
#include "../../unmatched.h"
#include "../../game/graphics_frame.h"
#include "helpers.h"

void func_80168D68(void)
{
    RECT *rect;

    func_80043B7C();
    rect = D_800E9D70;
    rect->x = 0x290;
    rect->y = 0;
    rect->w = 0x30;
    rect->h = 0x10;
    StoreImage(rect, (u32 *)D_8009B118);
    DrawSync(0);
    D_8009B118[0] = D_8009C02B;
    rect[1].x = 0x290;
    {
        RECT *destination = &rect[1];

        destination->y = 0xC0;
        destination->w = 0x30;
        destination->h = 0x10;
        LoadImage(destination, (u32 *)D_8009B118);
    }
    DrawSync(0);
}
