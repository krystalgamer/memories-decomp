#include "../../types.h"
#include "credits.h"

void func_80181C4C(s32 group)
{
    CreditsLine *line;
    s32 i;

    i = 0;
    line = D_8018220C;
next:
    if (line->entries == 0) {
        line->height = 0;
        line->width = 0;
        do {
            line->page = 0;
            if (func_80180F58(i, group) != 0) {
                line->x = 40;
                line->y = 40;
                if (line->width > 160 || line->height > 120) {
                    line->x = (320 - line->width) / 2;
                    line->y = (240 - line->height) / 2 - 10;
                }
                line->state = 0;
                line->timer = 152;
                line->field_13 = 0;
            }
            return;
        } while (0);
    }
    i++;
    line++;
    if (i < 2) {
        goto next;
    }
}

u8 func_80181D28(void)
{
    return D_80182208;
}
