#include "../../types.h"
#include "renderers.h"
#include "helpers.h"
#include "../../game/sprite_primitive.h"

void PalOptions_DrawWaveSprite(DisplayObject *object, GsOT *ordering_table)
{
    SpritePrim *sprite;
    POLY_GT4 *packet;
    s32 row;
    s32 column;
    s32 x;
    s32 y;
    s32 color;

    SetPolyGT4((POLY_GT4 *)0x1F800344);
    sprite = (SpritePrim *)0x1F800320;
    sprite->attribute = object->attribute;
    sprite->xy.h.x = object->field_30.h.field_30;
    sprite->xy.h.y = object->field_30.h.field_32;
    sprite->extent.wh.w.word = object->field_3C.h.field_3C;
    sprite->extent.wh.h = object->field_3C.h.field_3E;
    sprite->rgb = object->rgb;
    sprite->cxcy.word = object->field_40.word;
    sprite->uv.word = object->field_5C;
    sprite->tpage = object->field_66;
    packet = (POLY_GT4 *)0x1F800344;
    if (gPalOptions_bLanguage == object->field_6A) {
        packet->tpage = sprite->tpage;
        packet->clut = getClut(sprite->cxcy.h.cx, sprite->cxcy.h.cy);
        for (row = 0; row < 4; row++) {
            x = (s16)sprite->xy.h.x;
            y = (s16)sprite->xy.h.y + row * 8;
            packet->u0 = sprite->uv.b.lo;
            packet->v0 = packet->v1 = sprite->uv.b.hi + row * 8;
            packet->v2 = packet->v3 = packet->v0 + 8;
            for (column = 0; column < 8; x += 6, column++) {
                packet->x0 = packet->x2 = x;
                packet->x1 = packet->x3 = x + 6;
                packet->u2 = packet->u0;
                packet->u1 = packet->u3 = packet->u0 + 6;
                packet->y0 = y + gPalOptions_aWaveOffsets[row][column];
                packet->y1 = y + gPalOptions_aWaveOffsets[row][column + 1];
                packet->y2 = y + (s16)(gPalOptions_aWaveOffsets[row + 1][column] + 8);
                packet->y3 = y + (s16)(gPalOptions_aWaveOffsets[row + 1][column + 1] + 8);
                color = gPalOptions_aWaveColors[row][column];
                packet->r0 = color;
                packet->g0 = color >> 8;
                packet->b0 = color >> 16;
                *(u32 *)&packet->r1 = gPalOptions_aWaveColors[row][column + 1];
                *(u32 *)&packet->r2 = gPalOptions_aWaveColors[row + 1][column];
                *(u32 *)&packet->r3 = gPalOptions_aWaveColors[row + 1][column + 1];
                GsSortPoly(packet, ordering_table, 0);
                packet->u0 += 6;
            }
        }
    } else {
        GsSortFastSprite((GsSPRITE *)sprite, ordering_table, 0);
    }
}
