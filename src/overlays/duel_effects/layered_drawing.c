#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "layered_drawing.h"

void func_801558F4(u8 *color, SVECTOR *inner, SVECTOR *outer, u16 bias, u16 flags)
{
    POLY_G4 gradient;
    POLY_G4 *edge = &gradient;
    POLY_FT4 polygon;
    POLY_FT4 *packet = &polygon;
    SVECTOR vertices[4];
    s32 p;
    s32 flag;
    s32 depth;
    s32 i;
    s32 j;

    setPolyG4(edge);
    setRGB0(edge, 0, 0, 0);
    setRGB1(edge, 0, 0, 0);
    setRGB2(edge, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    setRGB3(&gradient, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    setSemiTrans(edge, 1);
    setPolyFT4(packet);
    packet->tpage = D_8015B748.named.page_at_00;
    packet->clut = D_8015B748.named.clut_at_02;
    setUV4(packet, 0, 0, 63, 0, 0, 63, 63, 63);
    setRGB0(packet, color[0], color[1], color[2]);
    setSemiTrans(packet, 1);
    for (i = 0; i < 4; i++) {
        depth = RotAverage4(&outer[i], &outer[(i + 1) % 4],
            &inner[i], &inner[(i + 1) % 4],
            (long *)&edge->x0, (long *)&edge->x1,
            (long *)&edge->x2, (long *)&edge->x3,
            (long *)&p, (long *)&flag);
        if (flag >= 0) {
            depth++;
            if (bias == 0) {
                func_80152EC4(edge, flags);
            } else {
                depth -= bias;
                func_8005B260((u32 *)edge, D_8015B7F4,
                    (u16)(depth >> 2), flags);
            }
        }
        if (bias == 0) {
            func_8014F358(vertices, 8);
        } else {
            func_8014F358(vertices, 16);
        }
        for (j = 0; j < 4; j++) {
            addVector(&vertices[j], &inner[i]);
        }
        func_80151218(packet, vertices, (s16)bias, flags);
    }
}

void func_80155BC0(u8 *color, u16 size, s16 depth, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = D_8015B748.named.page_at_00;
    packet->polygon.clut = D_8015B748.named.clut_at_02;
    setUV4(&packet->polygon, 0, 0, 63, 0, 0, 63, 63, 63);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    func_8014F358(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    func_80151218(&packet->polygon, drawing.vertices, depth, 1);
    setRGB0(&packet->polygon, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    func_8014F358(drawing.vertices, size * 2);
    for (i = 0; i < 4; i++) {
        addVector(&drawing.vertices[i], offset);
    }
    func_80151218(&packet->polygon, drawing.vertices, depth, 1);
}

void func_80155D90(u8 *color, u16 *widths, s16 height, s16 depth)
{
    DuelEffectGradientQuad drawing;
    DuelEffectGradientQuad *record = &drawing;
    POLY_GT4 *packet = &record->polygon;
    s32 i;

    setPolyGT4(packet);
    packet->tpage = D_8015B748.named.page_at_04;
    packet->clut = D_8015B748.named.clut_at_06;
    setUV4(packet, 128, 0, 159, 0, 128, 63, 159, 63);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, 0, 0, 0);
    setRGB2(packet, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    setRGB3(packet, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    drawing.vertices[0].vx = -widths[0];
    drawing.vertices[0].vy = -height;
    drawing.vertices[0].vz = 0;
    drawing.vertices[1].vx = widths[0];
    drawing.vertices[1].vy = -height;
    drawing.vertices[1].vz = 0;
    drawing.vertices[2].vx = -widths[1];
    drawing.vertices[2].vy = 0;
    drawing.vertices[2].vz = 0;
    drawing.vertices[3].vx = widths[1];
    drawing.vertices[3].vy = 0;
    drawing.vertices[3].vz = 0;
    func_8015131C(packet, drawing.vertices, depth, 1);
    setRGB2(&drawing.polygon, color[0] >> 2, color[1] >> 2, color[2] >> 2);
    setRGB3(&drawing.polygon, color[0] >> 2, color[1] >> 2, color[2] >> 2);
    for (i = 0; i < 4; i++) {
        applyVector(&record->vertices[i], 2, 2, 0, *=);
    }
    func_8015131C(packet, drawing.vertices, depth, 1);
}
