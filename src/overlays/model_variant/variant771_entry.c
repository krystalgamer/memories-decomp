#include "../../types.h"
#include "variant771_entry.h"

/* Header 771's entry: from the trail start four ribbons follow four parts of
 * the active slot and fade along their length; afterwards a textured fan
 * widens in front of the slot while two flares at the trail heads drift
 * forward and their colour fades, and a stack of growing rings of quads and a
 * cone of triangles spreads out and fades to black. The ambient light dims
 * and recovers, and the camera shakes at two set times and on the first
 * drawn frame, which returns 1; the trail end returns 4 and the end
 * returns 2. */

static const VECTOR gModelVariant771_D1FA0 = {4096, 4096, 4096, 0};

s32 ModelVariant771_HandleRequest(u8 *context, s32 command)
{
    Model771State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 fan;
    POLY_FT4 flare;
    POLY_G4 ribbon;
    POLY_G4 g4;
    POLY_G3 g3;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i;
    s32 angle;
    s32 otz;
    s32 sx;
    s32 sy;
    s32 sz;
    SVECTOR *v;
    SVECTOR *w;
    SVECTOR *u;
    SVECTOR *x;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant771_D1FA0;
    work = (Model771State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant771_D2004[command];
        v = work->trail0;
        w = work->trail1;
        u = work->trail2;
        x = work->trail3;
        for (i = 0; i < 64; v++, w++, u++, x++, i++) {
            setVector(v, 0, 0, 0);
            setVector(w, 0, 0, 0);
            setVector(u, 0, 0, 0);
            setVector(x, 0, 0, 0);
            work->tint.c.fade[i] = 0xC0;
        }
        setVector(&work->offset, 0, 0, 0);
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(slot, 1, &gModelVariant771_D1FB0[i]);
        }
        Model_GetSlotDimensionsWithMinimums(-slot + 1, (ModelEffectAdjustment *)&pos);
        v = work->fan_a;
        w = work->fan_b;
        if (pos.vx > pos.vz) {
            for (i = 0; i < 9; v++, w++, i++) {
                v->vx = ccos(i << 8) % 4096 / 32;
                v->vy = 0;
                v->vz = direction * pos.vx * (csin(i << 8) % 4096) / 4096;
                w->vx = pos.vx * ((s32)((u32)(ccos(i << 8) % 4096) << 1)) / 4096;
                w->vy = -work->config->fan_height;
                w->vz = direction * ((s32)((u32)(csin(i << 8) % 4096) << 8)) / 4096;
            }
        } else {
            for (i = 0; i < 9; v++, w++, i++) {
                v->vx = ccos(i << 8) % 4096 / 32;
                v->vy = 0;
                v->vz = direction * ((s32)((u32)(csin(i << 8) % 4096) << 7)) / 4096;
                w->vx = ccos(i << 8) % 4096 / 16;
                w->vy = -work->config->fan_height;
                w->vz = direction * ((s32)((u32)(csin(i << 8) % 4096) << 8)) / 4096;
            }
        }
        v = &work->quad[0];
        setVector(v, 0, -work->config->quad_height, 0);
        v = &work->quad[1];
        setVector(v, 0, -work->config->quad_height, direction * work->config->quad_depth);
        v = &work->quad[2];
        setVector(v, 0, 0, 0);
        v = &work->quad[3];
        setVector(v, 0, 0, direction * work->config->quad_depth);
        v = work->inner;
        w = work->outer;
        u = work->cone;
        for (i = 0; i < 16; v++, w++, u++, i++) {
            /* The spelled-out remainder keeps retail's a0 temporary. */
            angle = (i << 8) - (i << 8) / 4096 * 4096;
            v->vx = work->config->inner_radius * ccos(angle) / 4096;
            v->vy = 0;
            v->vz = work->config->inner_radius * csin(angle) / 4096;
            w->vx = work->config->outer_radius * ccos(angle) / 4096;
            w->vy = -work->config->outer_radius * csin(0x80) / 4096;
            w->vz = work->config->outer_radius * csin(angle) / 4096;
            angle += 0x80;
            u->vx = work->config->inner_radius * ((s32)((u32)ccos(angle) << 1)) / 4096;
            u->vy = -work->config->cone_height;
            u->vz = work->config->inner_radius * ((s32)((u32)csin(angle) << 1)) / 4096;
        }
        work->head = 0;
        work->ambient = 0x800;
        work->elapsed = 0;
        work->phase = 0;
        work->shakes = 0;
        work->tint.c.r = work->config->r;
        work->tint.c.g = work->config->g;
        work->tint.c.b = work->config->b;
        work->tint.c.grey[0] = 0x80;
        work->tint.c.grey[1] = 0x80;
        work->frame = 1;
        work->scale = 0;
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
    if ((work->elapsed >= work->config->shake[0] && work->shakes == 0)
        || (work->elapsed >= work->config->shake[1] && work->shakes == 1)) {
        Model_QueueCameraShake(16, 8);
        work->shakes++;
    }
    if (work->elapsed < work->config->trail_start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->trail_start && work->elapsed <= work->config->trail_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[0]), &m);
        v = work->trail0;
        setVector(&v[work->head], m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[1]), &m);
        w = work->trail1;
        setVector(&w[work->head], m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[2]), &m);
        u = work->trail2;
        setVector(&u[work->head], m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[3]), &m);
        x = work->trail3;
        setVector(&x[work->head], m.t[0], m.t[1], m.t[2]);
        work->head = (work->head + 1) % 64;
        if (work->head < 2) {
            PopMatrix();
            work->elapsed += step;
            return 0;
        }
    }

    SetPolyG4(&ribbon);
    v = work->trail0;
    w = work->trail1;
    u = work->trail2;
    x = work->trail3;
    setRGB2(&ribbon, 0, 0, 0);
    setRGB3(&ribbon, 0, 0, 0);
    GsSetLsMatrix(&light);
    setVector(&pos, 0, 0, 0);
    RotTrans(&pos, (VECTOR *)m.t, &flag);
    RotMatrix(&rot, &m);
    MulMatrix2(&light, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    for (i = work->head - 1; i > 0; i--) {
        setRGB0(&ribbon, work->tint.c.fade[i], work->tint.c.fade[i], work->tint.c.fade[i]);
        setRGB1(&ribbon, work->tint.c.fade[i], work->tint.c.fade[i], work->tint.c.fade[i]);
        otz = RotAverage4(&v[i], &v[i - 1], &w[i], &w[i - 1],
                          (PSXLONG *)&ribbon.x0, (PSXLONG *)&ribbon.x1, (PSXLONG *)&ribbon.x2,
                          (PSXLONG *)&ribbon.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ribbon, ot, (u16)otz, 1);
        }
        otz = RotAverage4(&u[i], &u[i - 1], &x[i], &x[i - 1],
                          (PSXLONG *)&ribbon.x0, (PSXLONG *)&ribbon.x1, (PSXLONG *)&ribbon.x2,
                          (PSXLONG *)&ribbon.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&ribbon, ot, (u16)otz, 1);
        }
        if (work->tint.c.fade[i] >= 16) {
            work->tint.c.fade[i] -= step * 6;
        } else {
            work->tint.c.fade[i] = 0;
        }
    }
    if (work->elapsed < work->config->trail_end) {
        work->elapsed += step;
        PopMatrix();
        return 4;
    }

    if (work->frame < 64 && (work->tint.word & 0xFFFFFF)) {
        SetPolyFT4(&fan);
        setRGB0(&fan, work->tint.c.r / 3, work->tint.c.g / 3, work->tint.c.b / 3);
        fan.tpage = work->texture[1] >> 16;
        fan.clut = work->texture[1];
        setUV4(&fan, 0xFF, 0, 0xFF, 0x1F, 0x80, 0, 0x80, 0x1F);
        setVector(&pos, 0, 0, direction * 450);
        v = work->fan_a;
        w = work->fan_b;
        /* Each axis is clamped separately; chaining from sy gives retail's
         * register copies. */
        sz = sx = sy = work->frame * 1024 + 4096;
        if (sx > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sy > 0x7FCD) {
            sy = 0x7FCD;
        }
        if (sz > 0x7FCD) {
            sz = 0x7FCD;
        }
        setVector(&scale, sx, sy, sz);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 8; i++) {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                              (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, (PSXLONG *)&fan.x3,
                              &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)otz, 1);
            }
        }
        setVector(&pos, 0, 0, direction * 450);
        v = work->fan_a;
        w = work->fan_b;
        sx = work->frame * 512 + 4096;
        sy = work->frame * 3072 + 4096;
        sz = work->frame * 512 + 4096;
        if (sx > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sy > 0x7FCD) {
            sy = 0x7FCD;
        }
        if (sz > 0x7FCD) {
            sz = 0x7FCD;
        }
        setVector(&scale, sx, sy, sz);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 8; i++) {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                              (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, (PSXLONG *)&fan.x3,
                              &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)otz, 1);
            }
        }
        setVector(&pos, 0, 0, direction * 450);
        v = work->fan_a;
        w = work->fan_b;
        sx = work->frame * 1024 + 4096;
        sy = work->frame * 2304 + 4096;
        sz = work->frame * 1024 + 4096;
        if (sx > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sy > 0x7FCD) {
            sy = 0x7FCD;
        }
        if (sz > 0x7FCD) {
            sz = 0x7FCD;
        }
        setVector(&scale, sx, sy, sz);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 8; i++) {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                              (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, (PSXLONG *)&fan.x3,
                              &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)otz, 1);
            }
        }

        SetPolyFT4(&flare);
        setRGB0(&flare, work->tint.c.r, work->tint.c.g, work->tint.c.b);
        flare.tpage = work->texture[2] >> 16;
        flare.clut = work->texture[2];
        setUV4(&flare, 0, 0x80, 0x1F, 0x80, 0, 0xBF, 0x1F, 0xBF);
        v = work->quad;
        sz = sx = sy = work->frame * 512 + 4096;
        if (sx > 0x7FCD) {
            sx = 0x7FCD;
        }
        if (sy > 0x7FCD) {
            sy = 0x7FCD;
        }
        if (sz > 0x7FCD) {
            sz = 0x7FCD;
        }
        setVector(&scale, sx, sy, sz);
        /* The previous trail sample, addressed from the head so the -1 folds
         * into the displacement as in retail. */
        pos.vx = (&work->trail1[work->head] - 1)->vx;
        pos.vy = (&work->trail1[work->head] - 1)->vy;
        pos.vz = (&work->trail1[work->head] - 1)->vz;
        pos.vx += work->offset.vx;
        pos.vy += work->offset.vy;
        pos.vz += work->offset.vz;
        setVector(&rot, 0, 0, 0);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(v, &v[1], &v[2], &v[3], (PSXLONG *)&flare.x0,
                          (PSXLONG *)&flare.x1, (PSXLONG *)&flare.x2, (PSXLONG *)&flare.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&flare, ot, (u16)otz, 1);
        }
        pos.vx = (&work->trail3[work->head] - 1)->vx;
        pos.vy = (&work->trail3[work->head] - 1)->vy;
        pos.vz = (&work->trail3[work->head] - 1)->vz;
        pos.vx += work->offset.vx;
        pos.vy += work->offset.vy;
        pos.vz += work->offset.vz;
        setVector(&rot, 0, 0, 0);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        otz = RotAverage4(v, &v[1], &v[2], &v[3], (PSXLONG *)&flare.x0,
                          (PSXLONG *)&flare.x1, (PSXLONG *)&flare.x2, (PSXLONG *)&flare.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&flare, ot, (u16)otz, 1);
        }
    }
    work->frame++;
    if (work->tint.word & 0xFFFFFF) {
        work->offset.vz += direction * work->config->drift;
        work->tint.c.r -= 6;
        if (work->tint.c.r < 15) {
            work->tint.c.r = 0;
        }
        work->tint.c.g -= 6;
        if (work->tint.c.g < 15) {
            work->tint.c.g = 0;
        }
        work->tint.c.b -= 6;
        if (work->tint.c.b < 15) {
            work->tint.c.b = 0;
        }
    }

    if (work->tint.c.grey[0] != 0 || work->tint.c.grey[1] != 0) {
        if (work->scale < 25000) {
            work->scale += work->config->grow;
        } else {
            work->scale += work->config->grow_late;
        }
        SetPolyG4(&g4);
        SetPolyG3(&g3);
        setRGB0(&g4, work->tint.c.grey[0], work->tint.c.grey[0], work->tint.c.grey[0]);
        setRGB1(&g4, work->tint.c.grey[0], work->tint.c.grey[0], work->tint.c.grey[0]);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        setVector(&pos, 0, 0, direction * 450);
        setVector(&rot, 0, 0, 0);
        setVector(&scale, work->scale, work->scale, work->scale);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        u = work->cone;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }

        setRGB0(&g3, work->tint.c.grey[0], work->tint.c.grey[0], work->tint.c.grey[0]);
        setRGB1(&g3, work->tint.c.grey[0], work->tint.c.grey[0], work->tint.c.grey[0]);
        setRGB2(&g3, 0, 0, 0);
        setVector(&scale, work->scale / 2, work->scale / 2, work->scale / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        u = work->cone;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
            if (i == 15) {
                otz = RotAverage3(&v[15], v, &u[15], (PSXLONG *)&g3.x0,
                                  (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            } else {
                otz = RotAverage3(&v[i], &v[i + 1], &u[i], (PSXLONG *)&g3.x0,
                                  (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
            }
        }

        setRGB0(&g3, work->tint.c.grey[1], work->tint.c.grey[1], work->tint.c.grey[1]);
        setRGB1(&g3, work->tint.c.grey[1], work->tint.c.grey[1], work->tint.c.grey[1]);
        setRGB2(&g3, 0, 0, 0);
        setVector(&scale, work->scale / 2, work->scale / 4, work->scale / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage3(&v[15], v, &u[15], (PSXLONG *)&g3.x0,
                                  (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            } else {
                otz = RotAverage3(&v[i], &v[i + 1], &u[i], (PSXLONG *)&g3.x0,
                                  (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
            }
        }

        setRGB0(&g4, work->tint.c.grey[1], work->tint.c.grey[1], work->tint.c.grey[1]);
        setRGB1(&g4, work->tint.c.grey[1], work->tint.c.grey[1], work->tint.c.grey[1]);
        setRGB2(&g4, 0, 0, 0);
        setRGB3(&g4, 0, 0, 0);
        setVector(&scale, work->scale - 255, work->scale - 255, work->scale - 255);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        u = work->cone;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }
        setVector(&scale, (work->scale - 255) / 2, (work->scale - 255) / 2, (work->scale - 255) / 2);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        MulMatrix2(&light, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        v = work->inner;
        w = work->outer;
        u = work->cone;
        for (i = 0; i < 16; i++) {
            if (i == 15) {
                otz = RotAverage4(&v[15], v, &w[15], w,
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            } else {
                otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3,
                                  &p, &flag);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
            }
        }

        if (work->tint.c.grey[0] >= 16) {
            work->tint.c.grey[0] -= step * 6;
        } else {
            work->tint.c.grey[0] = 0;
        }
        if (work->tint.c.grey[1] >= 16) {
            work->tint.c.grey[1] -= step * 6;
        } else {
            work->tint.c.grey[1] = 0;
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase != 0) {
        return work->elapsed >= work->config->end ? 2 : 0;
    }
    Model_QueueCameraShake(48, 16);
    work->phase = 1;
    return 1;
}
