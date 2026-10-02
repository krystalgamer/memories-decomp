#include "../../types.h"
#include "helpers.h"

void func_80168048(s32 selection)
{
    D_8016913C->field_30.h.field_30 = D_80169070 * 112 + 60;
    D_8016913C->field_30.h.field_32 = 80;
    if (selection != 0) {
        D_8016913C->field_30.h.field_32 = 112;
    }
    DisplayObject_UpdateResourceVariant(D_80169078, D_80169070);
}
