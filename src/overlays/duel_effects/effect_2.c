#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_2.h"
#include "fallback_semantic_views.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "texture_state_semantic_views.h"

void func_80153200(void *buffer, s32 phase, s16 number)
{
    MATRIX world;
    POLY_GT4 polygon;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    CVECTOR color_copy;
    POLY_GT4 *packet;
    DuelEffect2Work *work;
    s32 i;
    s32 frame_step;
    s16 value;

    memset(&rotation, 0, sizeof(rotation));
    memset(&position, 0, sizeof(position));
    value = number;
    packet = &polygon;
    scale = D_801461B8;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 7) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            if (number == 0) {
                work->config = &D_8015AF18[6];
            } else {
                work->config = &D_8015AF18[phase];
            }
            for (i = 0; i < 3; i++) {
                DuelEffect_InitCircle32Vertices(work->config->radii[i], work->rings[i]);
            }
            DuelEffect_InitParticleVectors(work->config->particle_speed, work->positions,
                         work->velocities, work->config->particle_count);
            DuelEffect_InitRandomVectorsUnscaled(64, work->rotations);
            work->scale = 0;
            DuelEffect_SetRgbColor((u8 *)&work->color, work->config->color[0],
                         work->config->color[1], work->config->color[2]);
            DuelEffect_SetGrayColor((u8 *)&work->background_color, 0);
            work->frame = 0;
            work->stage = 0;
            work->number_state = 0;
            if (value > 0) {
                DuelEffect_SetRgbColor((u8 *)&work->number_color,
                             work->config->number_red[1] >> 3,
                             work->config->number_green[1] >> 3,
                             work->config->number_blue[1] >> 3);
            } else if (value < 0) {
                DuelEffect_SetRgbColor((u8 *)&work->number_color,
                             work->config->number_red[0] >> 3,
                             work->config->number_green[0] >> 3,
                             work->config->number_blue[0] >> 3);
            } else if (value == 0) {
                DuelEffect_SetGrayColor((u8 *)&work->number_color, 0);
            }
            work->number = value;
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            DuelEffect_DrawErrorCross(1);
        } else {
            DuelEffect_DrawErrorCross(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            gDuelEffect_bRequestFinished = 1;
        }
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        DuelEffect_InitBaseMatrix(&world);
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 1);
        if (work->stage != 0 &&
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->number_color) && work->number != 0) {
            DuelEffect_DrawProjectedNumber(-__builtin_abs(work->number), (u8 *)&work->number_color,
                         &position, 1, 16, 0);
            if (work->number_state == 0) {
                if (work->number > 0) {
                    work->number_state = DuelEffect_StepColorToTarget(
                        (u8 *)&work->number_color, work->config->number_red[1],
                        work->config->number_green[1], work->config->number_blue[1], 15);
                } else if (work->number < 0) {
                    work->number_state = DuelEffect_StepColorToTarget(
                        (u8 *)&work->number_color, work->config->number_red[0],
                        work->config->number_green[0], work->config->number_blue[0], 15);
                }
            } else {
                DuelEffect_DarkenColor((u8 *)&work->number_color, 8);
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color)) {
            setPolyGT4(packet);
            packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_04;
            packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_06;
            setUV4(packet, 128, 0, 159, 0, 128, 63, 159, 63);
            setRGB0(packet, 0, 0, 0);
            setRGB1(packet, 0, 0, 0);
            setRGB2(packet, work->color.r, work->color.g, work->color.b);
            setRGB3(packet, work->color.r, work->color.g, work->color.b);
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            setVector(&rotation, 0, 0, -1536);
            DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 0);
            if (work->stage == 0) {
                DuelEffect_InitIndexedQuadVertices(quad, 2, 3, 1, 2, work->config->widths,
                             work->config->heights);
                DuelEffect_ProjectSubmitGt4(packet, quad, 0, 1);
                DuelEffect_InitIndexedQuadVertices(quad, 4, 3, 3, 2, work->config->widths,
                             work->config->heights);
                DuelEffect_ProjectSubmitGt4(packet, quad, 0, 1);
            } else {
                if (work->stage == 1) {
                    DuelEffect_SetGrayColor((u8 *)&work->background_color, 255);
                }
                DuelEffect_InitIndexedQuadToYZero(quad, 2, 1, 1, work->config->widths,
                             work->config->heights);
                DuelEffect_ProjectSubmitGt4(packet, quad, 0, 1);
            }
            if (work->stage >= 2) {
                DuelEffect_InitIndexedQuadToYZero(quad, 0, 1, 0, work->config->widths,
                             work->config->heights);
                DuelEffect_ProjectSubmitGt4(packet, quad, 0, 1);
            }
            if (work->stage != 0) {
                gDuelEffect_pActiveRequest->field_1D = 1;
                for (i = 0; i < work->config->particle_count; i++) {
                    DuelEffect_InitOffsetQuadVertices(quad, &work->positions[i]);
                    DuelEffect_DrawRandomTileQuad((u8 *)&work->color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
                scale.vx = (work->scale >> 1) + 4096;
                scale.vy = (work->scale >> 1) + 4096;
                scale.vz = 0;
                DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 1);
                DuelEffect_DrawLayeredTileQuad((u8 *)&work->color, work->config->sprite_size,
                             0, &position);
                scale.vx = work->scale + 4096;
                scale.vy = work->scale + 4096;
                scale.vz = 0;
                DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
                color_copy = work->color;
                color_copy.r >>= 1;
                color_copy.g >>= 1;
                color_copy.b >>= 1;
                DuelEffect_DrawTexturedRingStrip((u8 *)&color_copy, work->rings[0], work->rings[1],
                             work->rings[2], 0);
                if (work->config->variant == 1) {
                    scale.vx = (work->scale + 4096) >> 1;
                    scale.vy = (work->scale + 4096) >> 1;
                    scale.vz = 0;
                    DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
                    DuelEffect_DrawTexturedRingStrip((u8 *)&work->color, work->rings[0],
                                 work->rings[1], work->rings[2], 0);
                    scale.vx = 4096;
                    scale.vy = work->scale;
                    scale.vz = 4096;
                    for (i = 0; i < 64; i++) {
                        DuelEffect_SetTransformMatrix(&world, &saved, &position,
                                     &work->rotations[i], &scale, 2);
                        DuelEffect_DrawLayeredGradientTrapezoid((u8 *)&work->color, work->config->strip_widths,
                                     work->config->strip_height, 0);
                    }
                }
                if (work->scale < 0x6000) {
                    work->scale += 0x800;
                }
                DuelEffect_DarkenColor((u8 *)&work->color, 8);
            }
            if (work->stage < 3) {
                work->stage++;
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->background_color)) {
            DuelEffect_DrawFadingFullscreenColor((u8 *)&work->background_color, 15);
        }
        work->frame += frame_step;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->background_color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->number_color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
