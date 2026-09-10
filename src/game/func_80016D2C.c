#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "func_80016D2C.h"

#include "ordering_tables.h"

void func_80016D2C(
    DisplayObject *style,
    GsSPRITE *digit,
    int source,
    int count
) {
    u8 temp[8];
    int i;

    Text_EncodeDecimalDigits(source, count, temp);
    for (i = count - 1; i >= 0; i--) {
        digit->u = temp[i] << 3;
        GsSortFastSprite(
            digit,
            D_800E9D90[style->ot_index],
            style->field_14
        );
        digit->x += 8;
    }
}
