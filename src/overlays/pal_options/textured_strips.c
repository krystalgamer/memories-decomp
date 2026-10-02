#include "../../types.h"
#include "renderers.h"
#include "helpers.h"

void func_80168100(DisplayObject *object, GsOT *ordering_table)
{
    POLY_FT4 *packet = (POLY_FT4 *)0x1F800344;
    s32 index = 0;
    s32 position;
    s32 start;

    setPolyFT4(packet);
    packet->r0 = packet->g0 = packet->b0 = 0x80;
    start = (s16)(D_80169050 % 160);
    packet->tpage = 0x11;
    packet->x0 = packet->x2 = 0x30;
    packet->x1 = packet->x3 = 0x60;
    position = start + 60;
    start = position;
    for (;;) {
        packet->y0 = packet->y1 = func_801680AC(position);
        position += 32;
        packet->y2 = packet->y3 = func_801680AC(position);
        if (packet->y0 >= 152) {
            break;
        }
        packet->clut = getClut(0x280 + D_80169040[index][2], 0xE0);
        packet->u0 = D_80169040[index][0];
        packet->v0 = D_80169040[index][1];
        packet->u1 = D_80169040[index][0] + 0x30;
        packet->v1 = D_80169040[index][1];
        packet->u2 = D_80169040[index][0];
        packet->v2 = D_80169040[index][1] + 0x20;
        packet->u3 = D_80169040[index][0] + 0x30;
        packet->v3 = D_80169040[index][1] + 0x20;
        GsSortPoly(packet, ordering_table, object->field_14);
        index++;
        if (index >= 5) {
            index = 0;
        }
    }
    index = 4;
    position = start - 32;
    for (;;) {
        packet->y0 = packet->y1 = func_801680AC(position);
        packet->y2 = packet->y3 = func_801680AC(position + 32);
        if (packet->y2 < 8) {
            break;
        }
        packet->clut = getClut(0x280 + D_80169040[index][2], 0xE0);
        packet->u0 = D_80169040[index][0];
        packet->v0 = D_80169040[index][1];
        packet->u1 = D_80169040[index][0] + 0x30;
        packet->v1 = D_80169040[index][1];
        packet->u2 = D_80169040[index][0];
        packet->v2 = D_80169040[index][1] + 0x20;
        packet->u3 = D_80169040[index][0] + 0x30;
        packet->v3 = D_80169040[index][1] + 0x20;
        GsSortPoly(packet, ordering_table, object->field_14);
        index--;
        if (index < 0) {
            index = 4;
        }
        position -= 32;
    }
}
