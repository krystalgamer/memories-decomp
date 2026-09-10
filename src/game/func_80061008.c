#include "../types.h"
#include "graphics_frame.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_helpers.h"
#include "func_80060B38.h"
#include "card_list_rows.h"
#include "func_80061008.h"

/* Keep symbolic usage evidence while preserving the literal-address codegen. */
#define gGraphics_sViewportY_abs (*(s16 *)0x8009B148)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009B146)

void func_80061008(s32 x0, s32 y0, s32 x1, s32 y1)
{
    CardListRowSet *record = D_801A8000;
    DisplayObject *object;
    s32 i = 0;

    record[0].x = x0 + 2;
    record[0].y = y0;
    record[1].x = x1 + 2;
    record[1].y = y1;

    do {
        record->enabled = 0;
        object = func_800400AC(func_8004002C(), 6);
        object->field_68 = 0;
        object->field_69 = 0;
        object->field_30.h.field_30 = record->x;
        object->field_30.h.field_32 = record->y;
        object->field_6A = i++;
        func_80042918(object);
        object->field_4C = (s32)func_80060B38;
        record->object = (DuelCardDisplayObject *)object;
        record++;
    } while (i < 2);

    gGraphics_sViewportY_abs = 0;
    gGraphics_sViewportX_abs = 0;
}
