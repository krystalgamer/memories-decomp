#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/memory.h"
#include "../game/model_disc_effect.h"
#include "../game/func_80057E20.h"
#include "../game/func_80058E1C.h"
#include "../game/model_graphics_state.h"
#include "../game/screen_projection.h"
#include "../game/model_copy_slot_u16_values.h"
#include "../game/model_geometry_tables.h"
#include "../game/gpu_packets.h"

s32 Model_UpdateDiscEffect(ModelDiscEffectState *arg0, s32 arg1)
{
    ModelDiscEffectState *e;
    MATRIX ls;
    MATRIX m;
    POLY_G3 g3;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    ModelEffectAdjustment adj;
    /* Never read, but it occupies 0xC8..0xDF of the retail frame. */
    u8 unused_c8[0x18];
    DVECTOR sxy[68];
    u16 sz[68];
    u16 p[68];
    u16 flags[68];
    s32 flag;
    GsOT *ot;
    u8 step;
    ModelDiscEffectConfig *t;
    s32 i;
    s32 r;
    s32 v;
    s32 s;
    s32 otz;
    SVECTOR *pt;
    s32 c0;
    s32 c1;
    s32 c2;
    u16 u;
    s32 w;
    s32 k3;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8001187C;
    e = arg0;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();

    if (arg1 >= 0) {
        e->table = &D_80091604;
        t = e->table;
        func_80057E20(Model_GetActiveSlotIndex(), &adj);
        if (adj.y < adj.z) {
            v = adj.z;
        } else {
            v = adj.y;
        }
        if (v == adj.z) {
            u = adj.z;
            if (v < adj.x) {
                u = adj.x;
            }
        } else {
            u = adj.y;
            if (adj.y < adj.x) {
                u = adj.x;
            }
        }
        w = u << 16;
        v = w >> 16;
        v = v * 5 / 4;
        r = t->min_radius;
        if (v >= r) {
            r = 0x2BC;
            if (v < 0x2BD) {
                r = v;
            }
        }
        v = r;
        pt = e->points;
        k3 = v / 3;
        pt->vx = k3;
        pt->vy = k3;
        pt->vz = 0;
        pt++;
        for (i = 0; i < 33; i++) {
            pt->vx = v * ccos(i << 7) / 4096;
            pt->vy = v * csin(i << 7) / 4096;
            pt->vz = v * csin(i << 7) / 32768;
            pt++;
        }
        v = v * 4 / 5;
        for (i = 0; i < 33; i++) {
            pt->vx = v * ccos(i << 7) / 4096;
            pt->vy = v * csin(i << 7) / 4096;
            pt->vz = v * csin(i << 7) / 32768;
            pt++;
        }
        e->b = (arg1 % 10 + 6) * 0x11;
        e->r = ((arg1 - arg1 % 100) / 100 + 6) * 0x11;
        e->g = ((arg1 % 100 - arg1 % 10) / 10 + 6) * 0x11;
        e->level = t->full_level;
        e->scale = 0;
        e->frame = 0;
        return 0;
    }

    t = e->table;
    ot = func_80058F10();
    SetPolyG3(&g3);
    SetPolyG4(&g4);
    PushMatrix();
    ls = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&ls);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    v = (e->scale << 12) / t->full_scale + csin((e->frame & 3) << 10) / 16;
    scale.vx = v;
    scale.vy = v;
    scale.vz = v;
    Model_CopySlotU16Values(Model_GetActiveSlotIndex(), (u16 *)&pos);
    GsSetLsMatrix(&ls);
    RotTrans(&pos, (VECTOR *)m.t, (long *)&flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    c0 = e->r * e->level / t->full_level * e->scale / t->full_scale;
    c1 = e->g * e->level / t->full_level * e->scale / t->full_scale;
    c2 = e->b * e->level / t->full_level * e->scale / t->full_scale;
    g3.r0 = c0;
    g3.g0 = c1;
    g3.b0 = c2;
    c0 = t->r * e->level / t->full_level * e->scale / t->full_scale;
    c1 = t->g * e->level / t->full_level * e->scale / t->full_scale;
    c2 = t->b * e->level / t->full_level * e->scale / t->full_scale;
    g3.r1 = c0;
    g3.r2 = c0;
    g3.g1 = c1;
    g3.g2 = c1;
    g3.b1 = c2;
    g3.b2 = c2;
    RotTransPersN(e->points, sxy, sz, p, flags, 0x43);
    for (i = 0; i < 32; i++) {
        g3.x0 = sxy[0].vx;
        g3.y0 = sxy[0].vy;
        g3.x1 = sxy[i + 1].vx;
        g3.y1 = sxy[i + 1].vy;
        g3.x2 = sxy[i + 2].vx;
        g3.y2 = sxy[i + 2].vy;
        otz = AverageZ3(sz[0], sz[i + 1], sz[i + 2]) / 2;
        flag = (flags[0] | flags[1] | flags[2]) & 0x20;
        if (otz >= 0 && flag == 0) {
            func_8005B260((u32 *)&g3, ot, otz & 0xFFFF, 1);
        }
    }
    PopMatrix();
    e->frame++;
    if (arg1 == -2) {
        e->level -= step;
    } else {
        s = e->scale + step;
        e->scale = s;
        s = s < 0 ? 0 : (s > t->full_scale ? t->full_scale : s);
        e->scale = s;
    }
    return (e->level < 1) * 2;
}
