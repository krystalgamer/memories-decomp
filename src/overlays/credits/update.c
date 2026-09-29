#include "../../types.h"
#include "../../game/graphics_frame.h"
#include "credits.h"

/* The brightness ramp of a fading line: 0 below 0, 0x80 above 0x80. */
#define CLAMP(x) ((x) < 0 ? 0 : (x) > 0x80 ? 0x80 : (x))

/* Advances both credits lines by one frame. State 0 fades a line in, and
 * state 1 holds it; a line whose first row is group 0x63 sets D_80182208
 * instead. State 2 fades the line out. State 3 keeps it blank, then lays out
 * its next page, or moves to state 4 when no page is left. State 4 starts the
 * following group in the first free line and releases this one. Returns
 * D_80182208. */
s32 func_80180A24(void)
{
    CreditsLine *other;
    s32 i;
    s32 j;
    s32 t;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 n;
    s32 k;
    s32 t2;
    s32 a2;
    s32 b2;
    s32 c2;
    s32 d2;

    for (i = 0; i < 2; i++) {
        if (D_8018220C[i].entries == 0) {
            continue;
        }
        switch (D_8018220C[i].state) {
        case 0:
            t = (0x80 - D_8018220C[i].timer) << 7;
            a = t / 32;
            b = t / 64;
            c = t / 96;
            d = t / 128;
            D_8018220C[i].timer -= D_8009B0D8 * 2;
            if (D_8018220C[i].timer <= 0) {
                D_8018220C[i].timer = 0x98;
                D_8018220C[i].state++;
            }
            a = CLAMP(a);
            b = CLAMP(b);
            c = CLAMP(c);
            d = CLAMP(d);
            func_8018173C(i, a, b, c, d);
            break;
        case 1:
            if (D_8018220C[i].entries->group == 0x63) {
                D_80182208 = 1;
            } else {
                D_8018220C[i].timer -= D_8009B0D8;
                if (D_8018220C[i].timer <= 0) {
                    D_8018220C[i].timer = 0x98;
                    D_8018220C[i].state++;
                    if (D_8018220C[i].field_13 != 0) {
                        if (D_8018220C[i].page == 1) {
                            D_8018220C[i].field_13 = 0;
                        }
                    } else {
                        D_8018220C[i].field_13++;
                    }
                }
            }
            func_8018173C(i, 0x80, 0x80, 0x80, 0x80);
            break;
        case 2:
            k = 0x80;
            t2 = (k - D_8018220C[i].timer) << 7;
            a2 = t2 / 32;
            b2 = t2 / 64;
            c2 = t2 / 96;
            d2 = t2 / 128;
            D_8018220C[i].timer -= D_8009B0D8 * 2;
            if (D_8018220C[i].timer <= 0) {
                if (D_8018220C[i].page >= 2) {
                    D_8018220C[i].timer = 0x3C;
                    D_8018220C[i].state++;
                } else {
                    D_8018220C[i].timer = 0x98;
                    D_8018220C[i].state = 4;
                }
            }
            d2 = CLAMP(d2);
            c2 = CLAMP(c2);
            b2 = CLAMP(b2);
            a2 = CLAMP(a2);
            func_8018173C(i, k - d2, k - c2, k - b2, k - a2);
            break;
        case 3:
            D_8018220C[i].timer -= D_8009B0D8 * 2;
            if (D_8018220C[i].timer <= 0) {
                D_8018220C[i].timer = 0x98;
                D_8018220C[i].state = 0;
                D_8018220C[i].page--;
                if (D_8018220C[i].page != 0) {
                    func_80180F58(i, D_8018220C[i].entries->group);
                } else {
                    D_8018220C[i].timer = 0x98;
                    D_8018220C[i].state = 4;
                }
            }
            func_8018173C(i, 0, 0, 0, 0);
            break;
        case 4:
            D_8018220C[i].timer -= D_8009B0D8 * 2;
            if (D_8018220C[i].timer <= 0) {
                n = D_8018220C[i].end->group;
                if (n >= 0) {
                    j = 0;
                    do {
                        other = D_8018220C;
                    } while (0);
                next:
                    if (other->entries == 0) {
                            other->height = 0;
                            other->width = 0;
                            other->page = 0;
                            if (func_80180F58(j, n) != 0) {
                                other->x = 40;
                                other->y = 40;
                                if (other->width > 160 || other->height > 120) {
                                    other->x = (320 - other->width) / 2;
                                    other->y = (240 - other->height) / 2 - 10;
                                }
                                other->state = 0;
                                other->timer = 152;
                                other->field_13 = 0;
                            }
                            goto done;
                    }
                    j++;
                    other++;
                    if (j < 2) {
                        goto next;
                    }
                }
            done:
                D_8018220C[i].entries = 0;
            }
            break;
        }
    }
    return D_80182208;
}
