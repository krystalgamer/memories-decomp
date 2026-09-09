#include "../types.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_api.h"

void func_800402A0(DisplayObject *arg0, s32 arg1) {
    u16 saved = arg0->flags;
    s32 v;

    func_8004020C(arg0);
    v = *(s16 *)((u8 *)D_800EFE38 + arg1 * 2);
    if (v < 0) {
        *(u16 *)((u8 *)D_800F2878 + arg1 * 2) = arg0->field_0A;
        arg0->next = -1;
        arg0->previous = -1;
    } else {
        D_800EFE48[v].previous = arg0->field_0A;
        arg0->next = *(u16 *)((u8 *)D_800EFE38 + arg1 * 2);
    }
    arg0->previous = -1;
    *(u16 *)((u8 *)D_800EFE38 + arg1 * 2) = arg0->field_0A;
    arg0->flags = saved;
}
