#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_24.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "layered_semantic_views.h"
#include "display_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "exodia_semantic_views.h"
#include "../../game/duel_effect_object_pool.h"

const VECTOR gDuelEffect_ExodiaInitialScale = { ONE, ONE, ONE, 0 };

void DuelEffect_RunExodiaBurst(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    DuelEffectGLinePacket line;
    SVECTOR vertices[4];
    CVECTOR color;
    POLY_FT4 *packet;
    DuelEffectGLinePacket *line_packet;
    DuelEffect24Work *work;
    s32 i;
    s32 x;
    s32 y;
    s32 z;
    long p;
    long flag;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_ExodiaInitialScale;
    packet = &polygon;
    line_packet = &line;
    work = buffer;
    if (phase >= 0) {
        DisplayObject_CopyWorkSlots((s32 *)gDuelEffect_CardObjectScratchWords);
        work->center.vx = (gDuelEffect_CardObjectScratchWords[0]->xy.s.x +
                           gDuelEffect_CardObjectScratchWords[1]->xy.s.x +
                           gDuelEffect_CardObjectScratchWords[2]->xy.s.x +
                           gDuelEffect_CardObjectScratchWords[3]->xy.s.x +
                           gDuelEffect_CardObjectScratchWords[4]->xy.s.x + 120) / 5;
        work->center.vy = (gDuelEffect_CardObjectScratchWords[0]->xy.s.y +
                           gDuelEffect_CardObjectScratchWords[1]->xy.s.y +
                           gDuelEffect_CardObjectScratchWords[2]->xy.s.y +
                           gDuelEffect_CardObjectScratchWords[3]->xy.s.y +
                           gDuelEffect_CardObjectScratchWords[4]->xy.s.y + 140) / 5;
        work->center.vz = 0;
        work->half_width = (gDuelEffect_CardObjectScratchWords[1]->xy.s.x -
                            gDuelEffect_CardObjectScratchWords[4]->xy.s.x) / 2;
        work->half_height = (gDuelEffect_CardObjectScratchWords[2]->xy.s.y -
                             gDuelEffect_CardObjectScratchWords[0]->xy.s.y) / 2;
        for (i = 0; i < 5; i++) {
            setVector(&work->targets[i],
                      gDuelEffect_CardObjectScratchWords[i]->xy.s.x - (s16)(work->center.vx - 24),
                      gDuelEffect_CardObjectScratchWords[i]->xy.s.y - (s16)(work->center.vy - 28), 0);
            setVector(&work->points[i], 0, 0, 0);
            setVector(&work->point_steps[i], work->targets[i].vx / 8, work->targets[i].vy / 8, 0);
        }
        for (i = 0; i < 64; i++) {
            x = (rand() - rand()) % 4096;
            y = (rand() - rand()) % 4096;
            z = (rand() - rand()) % 4096;
            setVector(&work->heads[i], x * 96 / 4096, y * 96 / 4096, z * 96 / 4096);
            setVector(&work->tails[i], x * 128 / 4096, y * 128 / 4096, z * 128 / 4096);
            setVector(&work->velocities[i], -work->tails[i].vx / 8,
                      -work->tails[i].vy / 8, -work->tails[i].vz / 8);
        }
        DuelEffect_InitRandomVectorsUnscaled(32, work->rotations);
        for (i = 0; i < 32; i++) {
            work->ray_scales[i] = 4096;
        }
        work->widths[0] = 2;
        work->widths[1] = 1;
        work->scale = 4096;
        work->tick = 0;
        work->arrival = 0;
        DuelEffect_SetGrayColor(&work->center_color, 1);
        DuelEffect_SetGrayColor(&work->flash_color, 255);
        DuelEffect_SetRgbColor(&work->line_color, 255, 128, 128);
        work->beam_width = 1;
        work->connection_tick = 0;
        work->line_count = 0;
        work->ray_count = 0;
        work->stage = 0;
    } else {
        Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        copyVector(&gDuelEffect_DrawOffset, &work->center);
        PushMatrix();
        DuelEffect_InitBaseMatrix(&world);
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
        setPolyFT4(packet);
        packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[18][0];
        packet->clut = gDuelEffect_TexturePageClutPairs.pairs[18][1];
        if ((u16)DuelEffect_HasNonzeroColor(&work->center_color)) {
            setRGB0(packet, work->center_color.r, work->center_color.g, work->center_color.b);
            setUV4(packet, 192, 128, 255, 128, 192, 255, 255, 255);
            for (i = 0; i < 2; i++) {
                setVector(&vertices[0], work->half_width * DuelEffect_PowS16(-1, i + 1), -work->half_height, 0);
                setVector(&vertices[1], 0, -work->half_height, 0);
                setVector(&vertices[2], work->half_width * DuelEffect_PowS16(-1, i + 1), work->half_height, 0);
                setVector(&vertices[3], 0, work->half_height, 0);
                DuelEffect_ProjectSubmitFt4(packet, vertices, 0, 1);
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor(&work->line_color)) {
            if (!(work->tick & 1)) {
                color = work->line_color;
                color.r >>= 1;
                color.g >>= 1;
                color.b >>= 1;
            } else {
                DuelEffect_SetGrayColor(&color, 255);
            }
            if (work->arrival == 8) {
                if (work->connection_tick > 0) {
                    DuelEffect_DrawGradientConnectionBands(&color, &work->targets[0],
                                 &work->targets[2], work->beam_width * 2);
                }
                if (work->connection_tick > 4) {
                    DuelEffect_DrawGradientConnectionBands(&color, &work->targets[2],
                                 &work->targets[4], work->beam_width);
                }
                if (work->connection_tick > 8) {
                    DuelEffect_DrawGradientConnectionBands(&color, &work->targets[1],
                                 &work->targets[4], work->beam_width);
                }
                if (work->connection_tick > 12) {
                    DuelEffect_DrawGradientConnectionBands(&color, &work->targets[1],
                                 &work->targets[3], work->beam_width);
                }
                if (work->connection_tick > 16) {
                    DuelEffect_DrawGradientConnectionBands(&color, &work->targets[0],
                                 &work->targets[3], work->beam_width * 2);
                }
                work->connection_tick++;
                if (work->connection_tick > 24) {
                    work->connection_tick = 24;
                    work->beam_width++;
                    if (work->beam_width > 8) {
                        work->beam_width = 8;
                        if (work->stage == 0) {
                            work->stage = 1;
                        }
                        DuelEffect_StepColorToTarget(&work->center_color, 255, 255, 255, 1);
                    }
                }
            }
            if (!(work->tick & 1)) {
                color = work->line_color;
            } else {
                DuelEffect_SetGrayColor(&color, 255);
            }
            for (i = 0; i < 5; i++) {
                DuelEffect_DrawLayeredTileQuad(&color, 16, 0, &work->points[i]);
                if (work->arrival < 8) {
                    addVector(&work->points[i], &work->point_steps[i]);
                }
            }
            work->arrival++;
            if (work->arrival > 8) {
                work->arrival = 8;
            }
        }
        if (work->stage != 0) {
            for (i = 0; i < work->line_count; i++) {
                line_packet->prim.attribute = 0x50000000;
                setRGB0(&line_packet->prim, 255, 128, 128);
                setRGB1(&line_packet->prim, 0, 0, 0);
                RotTransPers(&work->heads[i], &line_packet->w.xy0, &p, &flag);
                RotTransPers(&work->tails[i], &line_packet->w.xy1, &p, &flag);
                line_packet->prim.x0 += gDuelEffect_DrawOffset.vx;
                line_packet->prim.x1 += gDuelEffect_DrawOffset.vx;
                line_packet->prim.y0 += gDuelEffect_DrawOffset.vy;
                line_packet->prim.y1 += gDuelEffect_DrawOffset.vy;
                if (flag >= 0) {
                    GsSortGLine(&line_packet->prim, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority);
                }
                if (__builtin_abs(work->heads[i].vx) > 32 ||
                    __builtin_abs(work->heads[i].vy) > 32 ||
                    __builtin_abs(work->heads[i].vz) > 32) {
                    addVector(&work->heads[i], &work->velocities[i]);
                    addVector(&work->tails[i], &work->velocities[i]);
                } else {
                    x = (rand() - rand()) % 4096;
                    y = (rand() - rand()) % 4096;
                    z = (rand() - rand()) % 4096;
                    setVector(&work->heads[i], x * 96 / 4096, y * 96 / 4096, z * 96 / 4096);
                    setVector(&work->tails[i], x * 128 / 4096, y * 128 / 4096, z * 128 / 4096);
                    setVector(&work->velocities[i], -work->tails[i].vx / 8,
                              -work->tails[i].vy / 8, -work->tails[i].vz / 8);
                }
            }
            DuelEffect_SetRgbColor(&color, 255, 128, 128);
            setVector(&scale, work->scale, work->scale, work->scale);
            DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
            DuelEffect_DrawLayeredTileQuad(&color, 8, 0, &position);
            DuelEffect_SetRgbColor(&color, 127, 64, 64);
            for (i = 0; i < work->ray_count; i++) {
                setVector(&scale, 4096, work->ray_scales[i], 4096);
                DuelEffect_SetTransformMatrix(&world, &saved, &position, &work->rotations[i], &scale, 0);
                DuelEffect_DrawLayeredGradientTrapezoid(&color, work->widths, 32, 0);
                if (work->ray_scales[i] < 0x6000) {
                    work->ray_scales[i] += 0x400;
                } else {
                    work->ray_scales[i] = 4096;
                    setVector(&work->rotations[i], (rand() - rand()) % 4096,
                              (rand() - rand()) % 4096, (rand() - rand()) % 4096);
                }
            }
            if (work->scale < 0x6000) {
                work->scale += 128;
            }
            work->line_count++;
            if (work->line_count > 64) {
                work->line_count = 64;
            }
            if (!(work->tick & 1)) {
                work->ray_count++;
                if (work->ray_count > 32) {
                    work->ray_count = 32;
                }
            }
        }
        if (work->line_count == 64 && work->ray_count == 32 &&
            /* One word load tests the colour's r/g/b bytes together. */
            work->scale >= 0x6000 && (*(u32 *)&work->center_color & 0xFFFFFF) == 0xFFFFFF &&
            work->stage < 2) {
            work->stage = 2;
        }
        if ((u16)DuelEffect_HasNonzeroColor(&work->flash_color) && work->stage == 0) {
            DuelEffect_DrawFadingFullscreenColor(&work->flash_color, 15);
        }
        if (work->stage == 2) {
            DuelEffect_DrawFullscreenColor(&work->flash_color);
            if ((u16)DuelEffect_StepColorToTarget(&work->flash_color, 192, 192, 192, 4)) {
                DuelEffect_SetGrayColor(&work->flash_color, 255);
                gDuelEffect_pActiveRequest->progress = 1;
                work->stage = 3;
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor(&work->flash_color) && work->stage == 3) {
            DuelEffect_DrawFadingFullscreenColor(&work->flash_color, 0);
        }
        work->tick++;
        PopMatrix();
        if (work->stage == 3) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
