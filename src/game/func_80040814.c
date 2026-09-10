#include "../types.h"
#include "display_object.h"
#include "display_object_layout.h"
#define FUNC_80041D60_AMBIENT_ARGS
#include "func_80041D60.h"
#include "../unmatched.h"
#include "func_80040814.h"

#include "ordering_tables.h"

void func_80040814(void) {
    s32 i = D_800EFE3C;

    if (i >= 0) {
        DisplayObject *base = D_800EFE48;
        GsOT **t = D_800E9D90;

        do {
            DisplayObject *p =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback f = p->update;
            u8 *q = (u8 *)p;

            i = p->next;

            if (f != 0) {
                f(q);
            }

            if (((p->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                func_80041D60((DisplayObject *)q);
                func_8004158C(q, (s32)t[p->ot_index], *(s16 *)(q + 0x14));
            }
        } while (i >= 0);
    }
}
