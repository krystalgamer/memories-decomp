#include "../../types.h"
#include "effect_16.h"

void func_80151558(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR translation;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    SVECTOR quad[4];
    CVECTOR color;
    POLY_FT4 *packet;
    DuelEffect16Work *work;
    s32 i;
    s32 j;
    s32 k;
    s32 l;

    memset(&translation, 0, sizeof(translation));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146198;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015AEF4;
            for (i = 0; i < 5; i++) {
                for (k = 0; k < 5; k++) {
                    for (l = 0; l < 4; l++) {
                        setVector(&work->paths[k][l][i],
                            work->config->spread * ((rand() - rand()) % 4096) / 4096,
                            k * work->config->vertical_step - 256,
                            work->config->spread * ((rand() - rand()) % 4096) / 4096);
                        if (k == 4) {
                            work->paths[k][l][i].vx = 0;
                            work->paths[k][l][i].vz = 0;
                        }
                    }
                }
                func_8014F608(work->clouds[i], work->config->particle_spread,
                    work->config->particle_spread, work->config->particle_spread, 32);
                /* Retail advances overlapping 32-vector windows by five vectors. */
                func_8014F180(8, &work->positions[i * 5], &work->velocities[i * 5], 32);
                work->scales[i] = 4096;
                work->ages[i] = 0;
                func_8014F010((u8 *)&work->primary[i], 192);
                func_8014F020((u8 *)&work->secondary[i], work->config->color.r,
                    work->config->color.g, work->config->color.b);
                setVector(&work->origins[i],
                    (70 * i - 140) * (D_8015B7F8.vz < 0 ? -1 : 1),
                    -24, D_8015B7F8.vz >= 0 ? 95 : -95);
                work->activated[i] = 0;
            }
            for (k = 0; k < 3; k++) {
                func_8014EE0C(work->config->ring_radii[k], work->config->ring_radii[k],
                    work->config->ring_height * k, work->rings[k], 32);
            }
            work->count = 0;
            work->hits = 0;
            work->tick = 0;
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            D_8009B261 = 1;
        }
    } else {
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        for (j = 0; j < work->count; j++) {
            func_801513F4(&world, &saved, &work->origins[j], &rotation, &scale, 3);
            if ((u16)func_8014D3AC((u8 *)&work->primary[j]) ||
                (u16)func_8014D3AC((u8 *)&work->secondary[j])) {
                setPolyFT4(packet);
                packet->tpage = D_8015B748.pairs[14][0];
                packet->clut = D_8015B748.pairs[14][1];
                setUV4(packet, 80, 0, 127, 0, 80, 127, 127, 127);
                setRGB0(packet, work->primary[j].r, work->primary[j].g, work->primary[j].b);
                for (i = 0; i < 4; i++) {
                    for (k = 0; k < work->ages[j]; k++) {
                        for (l = 0; l < 4; l++) {
                            copyVector(&quad[l],
                                &work->paths[k + l / 2][i][j]);
                            quad[l].vx += (s16)func_8014F524(-1, l + 1) *
                                work->config->widths[i];
                        }
                        func_80151218(packet, quad, 32, 1);
                    }
                    if (work->ages[j] == 4) {
                        func_80155BC0((u8 *)&work->secondary[j], work->config->glow_sizes[i] * 2,
                            32, &work->paths[work->ages[j]][i][j]);
                    } else {
                        func_80155BC0((u8 *)&work->secondary[j], work->config->glow_sizes[i],
                            32, &work->paths[work->ages[j]][i][j]);
                    }
                }
                color = work->secondary[j];
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
                copyVector(&translation, &work->paths[work->ages[j]][0][j]);
                addVector(&translation, &work->origins[j]);
                func_801513F4(&world, &saved, &translation, &rotation, &scale, 2);
                func_80156E58((u8 *)&color, work->config->line_width, work->clouds[j], 32, 32);
                work->ages[j]++;
                if (work->ages[j] >= 5) {
                    if (work->activated[j] == 0) {
                        work->hits++;
                        work->activated[j] = 1;
                        D_8009B264->field_1D = work->hits;
                    }
                    work->ages[j] = 4;
                    for (k = 0; k < 32; k++) {
                        func_8014F2D4(quad, &(&work->positions[j * 5])[k]);
                        func_80156064((u8 *)&work->secondary[j], quad, 1);
                        addVector(&(&work->positions[j * 5])[k],
                            &(&work->velocities[j * 5])[k]);
                    }
                    setVector(&scale, work->scales[j], 4096, work->scales[j]);
                    func_801514BC(&saved, &scale);
                    func_8015616C((u8 *)&color, work->rings[0], work->rings[1], work->rings[2], 32);
                    setVector(&scale, work->scales[j] / 2, 6144, work->scales[j] / 2);
                    func_801514BC(&saved, &scale);
                    func_8015616C((u8 *)&work->secondary[j],
                        work->rings[0], work->rings[1], work->rings[2], 32);
                    if (work->scales[j] < 16384) {
                        work->scales[j] += 4096;
                    }
                    func_80153F28((u8 *)&work->primary[j], 31);
                    func_80153F28((u8 *)&work->secondary[j], 15);
                }
            }
        }
        work->tick++;
        if (!(work->tick & 7)) {
            work->count++;
            if (work->count > 5) {
                work->count = 5;
            }
        }
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->primary[4]) &&
            (u16)func_8014D378((u8 *)&work->secondary[4])) {
            D_8009B261 = 1;
        }
    }
}
