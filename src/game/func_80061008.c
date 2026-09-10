#include "../types.h"
#include "graphics_frame.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_helpers.h"

extern u8 D_801A8000[];

/* Keep symbolic usage evidence while preserving the literal-address codegen. */
#define gGraphics_sViewportY_abs (*(s16 *)0x8009B148)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009B146)
extern void func_80060B38(void);

void func_80061008(s32 x0, s32 y0, s32 x1, s32 y1)
{
    u8 *record = D_801A8000;
    DisplayObject *object;
    s32 i = 0;

    *(s16 *)(record + 4) = x0 + 2;
    *(s16 *)(record + 6) = y0;
    *(s16 *)(record + 0x1C) = x1 + 2;
    *(s16 *)(record + 0x1E) = y1;

    do {
        *(s16 *)(record + 8) = 0;
        object = func_800400AC(func_8004002C(), 6);
        object->field_68 = 0;
        object->field_69 = 0;
        object->field_30.h.field_30 = *(u16 *)(record + 4);
        object->field_30.h.field_32 = *(u16 *)(record + 6);
        object->field_6A = i++;
        func_80042918(object);
        object->field_4C = (s32)func_80060B38;
        *(u8 **)record = (u8 *)object;
        record += 0x18;
    } while (i < 2);

    gGraphics_sViewportY_abs = 0;
    gGraphics_sViewportX_abs = 0;
}
