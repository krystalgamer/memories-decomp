#include "../../types.h"
#include "variant759_entry.h"

/* Header 759's entry: the ambient light dims while an animated glow plays
 * under the slot, two pairs of tilted colour waves fade out, an animated
 * flash flickers and a ring of expanding strips swells towards the other
 * slot; then the glow and the waves play again and the light comes back. */

static const VECTOR gModelVariant759_D1DD4 = {4096, 4096, 4096, 0};

s32 ModelVariant759_HandleRequest(u8 *context, s32 command)
{
    MATRIX light;
    MATRIX m;
    POLY_FT4 glow;
    POLY_FT4 flash;
    POLY_FT4 ring;
    POLY_G4 wave;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    PSXLONG flag;
    PSXLONG p;
    Model759State *work;
    s32 slot, side, step;
    GsOT *ot;
    s32 i;
    s32 otz;
    s32 sx, sy, sz;
    SVECTOR *v;
    SVECTOR *w;
    s16 (*nc)[4];
    s16 (*fc)[4];

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant759_D1DD4;
    work = (Model759State *)context;
    slot = Model_GetActiveSlotIndex();
    side = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant759_D1E38[command];
        v = &work->glow[0];
        setVector(v, -work->config->glow_size, -work->config->glow_size, 0);
        v = &work->glow[1];
        setVector(v, work->config->glow_size, -work->config->glow_size, 0);
        v = &work->glow[2];
        setVector(v, -work->config->glow_size, work->config->glow_size, 0);
        v = &work->glow[3];
        setVector(v, work->config->glow_size, work->config->glow_size, 0);
        v = &work->wave[0];
        setVector(v, -work->config->wave_length, 0, 0);
        v = &work->wave[1];
        setVector(v, 0, -work->config->wave_height, 0);
        v = &work->wave[2];
        setVector(v, 0, work->config->wave_height, 0);
        v = &work->wave[3];
        setVector(v, work->config->wave_length, 0, 0);
        v = &work->wave2[0];
        setVector(v, -work->config->wave_length, 0, 0);
        v = &work->wave2[1];
        setVector(v, 0, -(s16)work->config->wave_height / 2, 0);
        v = &work->wave2[2];
        setVector(v, 0, (s16)work->config->wave_height / 2, 0);
        v = &work->wave2[3];
        setVector(v, work->config->wave_length, 0, 0);
        v = &work->flash[0];
        setVector(v, -work->config->flash_size, -work->config->flash_size, 0);
        v = &work->flash[1];
        setVector(v, work->config->flash_size, -work->config->flash_size, 0);
        v = &work->flash[2];
        setVector(v, -work->config->flash_size, work->config->flash_size, 0);
        v = &work->flash[3];
        setVector(v, work->config->flash_size, work->config->flash_size, 0);
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(slot, 1, &gModelVariant759_D1DE4[i]);
        }
        Model_GetSlotDimensionsWithMinimums(-slot + 1, (ModelEffectAdjustment *)&pos);
        v = work->near_ring.v;
        w = work->far_ring.v;
        nc = work->near_ring.c;
        fc = work->far_ring.c;
        /* vx is written through the vector view and vy/vz through the word
         * view of the same ring storage; walking the two views in lockstep
         * keeps them on the separate strength-reduced pointers. */
        if (pos.vx > pos.vz) {
            for (i = 0; i < 9; nc++, v++, fc++, w++, i++) {
                v->vx = pos.vx * (ccos(i * 256) % 4096) / 4096;
                (*nc)[1] = 0;
                (*nc)[2] = side * pos.vx * (csin(i * 256) % 4096) / 4096;
                w->vx = pos.vx * (ccos(i * 256) % 4096 << 1) / 4096;
                (*fc)[1] = -work->config->ring_height;
                (*fc)[2] = side * (pos.vx << 1) * (csin(i * 256) % 4096) / 4096;
            }
        } else {
            for (i = 0; i < 9; nc++, v++, fc++, w++, i++) {
                v->vx = pos.vz * (ccos(i * 256) % 4096) / 4096;
                (*nc)[1] = 0;
                (*nc)[2] = side * pos.vz * (csin(i * 256) % 4096) / 4096;
                w->vx = pos.vz * (ccos(i * 256) % 4096 << 1) / 4096;
                (*fc)[1] = -work->config->ring_height;
                (*fc)[2] = side * (pos.vz << 1) * (csin(i * 256) % 4096) / 4096;
            }
        }
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->glow_frame[0] = 0;
        work->glow_frame[1] = 0;
        work->flash_frame[0] = 0;
        work->flash_frame[1] = 0;
        work->colors[0][0][0] = work->config->first[0];
        work->colors[0][1][0] = work->config->first[1];
        work->colors[0][2][0] = work->config->first[2];
        work->colors[0][0][1] = work->config->first[0];
        work->colors[0][1][1] = work->config->first[1];
        work->colors[0][2][1] = work->config->first[2];
        work->colors[1][0][0] = work->config->second[0];
        work->colors[1][1][0] = work->config->second[1];
        work->colors[1][2][0] = work->config->second[2];
        work->colors[1][0][1] = work->config->second[0];
        work->colors[1][1][1] = work->config->second[1];
        work->colors[1][2][1] = work->config->second[2];
        work->done[0][0][0] = 0;
        work->done[0][0][1] = 0;
        work->done[0][0][2] = 0;
        work->done[0][1][0] = 0;
        work->done[0][1][1] = 0;
        work->done[0][1][2] = 0;
        work->done[1][0][0] = 0;
        work->done[1][0][1] = 0;
        work->done[1][0][2] = 0;
        work->done[1][1][0] = 0;
        work->done[1][1][1] = 0;
        work->done[1][1][2] = 0;
        work->ring_alpha = 0x80;
        work->ring_count = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->ambient = 0x800;
            work->phase = 2;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    v = work->glow;
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyFT4(&glow);
    SetPolyFT4(&flash);
    SetPolyG4(&wave);
    flash.tpage = work->texture[1] >> 16;
    flash.clut = work->texture[1];
    glow.tpage = work->texture[2] >> 16;
    glow.clut = work->texture[2];
    glow.r0 = work->config->r;
    glow.g0 = work->config->g;
    glow.b0 = work->config->b;
    pos.vx = 0;
    pos.vy = -350;
    pos.vz = side * 450;
    GsSetLsMatrix(&light);
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    otz = RotAverage4(&work->glow[0], &work->glow[1], &work->glow[2], &work->glow[3], (PSXLONG *)&glow.x0,
                      (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
    if (work->glow_frame[0] < 8) {
        setUV4(&glow, work->glow_frame[0] % 8 * 32, 0, work->glow_frame[0] % 8 * 32 + 31, 0,
               work->glow_frame[0] % 8 * 32, 31, work->glow_frame[0] % 8 * 32 + 31, 31);
        SetSemiTrans(&glow, 1);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
        }
        work->glow_frame[0]++;
    }
    if (work->done[0][0][0] == 0 || work->done[0][0][1] == 0 || work->done[0][0][2] == 0) {
        wave.r0 = 0;
        wave.g0 = 0;
        wave.b0 = 0;
        wave.r1 = work->colors[0][0][0];
        wave.g1 = work->colors[0][1][0];
        wave.b1 = work->colors[0][2][0];
        wave.r2 = work->colors[0][0][0];
        wave.g2 = work->colors[0][1][0];
        wave.b2 = work->colors[0][2][0];
        wave.r3 = 0;
        wave.g3 = 0;
        wave.b3 = 0;
        pos.vx = 0;
        pos.vy = -350;
        pos.vz = side * (450 - work->config->depth);
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = side * work->config->first_tilt;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&work->wave[0], &work->wave[1], &work->wave[2], &work->wave[3], (PSXLONG *)&wave.x0,
                          (PSXLONG *)&wave.x1, (PSXLONG *)&wave.x2, (PSXLONG *)&wave.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&wave, ot, (u16)otz, 1);
        }
        work->colors[0][0][0] -= step * 8;
        if (work->colors[0][0][0] < 31) {
            work->colors[0][0][0] = 0;
            work->done[0][0][0] = 1;
        }
        work->colors[0][1][0] -= step * 8;
        if (work->colors[0][1][0] < 31) {
            work->colors[0][1][0] = 0;
            work->done[0][0][1] = 1;
        }
        work->done[0][0][2] = 1;
    }
    if (work->done[1][0][0] == 0 || work->done[1][0][1] == 0 || work->done[1][0][2] == 0) {
        wave.r0 = 0;
        wave.g0 = 0;
        wave.b0 = 0;
        wave.r1 = work->colors[1][0][0];
        wave.g1 = work->colors[1][1][0];
        wave.b1 = work->colors[1][2][0];
        wave.r2 = work->colors[1][0][0];
        wave.g2 = work->colors[1][1][0];
        wave.b2 = work->colors[1][2][0];
        wave.r3 = 0;
        wave.g3 = 0;
        wave.b3 = 0;
        pos.vx = 0;
        pos.vy = -350;
        pos.vz = side * (450 - work->config->depth);
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = side * work->config->first_tilt;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&work->wave2[0], &work->wave2[1], &work->wave2[2], &work->wave2[3], (PSXLONG *)&wave.x0,
                          (PSXLONG *)&wave.x1, (PSXLONG *)&wave.x2, (PSXLONG *)&wave.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&wave, ot, (u16)otz, 1);
        }
        work->colors[1][0][0] -= step * 8;
        if (work->colors[1][0][0] < 31) {
            work->colors[1][0][0] = 0;
            work->done[1][0][0] = 1;
        }
        work->colors[1][1][0] -= step * 8;
        if (work->colors[1][1][0] < 31) {
            work->colors[1][1][0] = 0;
            work->done[1][0][1] = 1;
        }
        work->colors[1][2][0] -= step * 8;
        if (work->colors[1][2][0] < 31) {
            work->colors[1][2][0] = 0;
            work->done[1][0][2] = 1;
        }
    }
    if (work->flash_frame[0] < 3) {
        flash.r0 = 0x80;
        flash.g0 = 0x80;
        flash.b0 = 0x80;
        pos.vx = 0;
        pos.vy = -350;
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        pos.vz = side * 450;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&work->flash[0], &work->flash[1], &work->flash[2], &work->flash[3], (PSXLONG *)&flash.x0,
                          (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2, (PSXLONG *)&flash.x3, &p, &flag);
        setUV4(&flash, work->flash_frame[0] % 3 * 64, 32, work->flash_frame[0] % 3 * 64 + 63, 32,
               work->flash_frame[0] % 3 * 64, 95, work->flash_frame[0] % 3 * 64 + 63, 95);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 4);
        }
        work->flash_frame[0]++;
    }
    if (work->elapsed < work->config->first_time) {
        work->elapsed += step;
        PopMatrix();
        return 1;
    }
    if (work->elapsed < work->config->second_time) {
        work->elapsed += step;
        PopMatrix();
        return 0;
    }
    if (work->ring_count < 64) {
        SetPolyFT4(&ring);
        ring.r0 = work->ring_alpha;
        ring.g0 = work->ring_alpha;
        ring.b0 = work->ring_alpha;
        ring.tpage = work->texture[0] >> 16;
        ring.clut = work->texture[0];
        setUV4(&ring, 0, 96, 0, 223, 127, 96, 127, 223);
        v = work->near_ring.v;
        w = work->far_ring.v;
        pos.vx = 0;
        pos.vy = 0;
        pos.vz = side * 450;
        sx = work->ring_count * 2048 + 4096;
        sy = sx;
        sz = sx;
        if (sx > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sz > 0x7FCD) {
            sx = 0x7FCD;
        }
        scale.vx = sx;
        scale.vy = sy;
        scale.vz = sz;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 8; i++) {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1,
                              (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)otz, 1);
            }
        }
        v = work->near_ring.v;
        w = work->far_ring.v;
        pos.vx = 0;
        pos.vy = 0;
        pos.vz = side * 450;
        sx = work->ring_count * 512 + 4096;
        sy = work->ring_count * 3072 + 4096;
        sz = sx;
        if (sx > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sy > 0x7FCD) {
            sy = 0x7FCD;
        }
        if (sz > 0x7FCD) {
            sx = 0x7FCD;
        }
        scale.vx = sx;
        scale.vy = sy;
        scale.vz = sz;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 8; i++) {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1,
                              (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)otz, 1);
            }
        }
        v = work->near_ring.v;
        w = work->far_ring.v;
        pos.vx = 0;
        pos.vy = 0;
        pos.vz = side * 450;
        sx = work->ring_count * 1024 + 4096;
        sy = work->ring_count * 2304 + 4096;
        sz = sx;
        if (sx > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sy > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sz > 0x7FCD) {
            sx = 0x7FCD;
        }
        scale.vx = sx;
        scale.vy = sy;
        scale.vz = sz;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 8; i++) {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1,
                              (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)otz, 1);
            }
        }
        work->ring_count++;
        if (work->ring_alpha < 15) {
            work->ring_alpha = 0;
        } else {
            work->ring_alpha -= step * 6;
        }
    }
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    pos.vx = 0;
    pos.vy = -350;
    pos.vz = side * 450;
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    GsSetLsMatrix(&light);
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    otz = RotAverage4(&work->glow[0], &work->glow[1], &work->glow[2], &work->glow[3], (PSXLONG *)&glow.x0,
                      (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
    if (work->glow_frame[1] < 8) {
        setUV4(&glow, work->glow_frame[1] % 8 * 32, 0, work->glow_frame[1] % 8 * 32 + 31, 0,
               work->glow_frame[1] % 8 * 32, 31, work->glow_frame[1] % 8 * 32 + 31, 31);
        SetSemiTrans(&glow, 1);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
        }
        work->glow_frame[1]++;
    }
    if (work->done[0][1][0] == 0 || work->done[0][1][1] == 0 || work->done[0][1][2] == 0) {
        wave.r0 = 0;
        wave.g0 = 0;
        wave.b0 = 0;
        wave.r1 = work->colors[0][0][1];
        wave.g1 = work->colors[0][1][1];
        wave.b1 = work->colors[0][2][1];
        wave.r2 = work->colors[0][0][1];
        wave.g2 = work->colors[0][1][1];
        wave.b2 = work->colors[0][2][1];
        wave.r3 = 0;
        wave.g3 = 0;
        wave.b3 = 0;
        pos.vx = 0;
        pos.vy = -350;
        pos.vz = side * (450 - work->config->depth);
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = side * work->config->second_tilt;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&work->wave[0], &work->wave[1], &work->wave[2], &work->wave[3], (PSXLONG *)&wave.x0,
                          (PSXLONG *)&wave.x1, (PSXLONG *)&wave.x2, (PSXLONG *)&wave.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&wave, ot, (u16)otz, 1);
        }
        work->colors[0][0][1] -= step * 8;
        if (work->colors[0][0][1] < 31) {
            work->colors[0][0][1] = 0;
            work->done[0][1][0] = 1;
        }
        work->colors[0][1][1] -= step * 8;
        if (work->colors[0][1][1] < 31) {
            work->colors[0][1][1] = 0;
            work->done[0][1][1] = 1;
        }
        work->done[0][1][2] = 1;
    }
    if (work->done[1][1][0] == 0 || work->done[1][1][1] == 0 || work->done[1][1][2] == 0) {
        wave.r0 = 0;
        wave.g0 = 0;
        wave.b0 = 0;
        wave.r1 = work->colors[1][0][1];
        wave.g1 = work->colors[1][1][1];
        wave.b1 = work->colors[1][2][1];
        wave.r2 = work->colors[1][0][1];
        wave.g2 = work->colors[1][1][1];
        wave.b2 = work->colors[1][2][1];
        wave.r3 = 0;
        wave.g3 = 0;
        wave.b3 = 0;
        pos.vx = 0;
        pos.vy = -350;
        pos.vz = side * (450 - work->config->depth);
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = side * work->config->second_tilt;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&work->wave2[0], &work->wave2[1], &work->wave2[2], &work->wave2[3], (PSXLONG *)&wave.x0,
                          (PSXLONG *)&wave.x1, (PSXLONG *)&wave.x2, (PSXLONG *)&wave.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&wave, ot, (u16)otz, 1);
        }
        work->colors[1][0][1] -= step * 8;
        if (work->colors[1][0][1] < 31) {
            work->colors[1][0][1] = 0;
            work->done[1][1][0] = 1;
        }
        work->colors[1][1][1] -= step * 8;
        if (work->colors[1][1][1] < 31) {
            work->colors[1][1][1] = 0;
            work->done[1][1][1] = 1;
        }
        work->colors[1][2][1] -= step * 8;
        if (work->colors[1][2][1] < 31) {
            work->colors[1][2][1] = 0;
            work->done[1][1][2] = 1;
        }
    }
    if (work->flash_frame[1] < 3) {
        flash.r0 = 0x80;
        flash.g0 = 0x80;
        flash.b0 = 0x80;
        pos.vx = 0;
        pos.vy = -350;
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        pos.vz = side * 450;
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(&work->flash[0], &work->flash[1], &work->flash[2], &work->flash[3], (PSXLONG *)&flash.x0,
                          (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2, (PSXLONG *)&flash.x3, &p, &flag);
        setUV4(&flash, work->flash_frame[1] % 3 * 64, 32, work->flash_frame[1] % 3 * 64 + 63, 32,
               work->flash_frame[1] % 3 * 64, 95, work->flash_frame[1] % 3 * 64 + 63, 95);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)otz, 4);
        }
        work->flash_frame[1]++;
    }
    if (work->elapsed < work->config->hold) {
        work->elapsed += step;
        PopMatrix();
        return 1;
    }
    if (work->elapsed < work->config->end) {
        work->elapsed += step;
        work->phase = 1;
    } else {
        PopMatrix();
        return 2;
    }
    PopMatrix();
    return 0;
}
