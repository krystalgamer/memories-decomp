#include "../types.h"
#include "display_object_layout.h"

typedef void (*ObjFn)(u8 *);
typedef struct Slot70 {
    s16 unk0;
    s16 unk2;
    u8 pad4[4];
    u16 flags;
    u16 unkA[13];
    ObjFn unk24;
    u8 pad28[DISPLAY_OBJECT_RECORD_SIZE - 0x28];
} Slot70;
extern s16 D_800EFE3C[];
extern Slot70 D_800EFE48[];
extern s32 D_800E9D90[];
extern void func_80041D60(u8 *);
extern void func_8004158C(u8 *, s32, s32);

void func_80040814(void) {
    s32 i = D_800EFE3C[0];

    if (i >= 0) {
        Slot70 *base = D_800EFE48;
        s32 *t = (s32 *)D_800E9D90;

        do {
            Slot70 *p =
                (Slot70 *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            ObjFn f = p->unk24;
            u8 *q = (u8 *)p;

            i = p->unk2;

            if (f != 0) {
                f(q);
            }

            if (((p->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                func_80041D60(q);
                func_8004158C(q, t[q[0x17]], *(s16 *)(q + 0x14));
            }
        } while (i >= 0);
    }
}
