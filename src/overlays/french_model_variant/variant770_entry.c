#include "../../types.h"
#include "variant770_entry.h"

static const VECTOR D_8013C6E8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model770State *work;
    s32 slot, direction;
    MATRIX base, matrix;
    POLY_G4 gouraud;
    POLY_G4 *gouraud_packet;
    POLY_FT4 quad;
    POLY_FT4 *textured_packet;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    u16 projection[64], flags[64];
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    s32 step;
    GsOT *ot;
    SVECTOR *point, *other;
    DVECTOR *screen;
    s32 i, j, k;
    u16 side;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C6E8;
    work = (Model770State *)context;
    gouraud_packet = &gouraud;
    textured_packet = &quad;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C714[command];
        point = work->trail_first;
        other = work->trail_second;
        for (i = 0; i < 32; point++, other++, i++) {
            setVector(point, 0, 0, 0);
            setVector(other, 0, 0, 0);
        }
        work->trail_color.rgb.r = work->config->trail_r;
        work->trail_color.rgb.g = work->config->trail_g;
        work->trail_color.rgb.b = work->config->trail_b;
        work->level = 2048;
        work->trail_count = 0;
        work->elapsed = 0;
        work->phase = 0;
        point = work->sprite_offsets;
        for (i = 0; i < work->config->sprite_count; point++, i++) {
            point->vx = work->config->sprite_spread * (rand() % 4096 - rand() % 4096) / 4096;
            point->vy = work->config->sprite_spread * (rand() % 4096 - rand() % 4096) / 4096;
            point->vz = 0;
            work->sprite_frames[i] = 8;
            setVector(&work->sprite_origins[i], 0, 0, 0);
        }
        point = &work->quad[0];
        setVector(point, -work->config->sprite_size, -work->config->sprite_size, 0);
        point = &work->quad[1];
        setVector(point, work->config->sprite_size, -work->config->sprite_size, 0);
        point = &work->quad[2];
        setVector(point, -work->config->sprite_size, work->config->sprite_size, 0);
        point = &work->quad[3];
        setVector(point, work->config->sprite_size, work->config->sprite_size, 0);
        work->burst_tpage = 0xAE;
        work->burst_clut = 0x3D28;
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C6F8[i]);
        }
        point = work->positions;
        other = work->velocities;
        screen = work->screen;
        for (i = 0; i < work->config->dot_count; point++, other++, screen++, i++) {
            setVector(point, 0, 0, 0);
            k = rand() % 4096;
            j = rand() % 4096;
            other->vx = (work->config->dot_radius * ccos(k) / 4096) * ccos(j) / 4096;
            other->vy = (work->config->dot_radius * csin(k) / 4096) * ccos(j) / 4096;
            other->vz = (work->config->dot_radius * csin(k) / 4096) * ccos(j) / 4096;
            screen->vx = screen->vy = 0;
            work->depths[i] = 0;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->inner_ring[i], ccos(i * 256) / 128, csin(i * 256) / 128, 0);
            setVector(&work->outer_ring[i], ccos(i * 256) / 32, csin(i * 256) / 32, -direction * 32);
        }
        work->burst_color.rgb.r = 255;
        work->burst_color.rgb.g = 160;
        work->burst_color.rgb.b = 160;
        work->scale = 4096;
        work->active_sprites = 0;
        work->previous_sprites = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step * 64;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 1) {
        if (work->level < 2048) {
            work->level += step * 64;
        } else {
            work->level = 2048;
            work->phase = 2;
        }
    }
    func_800595C8(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->initial_delay) {
        work->elapsed += step;
        return 0;
    }
    setPolyG4(gouraud_packet);
    setPolyFT4(textured_packet);
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->trail_duration && work->trail_count < 32) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->first_part), &matrix);
        point = work->trail_first;
        setVector(&point[work->trail_count], matrix.t[0], matrix.t[1], matrix.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->second_part), &matrix);
        other = work->trail_second;
        setVector(&other[work->trail_count], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->trail_count++;
    }
    point = work->trail_first;
    other = work->trail_second;
    setRGB2(gouraud_packet, 0, 0, 0);
    setRGB3(gouraud_packet, 0, 0, 0);
    if (work->trail_count >= 2 && (work->trail_color.word & 0xFFFFFF)) {
        for (i = work->trail_count - 1, j = 1; i > 0; i--, j++) {
            setVector(&position, 0, 0, 0);
            setRGB0(gouraud_packet, work->trail_color.rgb.r / j,
                work->trail_color.rgb.g / j, work->trail_color.rgb.b / j);
            setRGB1(gouraud_packet, work->trail_color.rgb.r / (j + 1),
                work->trail_color.rgb.g / (j + 1), work->trail_color.rgb.b / (j + 1));
            for (k = 0; k < work->config->trail_layers; k++) {
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&point[i], &point[i - 1], &other[i], &other[i - 1],
                    (PSXLONG *)&gouraud_packet->x0, (PSXLONG *)&gouraud_packet->x1,
                    (PSXLONG *)&gouraud_packet->x2, (PSXLONG *)&gouraud_packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    func_8005B260((u32 *)gouraud_packet, ot, (u16)depth, 1);
                }
                position.vy += work->config->trail_layer_step;
            }
        }
        if (work->phase != 0) {
            if (work->trail_color.rgb.r > 15) work->trail_color.rgb.r -= 15;
            else work->trail_color.rgb.r = 0;
            if (work->trail_color.rgb.g > 15) work->trail_color.rgb.g -= 15;
            else work->trail_color.rgb.g = 0;
            if (work->trail_color.rgb.b > 15) work->trail_color.rgb.b -= 15;
            else work->trail_color.rgb.b = 0;
        }
    }
    if (work->elapsed >= work->config->sprite_delay) {
        point = work->quad;
        other = work->sprite_offsets;
        textured_packet->tpage = work->texture[0] >> 16;
        textured_packet->clut = work->texture[0];
        setRGB0(textured_packet, work->config->sprite_r, work->config->sprite_g, work->config->sprite_b);
        work->active_sprites += step * 3;
        if (work->active_sprites > work->config->sprite_count) {
            work->active_sprites = work->config->sprite_count;
        }
        for (i = work->active_sprites; i > work->previous_sprites; i--) {
            setVector(&work->sprite_origins[i - 1],
                (work->trail_first + work->trail_count - 1)->vx,
                (work->trail_first + work->trail_count - 1)->vy,
                (work->trail_first + work->trail_count - 1)->vz);
        }
        work->previous_sprites = work->active_sprites;
        for (i = 0; i < work->active_sprites; i++) {
            if (work->sprite_frames[i]) {
                copyVector(&position, &work->sprite_origins[i]);
                GsSetLsMatrix(&base);
                position.vx += other[i].vx;
                position.vy += other[i].vy;
                position.vz += other[i].vz;
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&point[0], &point[1], &point[2], &point[3],
                    (PSXLONG *)&textured_packet->x0, (PSXLONG *)&textured_packet->x1,
                    (PSXLONG *)&textured_packet->x2, (PSXLONG *)&textured_packet->x3, &interpolation, &flag);
                setUV4(textured_packet, (8 - work->sprite_frames[i]) * 32, 0,
                    (8 - work->sprite_frames[i]) * 32 + 31, 0,
                    (8 - work->sprite_frames[i]) * 32, 31,
                    (8 - work->sprite_frames[i]) * 32 + 31, 31);
                if (depth >= 0) {
                    func_8005B260((u32 *)textured_packet, ot, (u16)depth, 1);
                }
                work->sprite_frames[i]--;
            }
        }
    }
    if (work->elapsed > work->config->burst_delay) {
        SVECTOR *velocity;
        if (work->burst_color.word & 0xFFFFFF) {
            setVector(&position, 0, -350, direction * 450);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, work->scale, work->scale, work->scale);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            textured_packet->tpage = work->burst_tpage;
            textured_packet->clut = work->burst_clut;
            setUV4(textured_packet, 0, 128, 31, 128, 0, 255, 31, 255);
            setRGB0(textured_packet, work->burst_color.rgb.r / 2,
                work->burst_color.rgb.g / 2, work->burst_color.rgb.b / 2);
            for (i = 0; i < 16; i++) {
                depth = RotAverage4(&work->inner_ring[i], &work->inner_ring[(i + 1) % 16],
                    &work->outer_ring[i], &work->outer_ring[(i + 1) % 16],
                    (PSXLONG *)&textured_packet->x0, (PSXLONG *)&textured_packet->x1,
                    (PSXLONG *)&textured_packet->x2, (PSXLONG *)&textured_packet->x3, &interpolation, &flag);
                setSemiTrans(textured_packet, 1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(textured_packet, ot, (u16)depth);
                }
            }
            setRGB0(textured_packet, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setUV4(textured_packet, 64, 64, 127, 64, 64, 127, 127, 127);
            side = 1;
            for (i = 0; i < 4; i++) {
                s32 next_side = -(s16)side;
                s32 first_x = -(s16)next_side * 64;
                side = next_side;
                setVector(&vertices[0], first_x, i - 2 >= 0 ? -64 : 64, 0);
                setVector(&vertices[1], 0, i - 2 >= 0 ? -64 : 64, 0);
                setVector(&vertices[2], -(s16)side * 64, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&textured_packet->x0, (PSXLONG *)&textured_packet->x1,
                    (PSXLONG *)&textured_packet->x2, (PSXLONG *)&textured_packet->x3, &interpolation, &flag);
                if (depth >= 0) {
                    func_8005B260((u32 *)textured_packet, ot, (u16)depth, 1);
                }
            }
            setUV4(textured_packet, 64, 0, 127, 0, 64, 63, 127, 63);
            side = 1;
            for (i = 0; i < 4; i++) {
                s32 next_side = -(s16)side;
                s32 first_x = -(s16)next_side * 32;
                side = next_side;
                setVector(&vertices[0], first_x, i - 2 >= 0 ? -32 : 32, 0);
                setVector(&vertices[1], 0, i - 2 >= 0 ? -32 : 32, 0);
                setVector(&vertices[2], -(s16)side * 32, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&textured_packet->x0, (PSXLONG *)&textured_packet->x1,
                    (PSXLONG *)&textured_packet->x2, (PSXLONG *)&textured_packet->x3, &interpolation, &flag);
                if (depth >= 0) {
                    func_8005B260((u32 *)textured_packet, ot, (u16)depth, 1);
                }
            }
            if (work->scale < 0x4000) work->scale += 4096;
            else work->scale += 128;
            if (work->burst_color.rgb.r > 15) work->burst_color.rgb.r -= 15;
            else work->burst_color.rgb.r = 0;
            if (work->burst_color.rgb.g > 15) work->burst_color.rgb.g -= 15;
            else work->burst_color.rgb.g = 0;
            if (work->burst_color.rgb.b > 15) work->burst_color.rgb.b -= 15;
            else work->burst_color.rgb.b = 0;
        }
        box.w = box.h = 1;
        box.r = 128;
        box.g = 32;
        box.b = 32;
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        point = work->positions;
        screen = work->screen;
        box.attribute = 0x40000000;
        for (i = 0; i < work->config->dot_count; point++, screen++, i++) {
            if (point->vy < 350) {
                box.x = screen->vx;
                box.y = screen->vy;
                if (work->depths[i]) {
                    GsSortBoxFill(&box, ot, work->depths[i] / 4);
                }
            }
        }
        point = work->positions;
        screen = work->screen;
        RotTransPersN(point, screen, work->depths, projection, flags, work->config->dot_count);
        box.attribute = 0;
        flag = 0;
        for (i = 0; i < work->config->dot_count; point++, screen++, i++) {
            if (point->vy < 350) {
                box.x = screen->vx;
                box.y = screen->vy;
                if (work->depths[i]) {
                    GsSortBoxFill(&box, ot, work->depths[i] / 4);
                }
                flag = 1;
            }
        }
        gteMIMefunc(work->positions, work->velocities, work->config->dot_count, 2048);
        velocity = work->velocities;
        for (i = 0; i < work->config->dot_count; velocity++, i++) {
            velocity->vy += ((350 - velocity->vy) / work->config->dot_radius) / 2;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->elapsed < work->config->trail_duration && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->phase >= 2 && !(work->trail_color.word & 0xFFFFFF)
        && !(work->burst_color.word & 0xFFFFFF)) {
        return 2;
    }
    return 0;
}
