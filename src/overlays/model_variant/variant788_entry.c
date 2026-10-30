#include "../../types.h"
#include "variant788_entry.h"

static const VECTOR D_8013D3D0 = {4096, 4096, 4096, 0};

#define FADE_IN(value, target)                                     \
    if (value < work->config->target - 15) {                       \
        value += work->config->fade_in;                            \
    } else {                                                       \
        value = work->config->target;                              \
    }

#define DIVIDE_VECTOR(v, d)                                        \
    ((v)->vx /= (d), (v)->vy /= (d), (v)->vz /= (d))

#define FADE_OUT(value)                                            \
    if (value >= 16) {                                             \
        value -= work->config->fade_out;                           \
    } else {                                                       \
        value = 0;                                                 \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model788State *work;
    MATRIX base, matrix, saved;
    POLY_FT4 quad;
    GsGLINE line;
    POLY_G4 band;
    POLY_G3 fan;
    POLY_F4 flash;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR center;
    SVECTOR vertices[4];
    DVECTOR screen[2][64];
    u16 projection[2][64], flags[2][64], depths[2][64];
    DVECTOR dot_screen[128];
    u16 dot_projection[128], dot_flags[128], dot_depths[128];
    PSXLONG flag, interpolation;
    s32 direction, step;
    GsOT *ot;
    s32 slot, i;
    s32 a, b, c;
    s32 length;
    u16 half;
    s32 depth;
    s32 otz;
    s32 size;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D3D0;
    memset(&center, 0, 8);
    work = (Model788State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D3FC[command];
        Model_CopySlotPosition(slot ^ 1, (u16 *)&work->position);
        work->position.vy = 0;
        work->radius = work->config->radius;
        for (i = 0; i < work->config->line_count; i++) {
            a = rand();
            b = rand();
            c = rand();
            setVector(&work->inner[i], work->config->outer_radius * ccos(a) / 4096, work->config->outer_radius * csin(b) / 4096,
                      work->config->outer_radius * ccos(c) / 4096);
            copyVector(&work->inner_velocity[i], &work->inner[i]);
            DIVIDE_VECTOR(&work->inner_velocity[i], -work->config->line_duration);
            setVector(&work->outer[i], (work->config->outer_radius - work->config->inner_radius) * ccos(a) / 4096, (work->config->outer_radius - work->config->inner_radius) * csin(b) / 4096,
                      (work->config->outer_radius - work->config->inner_radius) * ccos(c) / 4096);
            copyVector(&work->outer_velocity[i], &work->outer[i]);
            DIVIDE_VECTOR(&work->outer_velocity[i], -work->config->line_duration);
        }
        for (i = 0; i < work->config->band_count; i++) {
            setVector(&work->spins[i], (rand() - rand()) % 4096, (rand() - rand()) % 4096,
                      (rand() - rand()) % 4096);
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->ring[i], work->radius * ccos(i << 8) / 4096, 0, work->radius * csin(i << 8) / 4096);
        }
        for (i = 0; i < work->config->dot_count; i++) {
            setVector(&work->dots[i], work->config->dot_spread * (rand() % 4096 - rand() % 4096) / 4096,
                      work->config->dot_spread * (rand() % 4096 - rand() % 4096) / 4096,
                      work->config->dot_spread * (rand() % 4096 - rand() % 4096) / 4096);
            setVector(&work->dot_velocity[i], work->dots[i].vx / work->config->dot_duration,
                      work->dots[i].vy / work->config->dot_duration, work->dots[i].vz / work->config->dot_duration);
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D3E0[i]);
        }
        work->band_bottom = work->config->band_width;
        work->band_top = work->config->band_top;
        work->band_height = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->count = 0;
        work->band_depth = work->radius;
        work->ambient = 0x800;
        work->beam_length = work->config->beam_length;
        work->beam_width = work->config->beam_width;
        work->beam_half = work->config->beam_half;
        work->quad[0] = 1;
        work->quad[1] = 1;
        work->quad[2] = 1;
        work->ring_colors[0][0] = 1;
        work->ring_colors[1][0] = 1;
        work->ring_colors[2][0] = 1;
        work->ring_colors[0][1] = 1;
        work->ring_colors[1][1] = 1;
        work->ring_colors[2][1] = 1;
        work->band_colors[0][0] = 1;
        work->band_colors[1][0] = 1;
        work->band_colors[2][0] = 1;
        work->band_colors[0][1] = 1;
        work->band_colors[1][1] = 1;
        work->band_colors[2][1] = 1;
        work->flash = 0xFF;
        work->box[0] = work->config->colors[0][0];
        work->box[1] = work->config->colors[0][1];
        work->box[2] = work->config->colors[0][2];
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rotation, -16);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rotation, 32);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rotation, 32);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step << 6;
        } else {
            work->ambient = -0x200;
        }
    } else {
        work->ambient = 0x800;
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
    position.vx = matrix.t[0];
    position.vy = matrix.t[1];
    position.vz = matrix.t[2];
    if (*(u32 *)work->quad & 0xFFFFFF) {
        if (work->phase == 0) {
            setPolyFT4(&quad);
            setRGB0(&quad, work->quad[0], work->quad[1], work->quad[2]);
            quad.tpage = work->texture[0] >> 16;
            quad.clut = work->texture[0];
            setVector(&vertices[0], -work->config->half_size, -work->config->half_size, 0);
            setVector(&vertices[1], work->config->half_size, -work->config->half_size, 0);
            setVector(&vertices[2], -work->config->half_size, work->config->half_size, 0);
            setVector(&vertices[3], work->config->half_size, work->config->half_size, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                                (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation,
                                &flag);
            setUV4(&quad, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            if (work->elapsed < work->config->fade_end) {
                FADE_IN(work->quad[0], colors[0][0]);
                FADE_IN(work->quad[1], colors[0][1]);
                FADE_IN(work->quad[2], colors[0][2]);
                FADE_IN(work->ring_colors[0][0], colors[0][0]);
                FADE_IN(work->ring_colors[1][0], colors[0][1]);
                FADE_IN(work->ring_colors[2][0], colors[0][2]);
                FADE_IN(work->ring_colors[0][1], colors[1][0]);
                FADE_IN(work->ring_colors[1][1], colors[1][1]);
                FADE_IN(work->ring_colors[2][1], colors[1][2]);
                FADE_IN(work->band_colors[0][0], colors[2][0]);
                FADE_IN(work->band_colors[1][0], colors[2][1]);
                FADE_IN(work->band_colors[2][0], colors[2][2]);
                FADE_IN(work->band_colors[0][1], colors[3][0]);
                FADE_IN(work->band_colors[1][1], colors[3][1]);
                FADE_IN(work->band_colors[2][1], colors[3][2]);
            } else {
                FADE_OUT(work->quad[0]);
                FADE_OUT(work->quad[1]);
                FADE_OUT(work->quad[2]);
                work->beam_length -= work->config->beam_shrink;
                if (work->beam_length < 0) {
                    work->beam_length = 0;
                }
            }
        }
    }
    if (work->phase == 0) {
        setRGB0(&line, work->config->colors[0][0], work->config->colors[0][1], work->config->colors[0][2]);
        setRGB1(&line, 0, 0, 0);
        line.attribute = 0x50000000;
        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->inner, screen[0], depths[0], projection[0], flags[0], work->count);
        RotTransPersN(work->outer, screen[1], depths[1], projection[1], flags[1], work->count);
        line.attribute = 0x50000000;
        flag = 0;
        for (i = 0; i < work->count; i++) {
            half = work->config->half_size;
            length = work->outer[i].vx;
            if (length < 0) {
                length = -length;
            }
            if (half >> 1 < length) {
                line.x0 = screen[0][i].vx;
                line.y0 = screen[0][i].vy;
                line.x1 = screen[1][i].vx;
                line.y1 = screen[1][i].vy;
                otz = (depths[0][i] + depths[1][i]) >> 1;
                if (otz != 0) {
                    GsSortGLine(&line, ot, otz);
                }
                addVector(&work->inner[i], &work->inner_velocity[i]);
                addVector(&work->outer[i], &work->outer_velocity[i]);
            }
        }
        if (work->count != 0) {
            setPolyG4(&band);
            /* Explicit guard: placing size's set here keeps it in the loop preheader. */
            i = 0;
            if (i < work->config->band_count) {
                size = 0x1000;
                /*
                 * Dead store: the branch outlives combine, so the loop-hoisted copy
                 * of size stays a second set and size keeps its saved register.
                 */
                if (work->config->band_count == 0) {
                    depth = 0;
                }
                for (; i < work->config->band_count; i++) {
                    setRGB0(&band, work->band_colors[0][0], work->band_colors[1][0], work->band_colors[2][0]);
                    setRGB1(&band, work->band_colors[0][0], work->band_colors[1][0], work->band_colors[2][0]);
                    setRGB2(&band, 0, 0, 0);
                    setRGB3(&band, 0, 0, 0);
                    size = 0x1000;
                    setVector(&scale, size, size, size);
                    rotation.vx = 0;
                    rotation.vy = 4096 / work->config->band_count * i;
                    rotation.vz = 0;
                    rotation.vy += work->elapsed * work->config->spin;
                    GsSetLsMatrix(&base);
                    RotTrans(&work->position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    MulMatrix2(&base, &matrix);
                    saved = matrix;
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    setVector(&vertices[0], -work->config->band_width * 2, 0, -direction * work->radius / 4);
                    setVector(&vertices[1], work->config->band_width * 2, 0, -direction * work->radius / 4);
                    setVector(&vertices[2], -work->config->band_top * 2, -work->band_height, -direction * work->band_depth);
                    setVector(&vertices[3], work->config->band_top * 2, -work->band_height, -direction * work->band_depth);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&band.x0,
                                        (PSXLONG *)&band.x1, (PSXLONG *)&band.x2, (PSXLONG *)&band.x3,
                                        &interpolation, &flag);
                    Graphics_SubmitDrawModePacket((u32 *)&band, ot, (u16)depth, 1);
                    setRGB0(&band, work->band_colors[0][1], work->band_colors[1][1], work->band_colors[2][1]);
                    setRGB1(&band, work->band_colors[0][1], work->band_colors[1][1], work->band_colors[2][1]);
                    setRGB2(&band, 0, 0, 0);
                    setRGB3(&band, 0, 0, 0);
                    setVector(&vertices[0], -work->band_bottom, 0, -direction * work->radius / 4);
                    setVector(&vertices[1], work->band_bottom, 0, -direction * work->radius / 4);
                    setVector(&vertices[2], -work->band_top, -work->band_height, -direction * work->band_depth / 2);
                    setVector(&vertices[3], work->band_top, -work->band_height, -direction * work->band_depth / 2);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&band.x0,
                                        (PSXLONG *)&band.x1, (PSXLONG *)&band.x2, (PSXLONG *)&band.x3,
                                        &interpolation, &flag);
                    Graphics_SubmitDrawModePacket((u32 *)&band, ot, (u16)depth, 1);
                }
            }
            setPolyG3(&fan);
            setRGB0(&fan, work->ring_colors[0][0], work->ring_colors[1][0], work->ring_colors[2][0]);
            setRGB1(&fan, 0, 0, 0);
            setRGB2(&fan, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < 16; i++) {
                if (i == 15) {
                    depth = RotAverage3(&center, &work->ring[15], &work->ring[0], (PSXLONG *)&fan.x0,
                                        (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
                } else {
                    depth = RotAverage3(&center, &work->ring[i], &work->ring[i + 1], (PSXLONG *)&fan.x0,
                                        (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
                }
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
            setVector(&scale, 0x800, 0x800, 0x800);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setRGB0(&fan, work->ring_colors[0][1], work->ring_colors[1][1], work->ring_colors[2][1]);
            setRGB1(&fan, 0, 0, 0);
            setRGB2(&fan, 0, 0, 0);
            for (i = 0; i < 16; i++) {
                if (i == 15) {
                    depth = RotAverage3(&center, &work->ring[15], &work->ring[0], (PSXLONG *)&fan.x0,
                                        (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
                } else {
                    depth = RotAverage3(&center, &work->ring[i], &work->ring[i + 1], (PSXLONG *)&fan.x0,
                                        (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
                }
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
            work->band_height += work->config->growth * 2;
            if ((u16)work->band_height > 0x578) {
                work->band_height = 0x578;
            }
            work->band_depth -= work->config->growth;
            if (work->band_depth < work->radius >> 2) {
                work->band_depth = work->radius >> 2;
            }
            work->band_bottom += work->config->growth / 4;
            if (work->band_bottom > work->config->band_width * 4) {
                work->band_bottom = work->config->band_width * 4;
            }
            work->band_top += work->config->growth / 4;
            if (work->band_top > work->config->band_width * 4) {
                work->band_top = work->config->band_width * 4;
            }
        }
        if (work->beam_length > 0 && work->beam_width > 0) {
            setPolyG4(&band);
            setRGB0(&band, 0, 0, 0);
            setRGB1(&band, 0, 0, 0);
            setRGB2(&band, work->quad[0], work->quad[1], work->quad[2]);
            setRGB3(&band, work->quad[0], work->quad[1], work->quad[2]);
            setVector(&vertices[0], 0, -work->beam_half, direction * work->beam_length);
            setVector(&vertices[1], 0, work->beam_half, direction * work->beam_length);
            setVector(&vertices[2], 0, -work->beam_width, 0);
            setVector(&vertices[3], 0, work->beam_width, 0);
            for (i = 0; i < work->count / (work->config->line_count / work->config->band_count); i++) {
                copyVector(&rotation, &work->spins[i]);
                rotation.vx += work->elapsed * work->config->spin;
                rotation.vy += work->elapsed * work->config->spin;
                rotation.vz += work->elapsed * work->config->spin;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&band.x0,
                                    (PSXLONG *)&band.x1, (PSXLONG *)&band.x2, (PSXLONG *)&band.x3,
                                    &interpolation, &flag);
                Graphics_SubmitDrawModePacket((u32 *)&band, ot, (u16)depth, 1);
            }
        }
        work->count += step;
        if (work->count > work->config->line_count) {
            work->count = work->config->line_count;
        }
    }
    if (work->phase != 0) {
        if (work->flash != 0) {
            setPolyF4(&flash);
            setRGB0(&flash, work->flash, work->flash, work->flash);
            setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 0, 1);
            FADE_OUT(work->flash);
        }
        if (*(u32 *)work->box & 0xFFFFFF) {
            box.h = 1;
            box.w = 1;
            box.r = work->box[0];
            box.g = work->box[1];
            box.b = work->box[2];
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&rotation, 0, 0, 0);
            setVector(&position, 0, -350, direction * 450);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->dots, dot_screen, dot_depths, dot_projection, dot_flags, work->config->dot_count);
            box.attribute = 0x40000000;
            flag = 0;
            for (i = 0; i < work->config->dot_count; i++) {
                box.x = dot_screen[i].vx;
                box.y = dot_screen[i].vy;
                if (dot_depths[i] != 0) {
                    otz = dot_depths[i];
                    GsSortBoxFill(&box, ot, otz >> 2);
                }
                addVector(&work->dots[i], &work->dot_velocity[i]);
            }
            setPolyFT4(&quad);
            setRGB0(&quad, work->box[0], work->box[1], work->box[2]);
            quad.tpage = work->texture[0] >> 16;
            quad.clut = work->texture[0];
            setVector(&vertices[0], -work->config->box_half, -work->config->box_half, 0);
            setVector(&vertices[1], work->config->box_half, -work->config->box_half, 0);
            setVector(&vertices[2], -work->config->box_half, work->config->box_half, 0);
            setVector(&vertices[3], work->config->box_half, work->config->box_half, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&quad.x0,
                                (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation,
                                &flag);
            setUV4(&quad, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            FADE_OUT(work->box[0]);
            FADE_OUT(work->box[1]);
            FADE_OUT(work->box[2]);
        }
    }
    if (work->elapsed >= work->config->finish && work->phase == 0) {
        work->phase = 1;
    }
    work->elapsed += step;
    PopMatrix();
    if (!(*(u32 *)work->box & 0xFFFFFF)) {
        return 2;
    }
    if (work->phase == 1) {
        work->phase = 2;
        return 1;
    }
    if (work->elapsed > work->config->start && work->phase == 0) {
        return 4;
    }
    return 0;
}
