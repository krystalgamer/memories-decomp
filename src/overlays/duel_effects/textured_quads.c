#include "../../types.h"
#include "textured_quads.h"

void func_80156C40(u8 *color, u16 size, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = D_8015B748.named.page0;
    packet->polygon.clut = D_8015B748.named.clut0;
    setUV4(&packet->polygon, 192, 128, 255, 128, 192, 191, 255, 191);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    func_8014F358(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    func_80151218(&packet->polygon, drawing.vertices, 0, 1);
}

void func_80156D50(u16 size, u16 index, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = D_8015B748.named.page1;
    packet->polygon.clut = D_8015B748.named.clut1;
    setUV4(&packet->polygon, index * 32, 0, index * 32 + 31, 0,
           index * 32, 31, index * 32 + 31, 31);
    setRGB0(&packet->polygon, 128, 128, 128);
    func_8014F358(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    func_80151218(&packet->polygon, drawing.vertices, 1, 1);
}
