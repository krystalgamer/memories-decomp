#include "../types.h"
#include "func_8003B378.h"
#include "rand_get_interval.h"
#include "func_8003134C.h"
#include "menu_record.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "../unmatched.h"

extern u16 D_8009B2DE;
extern DisplayObject *D_800EB184[4];
extern DisplayObject *D_8009B2E4;

extern void func_8002FD10(s32);

void func_80031354(void)
{
    u8 flags = D_8009B2EB;

    if (!(flags & 0x80)) {
        D_8009B2EB = flags | 0x80;
        D_8009B2DE = 0;
        func_8002FD10(0);
    }

    func_8003134C();

    if (D_8009B2DE == 0) {
        DisplayObject *first = D_800EB184[0];
        DisplayObject *second = D_8009B2E4;
        u16 second_flags;

        first->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;

        second_flags = second->flags;
        D_8009B2EB = 0;
        second_flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        second->flags = second_flags;
    } else {
        s32 value = Rand_GetInterval(4);
        func_8003B378((u8 *)D_800EB010, value);
    }
}
