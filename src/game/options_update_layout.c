#include "../types.h"
#include "display_object_layout.h"
#include "options_update_layout.h"
#include "options.h"
#include "display_object.h"

typedef struct { u16 h[3]; } Blk6;
extern u8 D_8009AF5C[];

void Options_UpdateLayout(s32 selection) {
    u8 sp0[12];
    DisplayObject *a;
    DisplayObject *b;
    s32 k;
    u32 v;

    *(Blk6 *)sp0 = *(Blk6 *)D_8009AF5C;
    *(s16 *)(sp0 + 8) = 0x68;
    k = gOptions_bOutputType;
    a = D_8009B380;
    b = D_8009B388;
    *(s16 *)(sp0 + 0xA) = 0xC8;
    v = *(u16 *)(sp0 + 8 - -(k * 2));
    a->field_30.h.field_32 = 0x48;
    a->field_30.h.field_30 = v;
    b->field_30.h.field_30 = 0x20;
    b->field_30.h.field_32 = *(u16 *)(sp0 - -(selection * 2)) + 8;
    if (selection == 0) {
        a->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        b->field_30.h.field_30 = a->field_30.h.field_30 + 8;
        b->field_30.h.field_32 = a->field_30.h.field_32 + 8;
    } else {
        a->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
    }
}
