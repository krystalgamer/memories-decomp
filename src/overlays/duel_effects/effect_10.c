#include "../../types.h"
#include "effect_10.h"

void func_8014C8FC(void *buffer, s32 phase)
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
    scale = D_80146138;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 6) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015AB14[phase];
            for (i = 0; i < 2; i++) {
                func_8014EC8C(175, 194, 16, (u16)(i * 8), work->rings[i], 4);
            }
            func_8014F3E8(work->plane, 175, 16, 194);
            func_8014F608(work->particles, 175, 16, 194, 64);
            for (i = 0; i < 64; i++) {
                work->speeds[i] = work->config->particle_speed / 2 +
                    work->config->particle_speed * (rand() % 4096) / 4096;
            }
            func_8014F608(work->columns, 139, 0, 182, 64);
            for (i = 0; i < 64; i++) {
                func_8014F010((u8 *)&work->column_colors[i], 1);
                work->ages[i] = 0;
            }
            func_8014F020((u8 *)&work->color, work->config->color.r >> 3,
                         work->config->color.g >> 3, work->config->color.b >> 3);
            func_8014F010((u8 *)&work->screen_color, 255);
            func_8014F010((u8 *)&work->ring_color, 1);
            func_8014F010((u8 *)&work->plane_color, 1);
            work->height = 8;
            work->stage = 0;
            work->ring_ready = 0;
            work->color_ready = 0;
            work->frame = 0;
            work->tick = 0;
            work->spawned = 0;
            if (work->config->mode != 1) {
                D_8015B748.pairs[12][0] = getTPage(D_8015A8C8[12].pmode,
                    work->config->mode, D_8015A8C8[12].px, D_8015A8C8[12].py);
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
            D_8009B261 = 1;
        }
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        func_801513F4(&world, &saved, &position, &rotation, &scale, 2);
        if ((u16)func_8014D3AC((u8 *)&work->color) && work->stage == 0) {
            func_80156FA4((u8 *)&work->color, work->plane, work->config->mode, 1);
            func_80156FA4((u8 *)&work->plane_color, work->plane, work->config->mode, 1);
            if (work->config->mode == 1) {
                func_80153F98((u8 *)&work->color, work->config->color.r,
                             work->config->color.g, work->config->color.b, 4);
                work->color_ready = func_80153F98((u8 *)&work->plane_color, 192, 192, 192, 2);
            } else {
                work->color_ready = func_80153F98((u8 *)&work->color,
                    work->config->color.r, work->config->color.g, work->config->color.b, 4);
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->ring_color) && work->stage == 0) {
            func_801558F4((u8 *)&work->ring_color, work->rings[0], work->rings[1], 1, 1);
            if (work->height < work->config->maximum_height) {
                work->height += work->config->height_step;
                for (i = 0; i < 4; i++) {
                    work->rings[1][i].vy = -work->height;
                }
            }
            if (work->ring_ready == 0) {
                work->ring_ready = func_80153F98((u8 *)&work->ring_color, 255, 255, 255, 8);
            }
        }
        func_801513F4(&world, &saved, &position, &rotation, &scale, 0);
        if ((u16)func_8014D3AC((u8 *)&work->color) && work->stage == 1) {
            for (i = 0; i < 64; i++) {
                func_8014F2D4(quad, &work->particles[i]);
                func_80156064((u8 *)&work->color, quad, 80);
                work->particles[i].vy -= work->speeds[i];
            }
            func_80153F28((u8 *)&work->color, 8);
        }
        if (work->stage == 0) {
            for (i = 0; i < work->spawned; i++) {
                if ((u16)func_8014D3AC((u8 *)&work->column_colors[i])) {
                    setPolyFT4(packet);
                    packet->tpage = D_8015B748.pairs[12][0];
                    packet->clut = D_8015B748.pairs[12][1];
                    setUV4(packet, 224, 192, 255, 192, 224, 223, 255, 223);
                    for (j = 0; j < 4; j++) {
                        if (!((work->tick + i + j) & 1)) {
                            setRGB0(packet, work->column_colors[i].r / (j + 1),
                                    work->column_colors[i].g / (j + 1),
                                    work->column_colors[i].b / (j + 1));
                        } else {
                            setRGB0(packet, 192 / (j + 1), 192 / (j + 1), 192 / (j + 1));
                        }
                        func_8014F490(quad, (s16)(12 - j), (s16)(24 - j));
                        for (k = 0; k < 4; k++) {
                            addVector(&quad[k], &work->columns[i]);
                            quad[k].vy += (j + 1) * 10;
                            quad[k].vy -= work->ages[i];
                        }
                        func_80151218(packet, quad, 1, work->config->mode);
                    }
                    work->ages[i] += 12;
                    if (work->ages[i] > 160) {
                        func_80153F28((u8 *)&work->column_colors[i], 15);
                        if ((u16)func_8014D378((u8 *)&work->column_colors[i])) {
                            work->ages[i] = 0;
                            func_8014F010((u8 *)&work->column_colors[i], 1);
                        }
                    } else {
                        func_80153F98((u8 *)&work->column_colors[i],
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
        if ((u16)func_8014D3AC((u8 *)&work->screen_color) && work->stage == 1) {
            func_801556F4((u8 *)&work->screen_color, 15);
        }
        if (*(u32 *)&work->color_ready == 0x10001) {
            D_8009B264->field_1D = 1;
        }
        if (work->stage == 0 && phase < -1) {
            if (work->config->mode == 2) {
                func_8014F010((u8 *)&work->color, 64);
            }
            work->stage = 1;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->color) &&
            (u16)func_8014D378((u8 *)&work->screen_color) && work->stage == 1) {
            D_8009B261 = 1;
        }
    }
}
