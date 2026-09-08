#include "../types.h"
#include "display_object.h"
#include "display_object_layout.h"

extern s16 D_800EFE38;
extern s16 D_800EFE3E[];
extern s16 D_800EFE44[];
extern s32 D_800E9D90[];
extern void func_800408D0(u8 *, s32, s32);

void func_80040BF8(void)
{
    s32 i = D_800EFE3E[0];

    if (i >= 0) {
        DisplayObject *base = D_800EFE48;
        s32 *table = (s32 *)D_800E9D90;

        do {
            DisplayObject *object =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback callback = object->update;
            u8 *data = (u8 *)object;

            i = object->next;

            if (callback != 0) {
                callback(data);
            }

            if (((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                func_800408D0(data, table[data[0x17]], *(s16 *)(data + 0x14));
            }
        } while (i >= 0);
    }
}

void func_80040CAC(void)
{
    s32 i = D_800EFE38;

    while (i >= 0) {
        DisplayObject *object = &D_800EFE48[i];
        DisplayObjectCallback callback = object->update;

        i = object->next;
        if (callback != 0) {
            callback((u8 *)object);
        }
    }
}

void func_80040D14(void)
{
    s32 i = D_800EFE44[0];

    if (i >= 0) {
        DisplayObject *base = D_800EFE48;
        s32 *table = (s32 *)D_800E9D90;

        do {
            DisplayObject *object =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback callback = object->update;
            u8 *data = (u8 *)object;

            i = object->next;

            if (callback != 0) {
                callback(data);
            }

            if (((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                void (*secondary)(u8 *, s32) =
                    *(void (**)(u8 *, s32))(data + 0x4C);

                if (secondary != 0) {
                    secondary(data, table[data[0x17]]);
                }
            }
        } while (i >= 0);
    }
}
