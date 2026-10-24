#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_11.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "texture_semantic_views.h"
#include "primitive_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"

void func_80146760(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR zero;
    VECTOR scale;
    POLY_FT4 polygon;
    POLY_GT4 gradient;
    SVECTOR vertices[4];
    CVECTOR color;
    POLY_FT4 *packet;
    POLY_GT4 *gradient_packet;
    DuelEffect11Work *work;
    GsIMAGE *image;
    s32 i;
    s32 j;
    s32 k;
    s32 l;
    s32 variant;

    memset(&zero, 0, sizeof(zero));
    scale = D_80146004;
    packet = &polygon;
    gradient_packet = &gradient;
    work = buffer;
    if (phase >= 0) {
        variant = phase & 15;
        if (variant >= 5) {
            work->cross = 1;
        } else {
            work->cross = 0;
            work->config = &D_8015A4E4;
            DuelEffect_PackFixedPageClut(work->base_texture, &gDuelEffect_TextureModesAndVariants.variant.images[0]);
            image = &gDuelEffect_TextureModesAndVariants.variant.images[variant];
            DuelEffect_PackFixedPageClut(work->texture, image);
            work->uv[0] = image->px;
            work->uv[1] = image->py;
            copyVector(&work->origin, &gDuelEffect_DrawOffset);
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 7; j++) {
                    setVector(&work->positions[i][j], i * 12 - 18, 0, j * 8 - 24);
                    setVector(&work->velocities[i][j],
                              (work->config->speed + (rand() % 3 - 1) * 3) * work->positions[i][j].vx / 24,
                              (rand() % 3 - 1) * 3 - work->config->lift,
                              (work->config->speed + (rand() % 3 - 1) * 3) * work->positions[i][j].vz / 28);
                    setVector(&work->rotations[i][j], 0, 0, 0);
                    setVector(&work->rotation_steps[i][j],
                              ratan2(work->velocities[i][j].vz, work->velocities[i][j].vy) / 8,
                              ratan2(work->velocities[i][j].vx, work->velocities[i][j].vz) / 8,
                              ratan2(work->velocities[i][j].vy, work->velocities[i][j].vx) / 8);
                    DuelEffect_InitRandomVectorsXYZ(work->particles[i][j], work->config->spread,
                                 work->config->spread, work->config->spread, 16);
                    addVector(&work->positions[i][j], &work->origin);
                    DuelEffect_SetGrayColor((u8 *)&work->colors[i][j], 128);
                    DuelEffect_SetGrayColor((u8 *)&work->particle_colors[i][j], 255);
                    work->states[i][j] = 0;
                    work->frames[i][j] = rand() % 4;
                    for (k = 0; k < 16; k++) {
                        work->sizes[i][j][k] = work->config->fragment_size +
                            rand() % (work->config->fragment_size / 2);
                    }
                    work->chances[i][j] = work->config->chance;
                    if ((phase >> 7) == 1) {
                        work->modes[i][j] = 1;
                    } else {
                        work->modes[i][j] = 0;
                    }
                }
            }
            DuelEffect_InitRandomVectorsNegatedY(work->rays, work->config->ray_length, work->config->ray_length,
                         work->config->ray_length, work->config->ray_count);
            DuelEffect_InitParticleVectorsNegatedY(work->config->dust_speed, work->dust,
                         work->dust_velocities, work->config->dust_count);
            work->width = work->config->width;
            work->height = work->config->height;
            for (i = 0; i < 3; i++) {
                DuelEffect_InitXZRingVertices(work->config->radii[i], work->config->radii[i],
                             work->config->ring_height * i, work->rings[i], 32);
            }
            DuelEffect_SetRgbColor((u8 *)&work->glow, work->config->color.r,
                         work->config->color.g, work->config->color.b);
            work->scale = 4096;
            work->completed = 0;
            work->tick = 0;
        }
    } else if (work->cross) {
        if (work->cross == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
        }
        work->cross++;
        if (work->cross > 180) {
            gDuelEffect_bRequestFinished = 1;
        }
    } else {
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        setPolyFT4(packet);
        setPolyGT4(gradient_packet);
        DuelEffect_SetTransformMatrix(&world, &saved, &work->origin, &zero, &scale, 2);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->glow)) {
            DuelEffect_DrawSharedBaseTriangles((u8 *)&work->glow, work->config->ray_width, work->rays,
                         work->config->ray_count, 64);
            for (i = 0; i < work->config->dust_count; i++) {
                DuelEffect_InitOffsetQuadVertices(vertices, &work->dust[i]);
                DuelEffect_DrawRandomTileQuad((u8 *)&work->glow, vertices, 1);
                addVector(&work->dust[i], &work->dust_velocities[i]);
                work->dust_velocities[i].vy += 0;
            }
            DuelEffect_DrawLayeredTileQuad((u8 *)&work->glow, work->config->glow_size, 64, &zero);
            gradient_packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[11][0];
            gradient_packet->clut = gDuelEffect_TexturePageClutPairs.pairs[11][1];
            setUV4(gradient_packet, 0, 0, 31, 0, 0, 127, 31, 127);
            setRGB0(gradient_packet, 0, 0, 0);
            setRGB1(gradient_packet, 0, 0, 0);
            setRGB2(gradient_packet, work->glow.r, work->glow.g, work->glow.b);
            setRGB3(gradient_packet, work->glow.r, work->glow.g, work->glow.b);
            setVector(&vertices[0], -work->width, -work->height, 0);
            setVector(&vertices[1], work->width, -work->height, 0);
            setVector(&vertices[2], -work->width, 0, 0);
            setVector(&vertices[3], work->width, 0, 0);
            DuelEffect_ProjectSubmitGt4(gradient_packet, vertices, 32, 1);
            work->width -= work->config->shrink;
            if (work->width < work->config->min_width) {
                work->width = work->config->min_width;
            }
            work->height += work->config->growth;
            if (work->height > work->config->max_height) {
                work->height = work->config->max_height;
            }
            if (work->height == work->config->max_height && work->width == work->config->min_width) {
                DuelEffect_DarkenColor((u8 *)&work->glow, 8);
            }
            color = work->glow;
            color.r >>= 1;
            color.g >>= 1;
            color.b >>= 1;
            setVector(&scale, work->scale, 4096, work->scale);
            DuelEffect_SetTransformMatrix(&world, &saved, &work->origin, &zero, &scale, 3);
            DuelEffect_DrawTexturedRingStrip((u8 *)&color, work->rings[0], work->rings[1], work->rings[2], 1);
            setVector(&scale, work->scale >> 1, 6144, work->scale >> 1);
            DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
            DuelEffect_DrawTexturedRingStrip((u8 *)&work->glow, work->rings[0], work->rings[1], work->rings[2], 1);
            if (work->scale < 16384) {
                work->scale += 4096;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 7; j++) {
                if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->particle_colors[i][j])) {
                    if (work->states[i][j] == 0) {
                        if (work->modes[i][j] == 1) {
                            packet->tpage = work->base_texture[0];
                            packet->clut = work->base_texture[1];
                            setUV4(packet, i * 12 + 56, j * 8 + 128,
                                   i * 12 + 68, j * 8 + 128,
                                   i * 12 + 56, j * 8 + 136,
                                   i * 12 + 68, j * 8 + 136);
                        } else {
                            packet->tpage = work->texture[0];
                            packet->clut = work->texture[1];
                            setUV4(packet, work->uv[0] + i * 12, work->uv[1] + j * 8,
                                   work->uv[0] + i * 12 + 12, work->uv[1] + j * 8,
                                   work->uv[0] + i * 12, work->uv[1] + j * 8 + 8,
                                   work->uv[0] + i * 12 + 12, work->uv[1] + j * 8 + 8);
                        }
                        setRGB0(packet, work->colors[i][j].r, work->colors[i][j].g, work->colors[i][j].b);
                        DuelEffect_InitXZRectVertices(vertices, 6, 0, 4);
                        DuelEffect_SetTransformMatrix(&world, &saved, &work->positions[i][j], &work->rotations[i][j], &scale, 2);
                        DuelEffect_ProjectSubmitFt4(packet, vertices, 1, 0);
                    }
                    if (work->velocities[i][j].vy > 0 && work->states[i][j] == 0 &&
                        work->positions[i][j].vy >= -31) {
                        if (rand() % work->chances[i][j] == 0) {
                            work->states[i][j] = 1;
                            DuelEffect_SetGrayColor((u8 *)&work->colors[i][j], 0);
                        } else {
                            work->chances[i][j]--;
                        }
                    }
                    if (work->states[i][j]) {
                        DuelEffect_SetTransformMatrix(&world, &saved, &work->positions[i][j], &zero, &scale, 0);
                        packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[4][0];
                        packet->clut = gDuelEffect_TexturePageClutPairs.pairs[4][1];
                        setRGB0(packet, work->particle_colors[i][j].r,
                                work->particle_colors[i][j].g, work->particle_colors[i][j].b);
                        setUV4(packet, 128 + work->frames[i][j] * 32, 64,
                               159 + work->frames[i][j] * 32, 64,
                               128 + work->frames[i][j] * 32, 127,
                               159 + work->frames[i][j] * 32, 127);
                        for (k = 0; k < work->states[i][j]; k++) {
                            DuelEffect_InitRectVertices(vertices, (s16)work->sizes[i][j][k], work->sizes[i][j][k] * 2);
                            for (l = 0; l < 4; l++) {
                                addVector(&vertices[l], &work->particles[i][j][k]);
                            }
                            DuelEffect_ProjectSubmitFt4(packet, vertices, 1, 1);
                            work->sizes[i][j][k]--;
                        }
                        work->frames[i][j] = (work->frames[i][j] + 1) % 4;
                        work->states[i][j]++;
                        if (work->states[i][j] > 16) {
                            work->states[i][j] = 16;
                        }
                        DuelEffect_DarkenColor((u8 *)&work->particle_colors[i][j], 31);
                    }
                    if (work->states[i][j]) {
                        work->positions[i][j].vy -= 8;
                    } else if (work->positions[i][j].vy < 16) {
                        work->rotations[i][j].vx = (work->rotations[i][j].vx + work->rotation_steps[i][j].vx) % 4096;
                        work->rotations[i][j].vy = (work->rotations[i][j].vy + work->rotation_steps[i][j].vy) % 4096;
                        work->rotations[i][j].vz = (work->rotations[i][j].vz + work->rotation_steps[i][j].vz) % 4096;
                        work->angle_sums[i][j] = work->rotations[i][j].vx % 1024 +
                            work->rotations[i][j].vy % 1024 + work->rotations[i][j].vz % 1024;
                        if ((u16)(work->angle_sums[i][j] % 3) == 0) {
                            work->modes[i][j] = !work->modes[i][j];
                        }
                        if (work->velocities[i][j].vy < 0) {
                            if (!(work->tick & 1)) {
                                work->positions[i][j].vx += work->velocities[i][j].vx;
                            }
                            work->positions[i][j].vy += work->velocities[i][j].vy;
                            if (!(work->tick & 1)) {
                                work->positions[i][j].vz += work->velocities[i][j].vz;
                            }
                            work->velocities[i][j].vy += 3;
                        } else {
                            if (!(work->tick & 1)) {
                                work->positions[i][j].vx += work->velocities[i][j].vx;
                            }
                            work->positions[i][j].vy += work->velocities[i][j].vy;
                            if (!(work->tick & 1)) {
                                work->positions[i][j].vz += work->velocities[i][j].vz;
                            }
                            work->velocities[i][j].vy += 3;
                        }
                        if (work->positions[i][j].vy > 16) {
                            work->positions[i][j].vy = 16;
                            work->states[i][j] = 1;
                        }
                    }
                    if ((u16)DuelEffect_IsColorBlack((u8 *)&work->particle_colors[i][j])) {
                        work->completed++;
                    }
                }
            }
        }
        work->tick++;
        PopMatrix();
        if (work->completed == 28 && (u16)DuelEffect_IsColorBlack((u8 *)&work->glow)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
