#include "../../types.h"
#include "variant764_entry.h"

/* Header 764's entry: after the delay a burst of textured shards flies out
 * in front of the slot for eight frames while the ambient light dims and
 * recovers, and a stack of growing rings of quads and a cone of triangles
 * spreads out and fades. The first drawn frame returns 1 and the end
 * returns 2. */

static const VECTOR gModelVariant764_D12EC = {4096, 4096, 4096, 0};

s32 ModelVariant764_HandleRequest(u8 *context, s32 command)
{
    Model764State *work;
    MATRIX light;
    MATRIX m;
    POLY_G4 g4;
    POLY_G3 g3;
    POLY_FT4 ft4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i;
    s32 angle;
    s32 a;
    s32 b;
    s32 c;
    s32 otz;
    SVECTOR *v;
    SVECTOR *w;
    SVECTOR *u;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant764_D12EC;
    work = (Model764State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant764_D1318[command];
        v = work->inner;
        w = work->outer;
        u = work->cone;
        for (i = 0; i < 16; v++, w++, u++, i++) {
            /* The spelled-out remainder, in its own variable, keeps retail's
             * a0 temporary and in-place angle update. */
            angle = (i << 8) - (i << 8) / 4096 * 4096;
            v->vx = work->config->inner_radius * ccos(angle) / 4096;
            v->vy = 0;
            v->vz = work->config->inner_radius * csin(angle) / 4096;
            w->vx = work->config->outer_radius * ccos(angle) / 4096;
            w->vy = -work->config->outer_radius * csin(0x80) / 4096;
            w->vz = work->config->outer_radius * csin(angle) / 4096;
            angle += 0x80;
            u->vx = work->config->inner_radius * (ccos(angle) << 1) / 4096;
            u->vy = -work->config->cone_height;
            u->vz = work->config->inner_radius * (csin(angle) << 1) / 4096;
        }
        v = work->shards;
        w = work->velocities;
        for (i = 0; i < work->config->shard_count; v++, w++, i++) {
            a = rand() % 4096 - rand() % 4096;
            b = -(rand() % 4096);
            c = rand() % 4096 - rand() % 4096;
            v->vx = work->config->shard_spread * a / 4096;
            v->vy = work->config->shard_spread_y * b / 4096;
            v->vz = work->config->shard_spread * c / 4096;
            w->vx = work->config->shard_speed * a / 4096;
            w->vy = work->config->shard_speed * b / 4096;
            w->vz = work->config->shard_speed * c / 4096;
        }
        v = &work->quad[0];
        setVector(v, -work->config->quad_half, -work->config->quad_half, 0);
        v = &work->quad[1];
        setVector(v, work->config->quad_half, -work->config->quad_half, 0);
        v = &work->quad[2];
        setVector(v, -work->config->quad_half, work->config->quad_half, 0);
        v = &work->quad[3];
        setVector(v, work->config->quad_half, work->config->quad_half, 0);
        work->ambient = 0x800;
        work->elapsed = 0;
        work->flash = 0;
        work->scale = 0x1000;
        work->phase = 0;
        work->color[0] = work->config->r;
        work->color[1] = work->config->g;
        work->color[2] = work->config->b;
        work->grey = 0x80;
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant764_D12FC[i]);
        }
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
    } else if (work->phase > 0) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->ambient = 0x800;
            work->phase = 2;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->flash < 8) {
        SetPolyFT4(&ft4);
        v = work->shards;
        w = work->quad;
        ft4.tpage = work->texture[0] >> 16;
        ft4.clut = work->texture[0];
        setRGB0(&ft4, 0x80, 0x80, 0x80);
        setVector(&rot, 0, 0, 0);
        u = work->velocities;
        for (i = 0; i < work->config->shard_count; i++) {
            /* "+=" onto the just-set position folds to retail's operand
             * order without reloading it. */
            setVector(&pos, 0, 0, direction * 450);
            pos.vx += v[i].vx;
            pos.vy += v[i].vy;
            pos.vz += v[i].vz;
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(w, &w[1], &w[2], &w[3], (PSXLONG *)&ft4.x0,
                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, work->flash * 32, 0, work->flash * 32 + 31, 0, work->flash * 32, 0x1F,
                   work->flash * 32 + 31, 0x1F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }
            v[i].vx += u[i].vx;
            v[i].vy += u[i].vy;
            v[i].vz += u[i].vz;
        }
        work->flash++;
    }

    if (work->scale < 25000) {
        work->scale += work->config->grow;
    } else {
        work->scale += work->config->grow_late;
    }
    SetPolyG4(&g4);
    SetPolyG3(&g3);
    setRGB0(&g4, work->color[0], work->color[1], work->color[2]);
    setRGB1(&g4, work->color[0], work->color[1], work->color[2]);
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
    for (i = 0; i < 16; i++) {
        if (i == 15) {
            otz = RotAverage4(&v[15], v, &w[15], w, (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        } else {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                              (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
        }
    }

    setRGB0(&g3, work->color[0], work->color[1], work->color[2]);
    setRGB1(&g3, work->color[0], work->color[1], work->color[2]);
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
            otz = RotAverage4(&v[15], v, &w[15], w, (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        } else {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                              (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
        }
        if (i == 15) {
            otz = RotAverage3(&v[15], v, &u[15], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p,
                              &flag);
        } else {
            otz = RotAverage3(&v[i], &v[i + 1], &u[i], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2,
                              &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
        }
    }

    setRGB0(&g3, work->grey, work->grey, work->grey);
    setRGB1(&g3, work->grey, work->grey, work->grey);
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
            otz = RotAverage3(&v[15], v, &u[15], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p,
                              &flag);
        } else {
            otz = RotAverage3(&v[i], &v[i + 1], &u[i], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2,
                              &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
        }
    }

    setRGB0(&g4, work->grey, work->grey, work->grey);
    setRGB1(&g4, work->grey, work->grey, work->grey);
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
    for (i = 0; i < 16; i++) {
        if (i == 15) {
            otz = RotAverage4(&v[15], v, &w[15], w, (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        } else {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                              (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
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
    for (i = 0; i < 16; i++) {
        if (i == 15) {
            otz = RotAverage4(&v[15], v, &w[15], w, (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2,
                              (PSXLONG *)&g4.x3, &p, &flag);
        } else {
            otz = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                              (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
        }
    }

    if (work->color[0] >= 32) {
        work->color[0] -= step * 6;
    } else {
        work->color[0] = 0;
    }
    if (work->color[1] >= 32) {
        work->color[1] -= step * 6;
    } else {
        work->color[1] = 0;
    }
    if (work->color[2] >= 32) {
        work->color[2] -= step * 6;
    } else {
        work->color[2] = 0;
    }
    if (work->grey >= 32) {
        work->grey -= step * 6;
    } else {
        work->grey = 0;
    }

    if (work->phase == 0) {
        work->phase = 1;
        PopMatrix();
        return 1;
    }
    if (work->elapsed < work->config->end) {
        work->elapsed += step;
        PopMatrix();
        return 0;
    }
    PopMatrix();
    return 2;
}
