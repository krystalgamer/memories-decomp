#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_10.h"
#include "live_handler_data_semantic_views.h"
#include "handler_semantic_views.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "primitive_semantic_views.h"
#include "drawing_semantic_views.h"
#include "texture_state_semantic_views.h"

void DuelEffect_RunTerrainChange(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    POLY_FT4 polygon;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR quad[4];
    POLY_FT4 *packet;
    DuelEffect10Work *work;
    s32 i;
    s32 j;
    s32 k;
    s32 frame_step;

    memset(&rotation, 0, sizeof(rotation));
    memset(&position, 0, sizeof(position));
    packet = &polygon;
    scale = gDuelEffect_TerrainChangeInitialScale;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 6) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_TerrainChangeConfigs[phase];
            for (i = 0; i < 2; i++) {
                DuelEffect_InitHalfStepXZRingVertices(175, 194, 16, (u16)(i * 8), work->rings[i], 4);
            }
            DuelEffect_InitXZRectVertices(work->plane, 175, 16, 194);
            DuelEffect_InitRandomVectorsXYZ(work->particles, 175, 16, 194, 64);
            for (i = 0; i < 64; i++) {
                work->speeds[i] = work->config->particle_speed / 2 +
                    work->config->particle_speed * (rand() % 4096) / 4096;
            }
            DuelEffect_InitRandomVectorsXYZ(work->columns, 139, 0, 182, 64);
            for (i = 0; i < 64; i++) {
                DuelEffect_SetGrayColor((u8 *)&work->column_colors[i], 1);
                work->ages[i] = 0;
            }
            DuelEffect_SetRgbColor((u8 *)&work->color, work->config->color.r >> 3,
                         work->config->color.g >> 3, work->config->color.b >> 3);
            DuelEffect_SetGrayColor((u8 *)&work->screen_color, 255);
            DuelEffect_SetGrayColor((u8 *)&work->ring_color, 1);
            DuelEffect_SetGrayColor((u8 *)&work->plane_color, 1);
            work->height = 8;
            work->stage = 0;
            work->ring_ready = 0;
            work->color_ready = 0;
            work->frame = 0;
            work->tick = 0;
            work->spawned = 0;
            if (work->config->mode != 1) {
                gDuelEffect_TexturePageClutPairs.pairs[12][0] = getTPage(gDuelEffect_TerrainChangeTextureImages[12].pmode,
                    work->config->mode, gDuelEffect_TerrainChangeTextureImages[12].px, gDuelEffect_TerrainChangeTextureImages[12].py);
            }
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            gDuelEffect_bRequestFinished = 1;
        }
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 2);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color) && work->stage == 0) {
            DuelEffect_DrawProjectedFlatQuad((u8 *)&work->color, work->plane, work->config->mode, 1);
            DuelEffect_DrawProjectedFlatQuad((u8 *)&work->plane_color, work->plane, work->config->mode, 1);
            if (work->config->mode == 1) {
                DuelEffect_StepColorToTarget((u8 *)&work->color, work->config->color.r,
                             work->config->color.g, work->config->color.b, 4);
                work->color_ready = DuelEffect_StepColorToTarget((u8 *)&work->plane_color, 192, 192, 192, 2);
            } else {
                work->color_ready = DuelEffect_StepColorToTarget((u8 *)&work->color,
                    work->config->color.r, work->config->color.g, work->config->color.b, 4);
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->ring_color) && work->stage == 0) {
            DuelEffect_DrawQuadContour((u8 *)&work->ring_color, work->rings[0], work->rings[1], 1, 1);
            if (work->height < work->config->maximum_height) {
                work->height += work->config->height_step;
                for (i = 0; i < 4; i++) {
                    work->rings[1][i].vy = -work->height;
                }
            }
            if (work->ring_ready == 0) {
                work->ring_ready = DuelEffect_StepColorToTarget((u8 *)&work->ring_color, 255, 255, 255, 8);
            }
        }
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color) && work->stage == 1) {
            for (i = 0; i < 64; i++) {
                DuelEffect_InitOffsetQuadVertices(quad, &work->particles[i]);
                DuelEffect_DrawRandomTileQuad((u8 *)&work->color, quad, 80);
                work->particles[i].vy -= work->speeds[i];
            }
            DuelEffect_DarkenColor((u8 *)&work->color, 8);
        }
        if (work->stage == 0) {
            for (i = 0; i < work->spawned; i++) {
                if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->column_colors[i])) {
                    setPolyFT4(packet);
                    packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[12][0];
                    packet->clut = gDuelEffect_TexturePageClutPairs.pairs[12][1];
                    setUV4(packet, 224, 192, 255, 192, 224, 223, 255, 223);
                    for (j = 0; j < 4; j++) {
                        if (!((work->tick + i + j) & 1)) {
                            setRGB0(packet, work->column_colors[i].r / (j + 1),
                                    work->column_colors[i].g / (j + 1),
                                    work->column_colors[i].b / (j + 1));
                        } else {
                            setRGB0(packet, 192 / (j + 1), 192 / (j + 1), 192 / (j + 1));
                        }
                        DuelEffect_InitRectVertices(quad, (s16)(12 - j), (s16)(24 - j));
                        for (k = 0; k < 4; k++) {
                            addVector(&quad[k], &work->columns[i]);
                            quad[k].vy += (j + 1) * 10;
                            quad[k].vy -= work->ages[i];
                        }
                        DuelEffect_ProjectSubmitFt4(packet, quad, 1, work->config->mode);
                    }
                    work->ages[i] += 12;
                    if (work->ages[i] > 160) {
                        DuelEffect_DarkenColor((u8 *)&work->column_colors[i], 15);
                        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->column_colors[i])) {
                            work->ages[i] = 0;
                            DuelEffect_SetGrayColor((u8 *)&work->column_colors[i], 1);
                        }
                    } else {
                        DuelEffect_StepColorToTarget((u8 *)&work->column_colors[i],
                            work->config->color.r >> 1, work->config->color.g >> 1,
                            work->config->color.b >> 1, 15);
                    }
                }
            }
            if (!(work->tick & 3)) {
                work->spawned++;
                if (work->spawned > 64) {
                    work->spawned = 64;
                }
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->screen_color) && work->stage == 1) {
            DuelEffect_DrawFadingFullscreenColor((u8 *)&work->screen_color, 15);
        }
        if (*(u32 *)&work->color_ready == 0x10001) {
            gDuelEffect_pActiveRequest->field_1D = 1;
        }
        if (work->stage == 0 && phase < -1) {
            if (work->config->mode == 2) {
                DuelEffect_SetGrayColor((u8 *)&work->color, 64);
            }
            work->stage = 1;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->screen_color) && work->stage == 1) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
