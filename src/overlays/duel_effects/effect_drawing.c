#include "../../types.h"
#include "effect_drawing.h"
#include "../../game/gpu_packets.h"
#include "display_quads.h"
#include "drawing_tail.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "layered_semantic_views.h"
#include "texture_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "drawing_semantic_views.h"
#include "render_state_semantic_views.h"
#include "number_semantic_views.h"
#include "primitive_semantic_views.h"
#include "display_semantic_views.h"

void DuelEffect_DrawLayeredTileQuad(CVECTOR *color, u16 size, s16 depth, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_00;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_02;
    setUV4(&packet->polygon, 0, 0, 63, 0, 0, 63, 63, 63);
    setRGB0(&packet->polygon, color->r, color->g, color->b);
    DuelEffect_InitSquareVertices(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, depth, 1);
    setRGB0(&packet->polygon, color->r >> 1, color->g >> 1, color->b >> 1);
    DuelEffect_InitSquareVertices(drawing.vertices, size * 2);
    for (i = 0; i < 4; i++) {
        addVector(&drawing.vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, depth, 1);
}

void DuelEffect_DrawLayeredGradientTrapezoid(CVECTOR *color, u16 *widths, u16 height, s16 depth)
{
    DuelEffectGradientQuad drawing;
    DuelEffectGradientQuad *record = &drawing;
    POLY_GT4 *packet = &record->polygon;
    s32 i;

    setPolyGT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_04;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_06;
    setUV4(packet, 128, 0, 159, 0, 128, 63, 159, 63);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, 0, 0, 0);
    setRGB2(packet, color->r >> 1, color->g >> 1, color->b >> 1);
    setRGB3(packet, color->r >> 1, color->g >> 1, color->b >> 1);
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
    DuelEffect_ProjectSubmitGt4(packet, drawing.vertices, depth, 1);
    setRGB2(&drawing.polygon, color->r >> 2, color->g >> 2, color->b >> 2);
    setRGB3(&drawing.polygon, color->r >> 2, color->g >> 2, color->b >> 2);
    for (i = 0; i < 4; i++) {
        applyVector(&record->vertices[i], 2, 2, 0, *=);
    }
    DuelEffect_ProjectSubmitGt4(packet, drawing.vertices, depth, 1);
}

void DuelEffect_DrawFixedTileQuad(CVECTOR *color)
{
    POLY_FT4 polygon;
    POLY_FT4 *packet = &polygon;
    SVECTOR vertices[4];

    setPolyFT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_08;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_0A;
    setUV4(packet, 0, 64, 63, 64, 0, 127, 63, 127);
    DuelEffect_InitSquareVertices(vertices, 64);
    setRGB0(packet, color->r >> 1, color->g >> 1, color->b >> 1);
    DuelEffect_ProjectSubmitFt4(packet, vertices, 0, 1);
}

void DuelEffect_DrawRandomTileQuad(CVECTOR *color, SVECTOR *vertices, s16 depth)
{
    POLY_FT4 polygon;
    POLY_FT4 *packet = &polygon;
    s32 u;
    s32 v;

    setPolyFT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_0C;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_0E;
    setRGB0(packet, color->r, color->g, color->b);
    u = rand() % 2;
    v = rand() % 2;
    setUV4(packet, 160 + u * 32, v * 32, 191 + u * 32, v * 32,
           160 + u * 32, 31 + v * 32, 191 + u * 32, 31 + v * 32);
    DuelEffect_ProjectSubmitFt4(packet, vertices, depth, 1);
}

void DuelEffect_DrawTexturedRingStrip(CVECTOR *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, s16 mode)
{
    DisplayObjectGT4Packet polygon;
    DisplayObjectGT4Packet *packet = &polygon;
    long p;
    long flag;
    s32 depth;
    s32 adjusted;
    s32 i;

    setPolyGT4(&packet->prim);
    packet->prim.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_04;
    packet->prim.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_06;
    setUV4(&packet->prim, 128, 0, 159, 0, 128, 63, 159, 63);
    setRGB0(&packet->prim, 0, 0, 0);
    setRGB1(&packet->prim, 0, 0, 0);
    setRGB2(&packet->prim, color->r, color->g, color->b);
    setRGB3(&packet->prim, color->r, color->g, color->b);
    for (i = 0; i < 32; i++) {
        depth = RotAverage4(&first[i], &first[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            &packet->w.xy0, &packet->w.xy1,
            &packet->w.xy2, &packet->w.xy3,
            &p, &flag);
        if (depth >= 0 && flag >= 0) {
            adjusted = depth + 1;
            if (mode == 0) {
                DuelEffect_SubmitOffsetGt4(&packet->prim, 1);
            } else {
                depth = adjusted - mode;
                if (depth < 0) {
                    depth = 0;
                }
                Graphics_SubmitDrawModePacket(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
            }
        }
        depth = RotAverage4(&last[i], &last[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            &packet->w.xy0, &packet->w.xy1,
            &packet->w.xy2, &packet->w.xy3,
            &p, &flag);
        if (depth >= 0 && flag >= 0) {
            adjusted = depth + 1;
            if (mode == 0) {
                DuelEffect_SubmitOffsetGt4(&packet->prim, 1);
            } else {
                depth = adjusted - mode;
                if (depth < 0) {
                    depth = 0;
                }
                Graphics_SubmitDrawModePacket(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
            }
        }
    }
}

void DuelEffect_DrawGradientRingStrip(CVECTOR *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, u16 bias)
{
    DisplayObjectG4Packet polygon;
    DisplayObjectG4Packet *packet = &polygon;
    long p;
    long flag;
    s32 depth;
    s32 i;

    setPolyG4(&packet->prim);
    setRGB0(&packet->prim, 0, 0, 0);
    setRGB1(&packet->prim, 0, 0, 0);
    setRGB2(&packet->prim, color->r >> 1, color->g >> 1, color->b >> 1);
    setRGB3(&polygon.prim, color->r >> 1, color->g >> 1, color->b >> 1);
    for (i = 0; i < 32; i++) {
        depth = RotAverage4(&first[i], &first[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            &packet->w.xy0, &packet->w.xy1,
            &packet->w.xy2, &packet->w.xy3,
            &p, &flag);
        if (flag >= 0) {
            depth -= bias;
            if (bias == 0) {
                DuelEffect_SubmitOffsetQuad(&packet->prim, 1);
            } else {
                if (depth < 0) {
                    depth = 0;
                }
                Graphics_SubmitDrawModePacket(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2), 1);
            }
        }
        depth = RotAverage4(&last[i], &last[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            &packet->w.xy0, &packet->w.xy1,
            &packet->w.xy2, &packet->w.xy3,
            &p, &flag);
        if (flag >= 0) {
            depth -= bias;
            if (bias == 0) {
                DuelEffect_SubmitOffsetQuad(&packet->prim, 1);
            } else {
                if (depth < 0) {
                    depth = 0;
                }
                Graphics_SubmitDrawModePacket(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2), 1);
            }
        }
    }
}

void DuelEffect_DrawProjectedNumber(s32 value, CVECTOR *color, SVECTOR *offset, u16 mode, u16 size, u16 bias)
{
    SVECTOR vertices[20];
    u16 digits[8];
    DisplayObjectFT4Packet polygon;
    POLY_FT4 copy;
    DisplayObjectFT4Packet *packet = &polygon;
    long p;
    long flag;
    s32 depth;
    s32 adjusted;
    s32 i;
    u16 count;
    u16 vertex;
    u16 draw_mode;
    POLY_FT4 *draw_packet;
    u16 tile;

    count = DuelEffect_CountDecimalDigitsS16(value);
    DuelEffect_WriteNumberDigits(__builtin_abs(value), digits);
    vertex = 0;
    setPolyFT4(&packet->prim);
    setRGB0(&packet->prim, color->r, color->g, color->b);
    for (i = 0; i < count * 2 + 4; i++) {
        setVector(&vertices[i],
            size * (i / 2 << 1)
                - ((count + 1) / 2 * size * 2 + size * ((count + 1) % 2)),
            (s16)DuelEffect_PowS16(-1, i + 1) * size, 0);
        addVector(&vertices[i], offset);
    }
    for (i = 0; i < count + 1; i++) {
        packet->prim.tpage = gDuelEffect_TexturePageClutPairs.pairs[9][0];
        packet->prim.clut = gDuelEffect_TexturePageClutPairs.pairs[9][1];
        depth = RotAverage4(&vertices[vertex], &vertices[vertex + 2],
            &vertices[vertex + 1], &vertices[vertex + 3],
            &packet->w.xy0, &packet->w.xy1,
            &packet->w.xy2, &packet->w.xy3,
            &p, &flag);
        if (i == 0) {
            if (value > 0) {
                tile = 10;
            } else {
                tile = 11;
            }
        } else {
            tile = digits[i - 1];
        }
        setUV4(&packet->prim,
            gDuelEffect_aNumberGlyphRects[tile].x, gDuelEffect_aNumberGlyphRects[tile].y,
            gDuelEffect_aNumberGlyphRects[tile].x + gDuelEffect_aNumberGlyphRects[tile].w, gDuelEffect_aNumberGlyphRects[tile].y,
            gDuelEffect_aNumberGlyphRects[tile].x, gDuelEffect_aNumberGlyphRects[tile].y + gDuelEffect_aNumberGlyphRects[tile].h,
            gDuelEffect_aNumberGlyphRects[tile].x + gDuelEffect_aNumberGlyphRects[tile].w,
            gDuelEffect_aNumberGlyphRects[tile].y + gDuelEffect_aNumberGlyphRects[tile].h);
        copy = polygon.prim;
        if (bias == 0) {
            if (flag >= 0) {
                if (mode == 1) {
                    draw_packet = &packet->prim;
                    draw_mode = 1;
                } else if (mode == 0) {
                    draw_packet = &packet->prim;
                    draw_mode = 0;
                } else {
                    vertex += 2;
                    continue;
                }
                DuelEffect_SubmitOffsetFt4(draw_packet, draw_mode);
            }
        } else {
            adjusted = depth + 1;
            depth = adjusted - bias;
            if (depth >= 0 && flag >= 0) {
                if (mode == 1) {
                    Graphics_SubmitDrawModePacket(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2), 1);
                } else {
                    Graphics_SubmitDrawModePacket(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2), 0);
                }
            }
        }
        vertex += 2;
    }
}

u16 DuelEffect_CountDecimalDigitsS16(s16 value)
{
    u16 count = 0;
    while (__builtin_abs(value) > 0) {
        value /= 10;
        count++;
    }
    return count;
}

void DuelEffect_WriteNumberDigits(u16 value, u16 *digits)
{
    s16 original = value;
    s32 i;
    for (i = 0; i < DuelEffect_CountDecimalDigitsS16(original); i++) {
        digits[i] = value / (s16)DuelEffect_PowS16(10, DuelEffect_CountDecimalDigitsS16(original) - (i + 1));
        value -= digits[i] * (s16)DuelEffect_PowS16(10, DuelEffect_CountDecimalDigitsS16(original) - (i + 1));
    }
}

void DuelEffect_DrawFixedTextureQuad(CVECTOR *color, u16 size, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page0;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut0;
    setUV4(&packet->polygon, 192, 128, 255, 128, 192, 191, 255, 191);
    setRGB0(&packet->polygon, color->r, color->g, color->b);
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

void DuelEffect_DrawSharedBaseTriangles(CVECTOR *color, u16 width, SVECTOR *positions, u16 count, s16 bias)
{
    SVECTOR left;
    SVECTOR right;
    DuelEffectG3Packet polygon;
    DuelEffectG3Packet *packet = &polygon;
    long p;
    long flag;
    s32 i;
    s32 depth;

    setPolyG3(&packet->prim);
    setRGB0(&packet->prim, 0, 0, 0);
    setRGB1(&packet->prim, color->r, color->g, color->b);
    setRGB2(&packet->prim, color->r, color->g, color->b);
    left.vx = -width;
    left.vy = 0;
    left.vz = 0;
    right.vx = width;
    right.vy = 0;
    right.vz = 0;
    for (i = 0; i < count; i++) {
        depth = RotAverage3(&positions[i], &left, &right,
            &packet->w.xy0, &packet->w.xy1, &packet->w.xy2,
            &p, &flag) - bias;
        Graphics_SubmitDrawModePacket(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2), 1);
    }
}

void DuelEffect_DrawProjectedFlatQuad(CVECTOR *color, SVECTOR *vertices, u16 flags, u16 mode)
{
    DuelEffectF4Packet polygon;
    POLY_F4 *packet = &polygon.prim;
    long p;
    long flag;
    s32 depth;

    setPolyF4(packet);
    setRGB0(packet, color->r, color->g, color->b);
    setSemiTrans(packet, 1);
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
        &polygon.w.xy0, &polygon.w.xy1, &polygon.w.xy2,
        &polygon.w.xy3, &p, &flag);
    if (flag >= 0) {
        if (mode == 0) {
            setSemiTrans(packet, 0);
            GsSortPoly(packet, gDuelEffect_pOrderingTable, depth >> 2);
        } else {
            Graphics_SubmitDrawModePacket(packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), flags);
        }
    }
}

u16 DuelEffect_DrawGradientPath(CVECTOR *color, u16 count, SVECTOR *vertices, u16 bias,
                    u16 mode, SVECTOR *output, u16 flags, SVECTOR *offset)
{
    DuelEffectGLinePacket line;
    DuelEffectGLinePacket *packet = &line;
    long p;
    long flag;
    s32 depth;
    s32 divisor;
    s32 i;

    packet->prim.attribute = 0x40000000U + ((u32)flags << 28);
    for (i = 0; i < count; i++) {
        divisor = count + 1;
        setRGB0(&packet->prim, color->r / (divisor - i),
            color->g / (divisor - i), color->b / (divisor - i));
        setRGB1(&packet->prim, color->r / (count - i),
            color->g / (count - i), color->b / (count - i));
        RotTransPers(&vertices[i], &packet->w.xy0, &p, &flag);
        depth = RotTransPers(&vertices[i + 1], &packet->w.xy1,
                            &p, &flag);
        depth -= bias;
        if (depth >= 0) {
            GsSortGLine(&packet->prim, gDuelEffect_pOrderingTable, (u16)(depth >> 2));
        }
        if (mode == 1 && vertices[i + 1].vy > 0) {
            setVector(output, vertices[i + 1].vx, 0, vertices[i + 1].vz);
            addVector(output, offset);
            mode = 2;
        }
    }
    return mode;
}

void DuelEffect_DrawQuadrantTileQuads(CVECTOR *color, u16 size, s16 depth)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_1C;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_1E;
    setUV4(&packet->polygon, 128, 128, 191, 128, 128, 191, 191, 191);
    setRGB0(&packet->polygon, color->r, color->g, color->b);
    for (i = 0; i < 4; i++) {
        DuelEffect_InitQuadrantQuadVertices(packet->vertices, size, i);
        DuelEffect_ProjectSubmitFt4(&packet->polygon, packet->vertices, depth, 1);
    }
}

void DuelEffect_DrawTranslatedTileRect(CVECTOR *color, SVECTOR *offset, u16 width, u16 height, u16 tile)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_20;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_22;
    setUV4(&packet->polygon, 128 + tile * 32, 192, 159 + tile * 32, 192,
           128 + tile * 32, 255, 159 + tile * 32, 255);
    setRGB0(&packet->polygon, color->r, color->g, color->b);
    setVector(&drawing.vertices[0], -width, -height, 0);
    setVector(&drawing.vertices[1], width, -height, 0);
    setVector(&drawing.vertices[2], -width, 0, 0);
    setVector(&drawing.vertices[3], width, 0, 0);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, 1, 1);
}

void DuelEffect_DrawGradientConnectionBands(CVECTOR *color, SVECTOR *first, SVECTOR *last, u16 width)
{
    DisplayObjectG4Packet polygon;
    DisplayObjectG4Packet *packet = &polygon;
    SVECTOR vertices[4];
    long p;
    long flag;
    s32 i;

    setPolyG4(&packet->prim);
    setRGB0(&packet->prim, 0, 0, 0);
    setRGB1(&packet->prim, color->r, color->g, color->b);
    setRGB2(&packet->prim, color->r, color->g, color->b);
    setRGB3(&packet->prim, 0, 0, 0);
    for (i = 0; i < 2; i++) {
        setVector(&vertices[0], first->vx,
            first->vy + (s16)DuelEffect_PowS16(-1, i + 1) * width, 0);
        setVector(&vertices[1], first->vx, first->vy, 0);
        setVector(&vertices[2], last->vx, last->vy, 0);
        setVector(&vertices[3], last->vx,
            last->vy + (s16)DuelEffect_PowS16(-1, i + 1) * width, 0);
        RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            &packet->w.xy0, &packet->w.xy1,
            &packet->w.xy2, &packet->w.xy3,
            &p, &flag);
        if (flag >= 0) {
            DuelEffect_SubmitOffsetQuad(&packet->prim, 1);
        }
    }
}
