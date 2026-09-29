#include "../../types.h"
#include "effect_17.h"

void func_80148BA4(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    VECTOR scale;
    POLY_GT4 polygon;
    SVECTOR vertices[4];
    CVECTOR color;
    POLY_GT4 *packet;
    DuelEffect17Work *work;
    s32 i;
    s32 j;
    s32 k;

    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146034;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross = 1;
        } else {
            work->cross = 0;
            work->config = &D_8015A60C[phase];
            copyVector(&work->origin, &D_8015B7F8);
            if (work->config->mode == 0) {
                Duel_CollectMatchingFieldCardObjects(D_8015B7A0, -1);
                setVector(&work->center, 0, -work->config->height, 0);
            } else {
                Duel_CollectMatchingFieldCardObjects(D_8015B7A0, 0);
                setVector(&work->center, D_8015B7F8.vx,
                          D_8015B7F8.vy - work->config->height, D_8015B7F8.vz);
            }
            work->card_count = 0;
            for (i = 0; D_8015B7A0[i] != 0; i++) {
                setVector(&work->card_velocities[i],
                          (work->center.vx - (s16)((DisplayObject *)D_8015B7A0[i])->field_30.h.field_30) / 16,
                          (work->center.vy - (s16)((DisplayObject *)D_8015B7A0[i])->field_30.h.field_32) / 16,
                          (work->center.vz - ((DisplayObject *)D_8015B7A0[i])->field_34.h.field_34) / 16);
                setVector(&work->card_rotations[i],
                          (ratan2(work->card_velocities[i].vz, work->card_velocities[i].vy) % 256) / 16,
                          (ratan2(work->card_velocities[i].vx, work->card_velocities[i].vz) % 256) / 16,
                          (ratan2(work->card_velocities[i].vy, work->card_velocities[i].vx) % 256) / 16);
                func_8014F608(work->card_particles[i], 128, 128, 128, 16);
                work->card_count++;
                work->card_states[i] = 0;
                work->card_ages[i] = 0;
            }
            func_8014F608(work->particles, work->config->radius * 2,
                         work->config->radius * 2, work->config->radius * 2, 64);
            for (i = 0; i < 64; i++) {
                setVector(&work->velocities[i], -work->particles[i].vx / 32,
                          -work->particles[i].vy / 32, -work->particles[i].vz / 32);
                func_8014F020((u8 *)&work->particle_colors[i], work->config->color[0] / 8,
                             work->config->color[1] / 8, work->config->color[2] / 8);
            }
            for (i = 0; i < 48; i++) {
                func_8014FABC(24, 8, work->config->height, 8, work->paths[i]);
                func_8014F020((u8 *)&work->path_colors[i], work->config->burst_color[0],
                             work->config->burst_color[1], work->config->burst_color[2]);
                setVector(&work->endpoints[i], 0, 0, 0);
                work->path_ages[i] = 0;
                work->path_states[i] = 0;
                work->angles[i] = rand() % 512;
            }
            work->tick = 0;
            func_8014F020((u8 *)&work->color, work->config->color[0] / 8,
                         work->config->color[1] / 8, work->config->color[2] / 8);
            func_8014F010((u8 *)&work->burst_color, 0);
            work->field_1D94 = 0;
            work->completed = 0;
            work->stage = 0;
            work->brightness = 128;
            work->active_cards = 0;
            work->active_particles = 0;
            work->active_paths = 0;
        }
    } else if (work->cross) {
        if (work->cross == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
        }
        work->cross++;
        if (work->cross > 180) {
            D_8009B261 = 1;
        }
    } else {
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        if (work->stage < 2) {
            if (work->brightness > 8) {
                work->brightness -= 8;
            } else {
                work->brightness = 0;
            }
        } else {
            if (work->brightness < 120) {
                work->brightness += 8;
            } else {
                work->brightness = 128;
                work->stage = 3;
            }
        }
        func_8015405C(work->brightness, work->brightness, work->brightness);
        setVector(&rotation, 0, 0, work->config->spin * work->tick % 4096);
        func_801513F4(&world, &saved, &work->center, &rotation, &scale, 0);
        for (i = 0; i < work->active_particles; i++) {
            if ((u16)func_8014D3AC((u8 *)&work->particle_colors[i])) {
                func_8014F2D4(vertices, &work->particles[i]);
                if (!(i & 3)) {
                    func_80155BC0((u8 *)&work->particle_colors[i], 8, 1, &work->particles[i]);
                }
                func_80156064((u8 *)&work->particle_colors[i], vertices, 1);
                if (__builtin_abs(work->particles[i].vx) > 32 ||
                    __builtin_abs(work->particles[i].vy) > 32 ||
                    __builtin_abs(work->particles[i].vz) > 32) {
                    addVector(&work->particles[i], &work->velocities[i]);
                    func_80153F98((u8 *)&work->particle_colors[i], work->config->color[0],
                                 work->config->color[1], work->config->color[2], 31);
                } else {
                    func_80153F28((u8 *)&work->particle_colors[i], 31);
                    if ((u16)func_8014D378((u8 *)&work->particle_colors[i]) &&
                        work->card_states[work->card_count - 1] == 0) {
                        setVector(&work->particles[i],
                                  work->config->radius * (s32)((u32)((rand() - rand()) % 4096) << 1) / 4096,
                                  work->config->radius * (s32)((u32)((rand() - rand()) % 4096) << 1) / 4096,
                                  work->config->radius * (s32)((u32)((rand() - rand()) % 4096) << 1) / 4096);
                        setVector(&work->velocities[i], -work->particles[i].vx / 32,
                                  -work->particles[i].vy / 32, -work->particles[i].vz / 32);
                        func_8014F020((u8 *)&work->particle_colors[i], work->config->color[0] / 8,
                                     work->config->color[1] / 8, work->config->color[2] / 8);
                    }
                    if (!(u16)func_8014D378((u8 *)&work->particle_colors[i]) && work->stage >= 2) {
                        work->stage = 2;
                    }
                }
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->color)) {
            setPolyGT4(packet);
            packet->tpage = D_8015B748.pairs[15][0];
            packet->clut = D_8015B748.pairs[15][1];
            setUV4(packet, 96, 0, 159, 0, 96, 63, 159, 63);
            setVector(&vertices[0], -work->config->radius, -work->config->radius, 0);
            setVector(&vertices[1], -work->config->radius, 0, 0);
            setVector(&vertices[2], 0, -work->config->radius, 0);
            setVector(&vertices[3], 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            setRGB3(packet, 0, 0, 0);
            for (j = 0; j < 3; j++) {
                setVector(&scale, 4096 - j * 1280, 4096 - j * 1280, 0);
                setRGB0(packet, work->color.r / (j + 1), work->color.g / (j + 1), work->color.b / (j + 1));
                setRGB1(packet, work->color.r / (j + 1), work->color.g / (j + 1), work->color.b / (j + 1));
                setRGB2(packet, work->color.r / (j + 1), work->color.g / (j + 1), work->color.b / (j + 1));
                for (i = 0; i < 4; i++) {
                    for (k = 0; k < 3; k++) {
                        setVector(&rotation, 0, 0,
                                  -(work->tick * ((j * 2 + 1) * work->config->fan_spin) + k * 32 + i * 1024) % 4096);
                        func_801513F4(&world, &saved, &work->center, &rotation, &scale, 0);
                        func_8015131C(packet, vertices, 1, 1);
                    }
                }
                if (work->field_1D94 < 4096 && work->tick < 999) {
                    work->field_1D94 += 64;
                } else if (work->tick >= 999) {
                    work->field_1D94 -= 64;
                }
                if (work->stage < 2) {
                    work->stage = func_80153F98((u8 *)&work->color, work->config->color[0],
                                              work->config->color[1], work->config->color[0], 2);
                } else {
                    func_80153F28((u8 *)&work->color, 1);
                }
            }
        }
        if (work->card_count && work->stage < 2) {
            work->active_particles++;
            if (work->active_particles > 64) {
                work->active_particles = 64;
            }
        }
        if (work->stage != 0 && work->tick > 60) {
            if (work->card_count == 0 && work->stage == 1) {
                work->stage = 2;
            }
            for (i = 0; i < work->active_cards; i++) {
                if (work->card_states[i] == 0) {
                    ((DisplayObject *)D_8015B7A0[i])->field_30.h.field_30 += work->card_velocities[i].vx;
                    ((DisplayObject *)D_8015B7A0[i])->field_30.h.field_32 += work->card_velocities[i].vy;
                    ((DisplayObject *)D_8015B7A0[i])->field_34.h.field_34 += work->card_velocities[i].vz;
                    ((DisplayObject *)D_8015B7A0[i])->field_44.h.field_44 -= 256;
                    ((DisplayObject *)D_8015B7A0[i])->field_44.h.field_46 -= 256;
                    ((DisplayObject *)D_8015B7A0[i])->field_20.b.field_20 += work->card_rotations[i].vx;
                    ((DisplayObject *)D_8015B7A0[i])->field_20.b.field_21 += work->card_rotations[i].vy;
                    ((DisplayObject *)D_8015B7A0[i])->field_20.b.field_22 += work->card_rotations[i].vz;
                    work->card_ages[i]++;
                    if (work->card_ages[i] > 16) {
                        work->card_states[i] = 1;
                    }
                }
                if (work->card_states[i] == 1) {
                    work->completed++;
                    ((DisplayObject *)D_8015B7A0[i])->flags &= ~0x40;
                    work->card_states[i] = 2;
                    func_8014F020((u8 *)&work->burst_color, work->config->burst_color[0],
                                 work->config->burst_color[1], work->config->burst_color[2]);
                }
            }
            if (!(work->tick & 7)) {
                work->active_cards++;
                if (work->active_cards > work->card_count) {
                    work->active_cards = work->card_count;
                }
            }
        }
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        func_801513F4(&world, &saved, &work->center, &rotation, &scale, 0);
        if ((u16)func_8014D3AC((u8 *)&work->burst_color)) {
            func_80155BC0((u8 *)&work->burst_color, 32, 1, 0);
            color = work->burst_color;
            color.r >>= 1;
            color.g >>= 1;
            color.b >>= 1;
            func_80156E58((u8 *)&color, 3, work->card_particles[work->completed - 1], 16, 36);
            func_80153F28((u8 *)&work->burst_color, 15);
            if (work->card_states[work->card_count - 1] >= 2 &&
                (u16)func_8014D378((u8 *)&work->burst_color)) {
                work->stage = 2;
            }
        }
        if (work->config->mode && work->card_count) {
            for (i = 0; i < work->active_paths; i++) {
                setVector(&rotation, 0, work->angles[i], 0);
                func_801513F4(&world, &saved, &work->origin, &rotation, &scale, 2);
                if ((u16)func_8014D3AC((u8 *)&work->path_colors[i])) {
                    if (work->path_states[i] != 2) {
                        work->path_states[i] = func_801570B0((u8 *)&work->path_colors[i], work->path_ages[i],
                            work->paths[i], 16, 1, &work->endpoints[i], 1, &work->origin);
                    } else {
                        func_801570B0((u8 *)&work->path_colors[i], work->path_ages[i],
                            work->paths[i], 32, 0, &work->endpoints[i], 1, &work->origin);
                    }
                    if (work->path_states[i] == 2 &&
                        (u16)func_8014D3AC((u8 *)&work->path_colors[i])) {
                        func_801513F4(&world, &saved, &work->endpoints[i], &rotation, &scale, 0);
                        func_80155BC0((u8 *)&work->path_colors[i], 4, 1, 0);
                    }
                    work->path_ages[i]++;
                    if (work->path_ages[i] >= 8) {
                        work->path_ages[i] = 7;
                        func_80153F28((u8 *)&work->path_colors[i], 15);
                        if ((u16)func_8014D378((u8 *)&work->path_colors[i]) && work->stage < 2) {
                            func_8014F020((u8 *)&work->path_colors[i], work->config->burst_color[0],
                                         work->config->burst_color[1], work->config->burst_color[2]);
                            work->path_ages[i] = 0;
                        }
                    }
                }
            }
        }
        if (work->stage == 1) {
            work->active_paths++;
            if (work->active_paths > 48) {
                work->active_paths = 48;
            }
        }
        work->tick++;
        PopMatrix();
        if (work->stage == 3 && work->tick > 180) {
            D_8009B261 = 1;
        }
    }
}
