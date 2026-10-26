#include "../../types.h"
#include "textured_quads.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "texture_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "../../game/gpu_packets.h"
#include "drawing_helpers.h"
#include "primitive_semantic_views.h"
#include "render_state_semantic_views.h"
#include "display_quads.h"
#include "number_semantic_views.h"
#include "display_semantic_views.h"

void DuelEffect_DrawFixedTextureQuad(u8 *color, u16 size, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page0;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut0;
    setUV4(&packet->polygon, 192, 128, 255, 128, 192, 191, 255, 191);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    DuelEffect_InitSquareVertices(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, 0, 1);
}

void DuelEffect_DrawAtlasColumnQuad(u16 size, u16 index, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page1;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut1;
    setUV4(&packet->polygon, index * 32, 0, index * 32 + 31, 0,
           index * 32, 31, index * 32 + 31, 31);
    setRGB0(&packet->polygon, 128, 128, 128);
    DuelEffect_InitSquareVertices(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, 1, 1);
}

void DuelEffect_DrawSharedBaseTriangles(u8 *color, u16 width, SVECTOR *positions, u16 count, s16 bias)
{
    SVECTOR left;
    SVECTOR right;
    POLY_G3 polygon;
    POLY_G3 *packet = &polygon;
    s32 p;
    s32 flag;
    s32 i;
    s32 depth;

    setPolyG3(packet);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, color[0], color[1], color[2]);
    setRGB2(packet, color[0], color[1], color[2]);
    left.vx = -width;
    left.vy = 0;
    left.vz = 0;
    right.vx = width;
    right.vy = 0;
    right.vz = 0;
    for (i = 0; i < count; i++) {
        depth = RotAverage3(&positions[i], &left, &right,
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
            (PSXLONG *)&p, (PSXLONG *)&flag) - bias;
        func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), 1);
    }
}

void DuelEffect_DrawProjectedFlatQuad(u8 *color, SVECTOR *vertices, u16 flags, u16 mode)
{
    POLY_F4 polygon;
    POLY_F4 *packet = &polygon;
    s32 p;
    s32 flag;
    s32 depth;

    setPolyF4(packet);
    setRGB0(packet, color[0], color[1], color[2]);
    setSemiTrans(packet, 1);
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
        (PSXLONG *)&packet->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
    if (flag >= 0) {
        if (mode == 0) {
            setSemiTrans(packet, 0);
            GsSortPoly(packet, gDuelEffect_pOrderingTable, depth >> 2);
        } else {
            func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), flags);
        }
    }
}

u16 DuelEffect_DrawGradientPath(u8 *color, u16 count, SVECTOR *vertices, u16 bias,
                    u16 mode, SVECTOR *output, u16 flags, SVECTOR *offset)
{
    GsGLINE line;
    GsGLINE *packet = &line;
    s32 p;
    s32 flag;
    s32 depth;
    s32 divisor;
    s32 i;

    packet->attribute = 0x40000000U + ((u32)flags << 28);
    for (i = 0; i < count; i++) {
        divisor = count + 1;
        setRGB0(packet, color[0] / (divisor - i),
            color[1] / (divisor - i), color[2] / (divisor - i));
        setRGB1(packet, color[0] / (count - i),
            color[1] / (count - i), color[2] / (count - i));
        RotTransPers(&vertices[i], (PSXLONG *)&packet->x0, (PSXLONG *)&p, (PSXLONG *)&flag);
        depth = RotTransPers(&vertices[i + 1], (PSXLONG *)&packet->x1,
                            (PSXLONG *)&p, (PSXLONG *)&flag);
        depth -= bias;
        if (depth >= 0) {
            GsSortGLine(packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2));
        }
        if (mode == 1 && vertices[i + 1].vy > 0) {
            setVector(output, vertices[i + 1].vx, 0, vertices[i + 1].vz);
            addVector(output, offset);
            mode = 2;
        }
    }
    return mode;
}

void DuelEffect_DrawQuadrantTileQuads(u8 *color, u16 size, s16 depth)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_1C;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_1E;
    setUV4(&packet->polygon, 128, 128, 191, 128, 128, 191, 191, 191);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    for (i = 0; i < 4; i++) {
        DuelEffect_InitQuadrantQuadVertices(packet->vertices, size, i);
        DuelEffect_ProjectSubmitFt4(&packet->polygon, packet->vertices, depth, 1);
    }
}

void DuelEffect_DrawTranslatedTileRect(u8 *color, SVECTOR *offset, u16 width, u16 height, u16 tile)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_20;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_22;
    setUV4(&packet->polygon, 128 + tile * 32, 192, 159 + tile * 32, 192,
           128 + tile * 32, 255, 159 + tile * 32, 255);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    setVector(&drawing.vertices[0], -width, -height, 0);
    setVector(&drawing.vertices[1], width, -height, 0);
    setVector(&drawing.vertices[2], -width, 0, 0);
    setVector(&drawing.vertices[3], width, 0, 0);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, 1, 1);
}

void DuelEffect_DrawGradientConnectionBands(u8 *color, DVECTOR *first, DVECTOR *last, u16 width)
{
    POLY_G4 polygon;
    POLY_G4 *packet = &polygon;
    SVECTOR vertices[4];
    s32 p;
    s32 flag;
    s32 i;

    setPolyG4(packet);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, color[0], color[1], color[2]);
    setRGB2(packet, color[0], color[1], color[2]);
    setRGB3(packet, 0, 0, 0);
    for (i = 0; i < 2; i++) {
        setVector(&vertices[0], first->vx,
            first->vy + (s16)DuelEffect_PowS16(-1, i + 1) * width, 0);
        setVector(&vertices[1], first->vx, first->vy, 0);
        setVector(&vertices[2], last->vx, last->vy, 0);
        setVector(&vertices[3], last->vx,
            last->vy + (s16)DuelEffect_PowS16(-1, i + 1) * width, 0);
        RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
            (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (flag >= 0) {
            DuelEffect_SubmitOffsetQuad(packet, 1);
        }
    }
}
