#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_13.h"
#include "geometry_semantic_views.h"
#include "number_semantic_views.h"
#include "primitive_semantic_views.h"
#include "layered_semantic_views.h"
#include "display_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "stat_penalty_semantic_views.h"
#include "fallback_semantic_views.h"

void DuelEffect_RunStatPenalty(void *buffer, s32 phase, s16 number)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color;
    DuelEffect13Work *work;
    s32 i;
    s32 frame_step;
    VECTOR *scale_pointer;
    SVECTOR *position_pointer;

    scale_pointer = &scale;
    position_pointer = &position;
    memset(position_pointer, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    *scale_pointer = gDuelEffect_StatPenaltyInitialScale;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 4) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_StatPenaltyConfigs[phase];
            setVector(&work->offset, gDuelEffect_DrawOffset.vx, gDuelEffect_DrawOffset.vy, gDuelEffect_DrawOffset.vz);
            setVector(&work->origin, 0, -work->config->height, 0);
            addVector(&work->origin, &gDuelEffect_DrawOffset);
            for (i = 0; i < 48; i++) {
                DuelEffect_InitJaggedColumnVertices(work->config->radius, work->config->size >> 1,
                             work->config->height, 8, work->paths[i]);
                setVector(&work->endpoints[i], 0, 0, 0);
                DuelEffect_SetRgbColor((u8 *)&work->colors[i], work->config->color[0],
                             work->config->color[1], work->config->color[2]);
                work->ages[i] = 0;
                work->states[i] = 0;
                work->angles[i] = rand() % 512;
            }
            setVector(&work->number_position, 0, 0, 0);
            setVector(&work->number_velocity, 0, -12, 0);
            work->active = 0;
            work->bounces = 0;
            work->frame = 0;
            work->tick = 1;
            work->ready = 0;
            work->number_mode = 0;
            work->number = number;
            DuelEffect_SetRgbColor((u8 *)&work->glow_color, work->config->color[0] >> 3,
                         work->config->color[1] >> 3, work->config->color[2] >> 3);
            if (work->config->background == 0) {
                DuelEffect_SetGrayColor((u8 *)&work->background_color, 0);
            } else {
                DuelEffect_SetGrayColor((u8 *)&work->background_color, 1);
            }
            DuelEffect_SetRgbColor((u8 *)&work->number_color, work->config->number_color[0],
                         work->config->number_color[1], work->config->number_color[2]);
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
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->glow_color)) {
            setVector(&rotation, 0, 0, work->tick * 128);
            DuelEffect_SetTransformMatrix(&world, &saved, &work->origin, &rotation, scale_pointer, 0);
            if (work->config->blend != 1) {
                gDuelEffect_TexturePageClutPairs.named.page_at_00 = getTPage(gDuelEffect_StatPenaltyTextureImages[0].pmode,
                    work->config->blend, gDuelEffect_StatPenaltyTextureImages[0].px, gDuelEffect_StatPenaltyTextureImages[0].py);
            }
            DuelEffect_DrawLayeredTileQuad((u8 *)&work->glow_color, work->config->size, 1, position_pointer);
            if (work->config->blend != 1) {
                color = work->glow_color;
                color.r /= 4;
                color.r /= 4;
                color.r /= 4;
                gDuelEffect_TexturePageClutPairs.named.page_at_00 = getTPage(gDuelEffect_StatPenaltyTextureImages[0].pmode,
                    1, gDuelEffect_StatPenaltyTextureImages[0].px, gDuelEffect_StatPenaltyTextureImages[0].py);
                DuelEffect_DrawLayeredTileQuad((u8 *)&color, work->config->size * 2, 16, position_pointer);
            }
            if (work->active == 48) {
                DuelEffect_DarkenColor((u8 *)&work->glow_color, 15);
            } else {
                DuelEffect_StepColorToTarget((u8 *)&work->glow_color, work->config->color[0] >> 1,
                             work->config->color[1] >> 1, work->config->color[2] >> 1, 15);
            }
        }
        if (work->config->background == 1 &&
            (u16)DuelEffect_HasNonzeroColor((u8 *)&work->background_color)) {
            setVector(&rotation, 0, 0, 0);
            DuelEffect_SetTransformMatrix(&world, &saved, &work->origin, &rotation, &scale, 0);
            DuelEffect_DrawQuadrantTileQuads((u8 *)&work->background_color, work->config->size * 2, 32);
            if (work->ready == 0) {
                work->ready = DuelEffect_StepColorToTarget((u8 *)&work->background_color, 128, 128, 128, 8);
            }
            if (work->active == 48) {
                DuelEffect_DarkenColor((u8 *)&work->background_color, 8);
            }
        }
        for (i = 0; i < work->active; i++) {
            setVector(&rotation, 0, work->angles[i], 0);
            DuelEffect_SetTransformMatrix(&world, &saved, &work->offset, &rotation, &scale, 2);
            if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->colors[i])) {
                if (work->states[i] != 2) {
                    work->states[i] = DuelEffect_DrawGradientPath((u8 *)&work->colors[i], work->ages[i],
                        work->paths[i], 16, 1, &work->endpoints[i], work->config->blend,
                        &work->offset);
                } else {
                    DuelEffect_DrawGradientPath((u8 *)&work->colors[i], work->ages[i],
                        work->paths[i], 32, 0, &work->endpoints[i], work->config->blend,
                        &work->offset);
                }
                work->ages[i]++;
                if (work->ages[i] >= 8) {
                    work->ages[i] = 7;
                    DuelEffect_DarkenColor((u8 *)&work->colors[i], 15);
                }
            }
        }
        if (work->config->blend != 1) {
            gDuelEffect_TexturePageClutPairs.named.page_at_00 = getTPage(gDuelEffect_StatPenaltyTextureImages[0].pmode,
                work->config->blend, gDuelEffect_StatPenaltyTextureImages[0].px, gDuelEffect_StatPenaltyTextureImages[0].py);
        }
        for (i = 0; i < work->active; i++) {
            if (work->states[i] == 2 &&
                (u16)DuelEffect_HasNonzeroColor((u8 *)&work->colors[i])) {
                gDuelEffect_pActiveRequest->progress = 1;
                DuelEffect_SetTransformMatrix(&world, &saved, &work->endpoints[i], &rotation, &scale, 0);
                DuelEffect_DrawLayeredTileQuad((u8 *)&work->colors[i], work->config->particle_size, 1, &position);
            }
        }
        if (work->active >= 12 && (u16)DuelEffect_HasNonzeroColor((u8 *)&work->number_color)) {
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, 0, 0, 0);
            DuelEffect_SetTransformMatrix(&world, &saved, &work->offset, &rotation, &scale, 0);
            DuelEffect_DrawProjectedNumber(-__builtin_abs(work->number), (u8 *)&work->number_color,
                         &work->number_position, work->number_mode, 10, 1);
            addVector(&work->number_position, &work->number_velocity);
            work->number_velocity.vy += 3;
            if (work->number_position.vy >= 0) {
                work->number_position.vy = 0;
                if (work->bounces < 5) {
                    work->bounces++;
                    work->number_velocity.vy = -12 / work->bounces;
                } else {
                    work->number_mode = 1;
                    work->number_velocity.vy = 0;
                    DuelEffect_DarkenColor((u8 *)&work->number_color, 15);
                }
            }
        }
        work->active += 2;
        if (work->active > 48) {
            work->active = 48;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->colors[47]) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->background_color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->number_color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
