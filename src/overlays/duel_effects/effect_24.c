#include "../../types.h"
#include "effect_24.h"

void func_8014D3E8(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    GsGLINE line;
    SVECTOR vertices[4];
    CVECTOR color;
    POLY_FT4 *packet;
    GsGLINE *line_packet;
    DuelEffect24Work *work;
    s32 i;
    s32 x;
    s32 y;
    s32 z;
    s32 p;
    s32 flag;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146148;
    packet = &polygon;
    line_packet = &line;
    work = buffer;
    if (phase >= 0) {
        DisplayObject_CopyWorkSlots((s32 *)D_8015B7A0);
        work->center.vx = ((s16)((DisplayObject *)D_8015B7A0[0])->field_30.h.field_30 +
                           (s16)((DisplayObject *)D_8015B7A0[1])->field_30.h.field_30 +
                           (s16)((DisplayObject *)D_8015B7A0[2])->field_30.h.field_30 +
                           (s16)((DisplayObject *)D_8015B7A0[3])->field_30.h.field_30 +
                           (s16)((DisplayObject *)D_8015B7A0[4])->field_30.h.field_30 + 120) / 5;
        work->center.vy = ((s16)((DisplayObject *)D_8015B7A0[0])->field_30.h.field_32 +
                           (s16)((DisplayObject *)D_8015B7A0[1])->field_30.h.field_32 +
                           (s16)((DisplayObject *)D_8015B7A0[2])->field_30.h.field_32 +
                           (s16)((DisplayObject *)D_8015B7A0[3])->field_30.h.field_32 +
                           (s16)((DisplayObject *)D_8015B7A0[4])->field_30.h.field_32 + 140) / 5;
        work->center.vz = 0;
        work->half_width = ((s16)((DisplayObject *)D_8015B7A0[1])->field_30.h.field_30 -
                            (s16)((DisplayObject *)D_8015B7A0[4])->field_30.h.field_30) / 2;
        work->half_height = ((s16)((DisplayObject *)D_8015B7A0[2])->field_30.h.field_32 -
                             (s16)((DisplayObject *)D_8015B7A0[0])->field_30.h.field_32) / 2;
        for (i = 0; i < 5; i++) {
            setVector(&work->targets[i],
                      (s16)((DisplayObject *)D_8015B7A0[i])->field_30.h.field_30 - (s16)(work->center.vx - 24),
                      (s16)((DisplayObject *)D_8015B7A0[i])->field_30.h.field_32 - (s16)(work->center.vy - 28), 0);
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
        func_8014EF2C(32, work->rotations);
        for (i = 0; i < 32; i++) {
            work->ray_scales[i] = 4096;
        }
        work->widths[0] = 2;
        work->widths[1] = 1;
        work->scale = 4096;
        work->tick = 0;
        work->arrival = 0;
        func_8014F010((u8 *)&work->center_color, 1);
        func_8014F010((u8 *)&work->flash_color, 255);
        func_8014F020((u8 *)&work->line_color, 255, 128, 128);
        work->beam_width = 1;
        work->connection_tick = 0;
        work->line_count = 0;
        work->ray_count = 0;
        work->stage = 0;
    } else {
        Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        copyVector(&D_8015B7F8, &work->center);
        PushMatrix();
        func_801531C4(&world);
        func_801513F4(&world, &saved, &position, &rotation, &scale, 0);
        setPolyFT4(packet);
        packet->tpage = D_8015B748.pairs[18][0];
        packet->clut = D_8015B748.pairs[18][1];
        if ((u16)func_8014D3AC((u8 *)&work->center_color)) {
            setRGB0(packet, work->center_color.r, work->center_color.g, work->center_color.b);
            setUV4(packet, 192, 128, 255, 128, 192, 255, 255, 255);
            for (i = 0; i < 2; i++) {
                setVector(&vertices[0], work->half_width * func_8014F524(-1, i + 1), -work->half_height, 0);
                setVector(&vertices[1], 0, -work->half_height, 0);
                setVector(&vertices[2], work->half_width * func_8014F524(-1, i + 1), work->half_height, 0);
                setVector(&vertices[3], 0, work->half_height, 0);
                func_80151218(packet, vertices, 0, 1);
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->line_color)) {
            if (!(work->tick & 1)) {
                color = work->line_color;
                color.r >>= 1;
                color.g >>= 1;
                color.b >>= 1;
            } else {
                func_8014F010((u8 *)&color, 255);
            }
            if (work->arrival == 8) {
                if (work->connection_tick > 0) {
                    func_801575CC((u8 *)&color, (DVECTOR *)&work->targets[0],
                                 (DVECTOR *)&work->targets[2], work->beam_width * 2);
                }
                if (work->connection_tick > 4) {
                    func_801575CC((u8 *)&color, (DVECTOR *)&work->targets[2],
                                 (DVECTOR *)&work->targets[4], work->beam_width);
                }
                if (work->connection_tick > 8) {
                    func_801575CC((u8 *)&color, (DVECTOR *)&work->targets[1],
                                 (DVECTOR *)&work->targets[4], work->beam_width);
                }
                if (work->connection_tick > 12) {
                    func_801575CC((u8 *)&color, (DVECTOR *)&work->targets[1],
                                 (DVECTOR *)&work->targets[3], work->beam_width);
                }
                if (work->connection_tick > 16) {
                    func_801575CC((u8 *)&color, (DVECTOR *)&work->targets[0],
                                 (DVECTOR *)&work->targets[3], work->beam_width * 2);
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
                        func_80153F98((u8 *)&work->center_color, 255, 255, 255, 1);
                    }
                }
            }
            if (!(work->tick & 1)) {
                color = work->line_color;
            } else {
                func_8014F010((u8 *)&color, 255);
            }
            for (i = 0; i < 5; i++) {
                func_80155BC0((u8 *)&color, 16, 0, &work->points[i]);
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
                line_packet->attribute = 0x50000000;
                setRGB0(line_packet, 255, 128, 128);
                setRGB1(line_packet, 0, 0, 0);
                RotTransPers(&work->heads[i], (PSXLONG *)&line_packet->x0, (PSXLONG *)&p, (PSXLONG *)&flag);
                RotTransPers(&work->tails[i], (PSXLONG *)&line_packet->x1, (PSXLONG *)&p, (PSXLONG *)&flag);
                line_packet->x0 += D_8015B7F8.vx;
                line_packet->x1 += D_8015B7F8.vx;
                line_packet->y0 += D_8015B7F8.vy;
                line_packet->y1 += D_8015B7F8.vy;
                if (flag >= 0) {
                    GsSortGLine(line_packet, D_8015B7F4, D_8015B800);
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
            func_8014F020((u8 *)&color, 255, 128, 128);
            setVector(&scale, work->scale, work->scale, work->scale);
            func_801513F4(&world, &saved, &position, &rotation, &scale, 0);
            func_80155BC0((u8 *)&color, 8, 0, &position);
            func_8014F020((u8 *)&color, 127, 64, 64);
            for (i = 0; i < work->ray_count; i++) {
                setVector(&scale, 4096, work->ray_scales[i], 4096);
                func_801513F4(&world, &saved, &position, &work->rotations[i], &scale, 0);
                func_80155D90((u8 *)&color, work->widths, 32, 0);
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
            work->scale >= 0x6000 && (*(u32 *)&work->center_color & 0xFFFFFF) == 0xFFFFFF &&
            work->stage < 2) {
            work->stage = 2;
        }
        if ((u16)func_8014D3AC((u8 *)&work->flash_color) && work->stage == 0) {
            func_801556F4((u8 *)&work->flash_color, 15);
        }
        if (work->stage == 2) {
            func_80155864((u8 *)&work->flash_color);
            if ((u16)func_80153F98((u8 *)&work->flash_color, 192, 192, 192, 4)) {
                func_8014F010((u8 *)&work->flash_color, 255);
                D_8009B264->field_1D = 1;
                work->stage = 3;
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->flash_color) && work->stage == 3) {
            func_801556F4((u8 *)&work->flash_color, 0);
        }
        work->tick++;
        PopMatrix();
        if (work->stage == 3) {
            D_8009B261 = 1;
        }
    }
}
