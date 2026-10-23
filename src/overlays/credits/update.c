#include "../../types.h"
#include "../../game/graphics_frame.h"
#include "credits.h"

/* The brightness ramp of a fading line: 0 below 0, 0x80 above 0x80. */
#define CLAMP(x) ((x) < 0 ? 0 : (x) > 0x80 ? 0x80 : (x))

/* Advances both credits lines by one frame. State 0 fades a line in, and
 * state 1 holds it; a line whose first row is group 0x63 sets gCredits_bFinished
 * instead. State 2 fades the line out. State 3 keeps it blank, then lays out
 * its next page, or moves to state 4 when no page is left. State 4 starts the
 * following group in the first free line and releases this one. Returns
 * gCredits_bFinished. */
s32 Credits_UpdateLines(void)
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
        if (gCredits_aLines[i].entries == 0) {
            continue;
        }
        switch (gCredits_aLines[i].state) {
        case 0:
            t = (0x80 - gCredits_aLines[i].timer) << 7;
            a = t / 32;
            b = t / 64;
            c = t / 96;
            d = t / 128;
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                gCredits_aLines[i].timer = 0x98;
                gCredits_aLines[i].state++;
            }
            a = CLAMP(a);
            b = CLAMP(b);
            c = CLAMP(c);
            d = CLAMP(d);
            Credits_DrawLine(i, a, b, c, d);
            break;
        case 1:
            if (gCredits_aLines[i].entries->group == 0x63) {
                gCredits_bFinished = 1;
            } else {
                gCredits_aLines[i].timer -= D_8009B0D8;
                if (gCredits_aLines[i].timer <= 0) {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state++;
                    if (gCredits_aLines[i].field_13 != 0) {
                        if (gCredits_aLines[i].page == 1) {
                            gCredits_aLines[i].field_13 = 0;
                        }
                    } else {
                        gCredits_aLines[i].field_13++;
                    }
                }
            }
            Credits_DrawLine(i, 0x80, 0x80, 0x80, 0x80);
            break;
        case 2:
            k = 0x80;
            t2 = (k - gCredits_aLines[i].timer) << 7;
            a2 = t2 / 32;
            b2 = t2 / 64;
            c2 = t2 / 96;
            d2 = t2 / 128;
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                if (gCredits_aLines[i].page >= 2) {
                    gCredits_aLines[i].timer = 0x3C;
                    gCredits_aLines[i].state++;
                } else {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state = 4;
                }
            }
            d2 = CLAMP(d2);
            c2 = CLAMP(c2);
            b2 = CLAMP(b2);
            a2 = CLAMP(a2);
            Credits_DrawLine(i, k - d2, k - c2, k - b2, k - a2);
            break;
        case 3:
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                gCredits_aLines[i].timer = 0x98;
                gCredits_aLines[i].state = 0;
                gCredits_aLines[i].page--;
                if (gCredits_aLines[i].page != 0) {
                    Credits_BuildLineGlyphs(i, gCredits_aLines[i].entries->group);
                } else {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state = 4;
                }
            }
            Credits_DrawLine(i, 0, 0, 0, 0);
            break;
        case 4:
            gCredits_aLines[i].timer -= D_8009B0D8 * 2;
            if (gCredits_aLines[i].timer <= 0) {
                n = gCredits_aLines[i].end->group;
                if (n >= 0) {
                    j = 0;
                    do {
                        other = gCredits_aLines;
                    } while (0);
                next:
                    if (other->entries == 0) {
                            other->height = 0;
                            other->width = 0;
                            other->page = 0;
                            if (Credits_BuildLineGlyphs(j, n) != 0) {
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
                gCredits_aLines[i].entries = 0;
            }
            break;
        }
    }
    return gCredits_bFinished;
}
