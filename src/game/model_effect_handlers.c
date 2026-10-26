#include "../types.h"
#include "graphics_packet_submission.h"
#include "model_effect_handlers.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/memory.h"
#include "../psyq/rand.h"
#include "../game/model_subdivided_effect.h"
#include "../game/model_get_slot_dimensions_with_minimums.h"
#include "../game/model_get_frame_step.h"
#include "../game/model_copy_slot_position.h"
#include "../game/gpu_packets.h"
#include "../game/model_graphics_state.h"
#include "../game/model_geometry_tables.h"
#include "../game/model_slot_properties.h"
#include "../game/model_state_setters.h"
#include "../game/screen_projection.h"
#include "../game/sound.h"

/* Three of the four handlers in the gModel_apfnEffectHandlers model effect table, which sit
   back to back in the image: Model_UpdateDiscEffect (the 67-point disc over a
   ModelDiscEffectState), Model_UpdateBurstEffect (rings, flash, dust, sparks, embers
   and smoke over a ModelBurstEffect) and Model_UpdateSparkEffect (sparks, flashes and
   dust over a ModelSparkEffect). The two particle bursts take their texture
   from the same regional burst CLUT; the disc draws untextured Gouraud
   polygons. */

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
    scale = gModel_DiscEffectInitialScale;
    e = arg0;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();

    if (arg1 >= 0) {
        e->table = &gModel_DiscEffectConfig;
        t = e->table;
        Model_GetSlotDimensionsWithMinimums(Model_GetActiveSlotIndex(), &adj);
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
    Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&pos);
    GsSetLsMatrix(&ls);
    RotTrans(&pos, (VECTOR *)m.t, (PSXLONG *)&flag);
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
            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, otz & 0xFFFF, 1);
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

/* Regional value: the burst CLUT (0x3D20 -> 0x3D28: x 0x200 -> 0x280, y
 * 0xF4). The European build (src/game/european/) defines its own. */
#ifndef MODEL_BURST_CLUT
#define MODEL_BURST_CLUT 0x3D20
#endif

/*
 * A burst handler in the gModel_apfnEffectHandlers model effect table. With a non-negative
 * mode it seeds three rings, 64 dust particles, 32 sparks, 32 embers and 64
 * smoke particles from ccos/csin and rand(); otherwise it draws the rings,
 * the flash, the dust, the sparks and the smoke as quads through RotAverage4,
 * steps its phase and stage and fades every group. It returns 2 once the
 * smoke, the first spark and the flash have all faded to black.
 *
 * The vectors and the packet are written through the libgpu helper macros
 * (setVector, addVector, copyVector, setUVWH, setRGB0), each with the address
 * of the indexed element as its argument. The four loop counters are shared by
 * the whole function: the flash grid runs j, i, l where the ring grid runs
 * j, k, l, and the dust seeding keeps its two random offsets in l and j.
 */

#define BURST_IMAGES ((ModelBurstImage *)gModel_BurstImageData)
/* The larger of the two horizontal extents Model_GetSlotDimensionsWithMinimums reports. */
#define BURST_EXTENT(v) ((s16)((v).vz < (v).vx ? (v).vx : (v).vz))
/* Grid vertex coordinates in quarter-radius steps; sign mirrors the columns. */
#define QUAD_X(n) (sign * -(e->radius >> 2) * (n))
#define QUAD_Z(n) (-(e->radius >> 2) * (n))
#define BURST_ABS(x) ((x) >= 0 ? (x) : -(x))
#define VECTOR_VIEW(value) ((VECTOR *)(value))
#define PACKET_WORD_VIEW(packet) ((u32 *)(packet))
/* The red and green bytes of a colour, read as the high half of the word that
 * starts two bytes before it. */
#define HI16(p, o) (*(u32 *)((u8 *)(p) + (o)) & 0xFFFF0000)

s32 Model_UpdateBurstEffect(void *data, s32 arg1)
{
    MATRIX ls;
    MATRIX m;
    POLY_FT4 ft4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR q[4];
    s32 flag;
    s32 p;
    s32 slot;
    GsOT *ot;
    s16 sign;
    ModelBurstEffect *e;
    s32 j;
    s32 k;
    s32 l;
    s32 i;
    s32 otz;
    POLY_FT4 *f;

    f = &ft4;
    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModel_BurstEffectInitialScale;
    e = data;
    slot = Model_GetActiveSlotIndex();

    if (arg1 >= 0) {
        e->table = (ModelBurstPalette *)gModel_BurstRingPaletteWords;
        e->tpage = 0xAE;
        e->clut = MODEL_BURST_CLUT;
        for (i = 0; i < 1; i++) {
            e->spark_texture[i].tpage = getTPage(BURST_IMAGES[i].mode, 1, BURST_IMAGES[i].prect.x,
                                                 BURST_IMAGES[i].prect.y);
            e->spark_texture[i].clut = getClut(BURST_IMAGES[i].crect.x, BURST_IMAGES[i].crect.y);
        }
        Model_GetSlotDimensionsWithMinimums(slot, (ModelEffectAdjustment *)&pos);
        for (i = 0; i < 3; i++) {
            setVector(&e->rings[i], BURST_EXTENT(pos) * (ccos(i * 0x555) << 1) / 4096, 0,
                      BURST_EXTENT(pos) * (csin(i * 0x555) << 1) / 4096);
            setVector(&e->ring_speed[i], -e->rings[i].vx / 48, 0, -e->rings[i].vz / 48);
            e->colors[i].r = e->table->r[i] >> 3;
            e->colors[i].g = e->table->g[i] >> 3;
            e->colors[i].b = e->table->b[i] >> 3;
        }
        e->radius = BURST_EXTENT(pos) / 2 + 200;
        if (e->radius > 0x200) {
            e->radius = 0x200;
        }
        e->flash_r = 0x80;
        e->flash_g = 0x80;
        e->flash_b = 0x80;
        for (i = 0; i < 64; i++) {
            l = (rand() - rand()) % 4096;
            j = (rand() - rand()) % 4096;
            setVector(&e->dust[i], e->radius * l / 4096, 0, e->radius * j / 4096);
            setVector(&e->dust_speed[i], l * 24 / 4096, -(rand() % 4096 * 24) / 4096,
                      j * 24 / 4096);
            e->dust_frame[i] = rand() % 4;
        }
        for (i = 0; i < 32; i++) {
            setVector(&e->sparks[i], e->radius * ccos(i << 7) / 4096, 0,
                      e->radius * csin(i << 7) / 4096);
            setVector(&e->spark_drift[i], -e->sparks[i].vx / 128, -0x10, -e->sparks[i].vz / 128);
            setVector(&e->spark_rise[i], ccos(i << 7) / 128, 0, csin(i << 7) / 128);
            e->spark_frame[i] = rand() % 8;
            e->spark_colors[i].r = 1;
            e->spark_colors[i].g = 1;
            e->spark_colors[i].b = 1;
        }
        for (i = 0; i < 32; i++) {
            setVector(&e->embers[i], (rand() - rand()) % 4096 * 0xA0 / 4096,
                      (rand() - rand()) % 4096 * 0xA0 / 4096,
                      (rand() - rand()) % 4096 * 0xA0 / 4096);
        }
        for (i = 0; i < 64; i++) {
            setVector(&e->smoke[i], 0, 0, 0);
            setVector(&e->smoke_speed[i], (rand() - rand()) % 4096 * 0x60 / 4096,
                      (rand() - rand()) % 4096 * 0x18 / 4096,
                      (rand() - rand()) % 4096 * 0x60 / 4096);
            e->smoke_frame[i] = rand() % 4;
        }
        e->smoke_r = 0x80;
        e->smoke_g = 0x40;
        e->smoke_b = 0x79;
        Model_CopySlotPosition(slot, (u16 *)&rot);
        rot.vy = 0;
        copyVector(&e->origin, &rot);
        e->phase = 0;
        e->stage = 0;
        e->frame = 0;
        e->spark_count = 1;
        e->fade = 0;
        SD_SEPlay(0, 0xFF, 0);
        return 0;
    }

    ot = func_80058F10();
    Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    PushMatrix();
    ls = *(MATRIX *)Model_GetLightSourceMatrix();
    Model_CopySlotPosition(slot, (u16 *)&pos);
    pos.vy = 0;
    copyVector(&e->origin, &pos);
    if (e->phase == 0) {
        rot.vx = 0;
        rot.vy = (e->frame * 0xA0) & 0xFFF;
        rot.vz = 0;
    }
    GsSetLsMatrix(&ls);
    RotTrans(&e->origin, VECTOR_VIEW(m.t), (PSXLONG *)&flag);
    RotMatrix(&rot, &m);
    MulMatrix2(&ls, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    setPolyFT4(f);
    f->tpage = e->tpage;
    f->clut = e->clut;
    setUVWH(f, 0, 0, 0x3F, 0x3F);

    if (e->phase == 0) {
        for (i = 0; i < 3; i++) {
            if (e->colors[i].r != 0 || e->colors[i].g != 0 || e->colors[i].b != 0) {
                sign = 1;
                setRGB0(f, e->colors[i].r, e->colors[i].g, e->colors[i].b);
                for (j = 0; j < 4; j++) {
                    sign = sign * -1;
                    for (k = 0; k < 4; k++) {
                        s32 lo;
                        s32 hi;
                        for (l = 0, lo = -1, hi = 1; l < 4; lo--, l++, hi++) {
                            setVector(&q[0], QUAD_X(k + 1), 0, QUAD_Z(j - 2 < 0 ? lo : hi));
                            setVector(&q[1], QUAD_X(k), 0, QUAD_Z(j - 2 < 0 ? lo : hi));
                            setVector(&q[2], QUAD_X(k + 1), 0, QUAD_Z(j - 2 < 0 ? -l : l));
                            setVector(&q[3], QUAD_X(k), 0, QUAD_Z(j - 2 < 0 ? -l : l));
                            addVector(&q[0], &e->rings[i]);
                            addVector(&q[1], &e->rings[i]);
                            addVector(&q[2], &e->rings[i]);
                            addVector(&q[3], &e->rings[i]);
                            setUVWH(f, (3 - k) * 16, (3 - l) * 16, 0xF, 0xF);
                            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0,
                                              (PSXLONG *)&f->x1, (PSXLONG *)&f->x2,
                                              (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
                            if (otz >= 0 && flag >= 0) {
                                Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
                            }
                        }
                    }
                }
                if (BURST_ABS(e->rings[i].vx) >= 0x21 || BURST_ABS(e->rings[i].vy) >= 0x21 ||
                    BURST_ABS(e->rings[i].vz) >= 0x21) {
                    addVector(&e->rings[i], &e->ring_speed[i]);
                } else {
                    if (e->phase == 0) {
                        e->phase = 1;
                    }
                    setVector(&e->rings[i], 0, 0, 0);
                }
                if (e->phase == 0) {
                    if (e->colors[i].r < (e->table->r[i] >> 1) - 9) {
                        e->colors[i].r = e->colors[i].r + 8;
                    } else {
                        e->colors[i].r = e->table->r[i] >> 1;
                    }
                    if (e->colors[i].g < (e->table->g[i] >> 1) - 9) {
                        e->colors[i].g = e->colors[i].g + 8;
                    } else {
                        e->colors[i].g = e->table->g[i] >> 1;
                    }
                    if (e->colors[i].b < (e->table->b[i] >> 1) - 9) {
                        e->colors[i].b = e->colors[i].b + 8;
                    } else {
                        e->colors[i].b = e->table->b[i] >> 1;
                    }
                } else {
                    e->colors[0].r = 0x80;
                    e->colors[0].g = 0x20;
                    e->colors[0].b = 0x20;
                    e->colors[1].r = 0x20;
                    e->colors[1].g = 0x80;
                    e->colors[1].b = 0x20;
                    e->colors[2].r = 0x20;
                    e->colors[2].g = 0x20;
                    e->colors[2].b = 0x80;
                }
            }
        }
        if (e->phase == 0) {
            goto stage_test;
        }
    }

    if (HI16(e, 0xD9C) != 0 || e->flash_b != 0) {
        setRGB0(f, e->flash_r, e->flash_g, e->flash_b);
        sign = 1;
        for (j = 0; j < 4; j++) {
            sign = sign * -1;
            for (i = 0; i < 4; i++) {
                s32 lo;
                s32 hi;
                for (l = 0, lo = -1, hi = 1; l < 4; lo--, l++, hi++) {
                    setVector(&q[0], QUAD_X(i + 1), 0, QUAD_Z(j - 2 < 0 ? lo : hi));
                    setVector(&q[1], QUAD_X(i), 0, QUAD_Z(j - 2 < 0 ? lo : hi));
                    setVector(&q[2], QUAD_X(i + 1), 0, QUAD_Z(j - 2 < 0 ? -l : l));
                    setVector(&q[3], QUAD_X(i), 0, QUAD_Z(j - 2 < 0 ? -l : l));
                    setUVWH(f, (3 - i) * 16, (3 - l) * 16, 0xF, 0xF);
                    otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0,
                                      (PSXLONG *)&f->x1, (PSXLONG *)&f->x2,
                                      (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
                    }
                }
            }
        }
        if (e->stage >= 2) {
            if (e->flash_r >= 9) {
                e->flash_r = e->flash_r - 8;
            } else {
                e->flash_r = 0;
            }
            if (e->flash_g >= 9) {
                e->flash_g = e->flash_g - 8;
            } else {
                e->flash_g = 0;
            }
            if (e->flash_b >= 9) {
                e->flash_b = e->flash_b - 8;
            } else {
                e->flash_b = 0;
            }
            e->radius += 2;
        } else if (e->phase == 1) {
            if (e->flash_r < 0xF6) {
                e->flash_r = e->flash_r + 8;
            } else {
                e->flash_r = 0xFF;
            }
            if (e->flash_g < 0xF6) {
                e->flash_g = e->flash_g + 8;
            } else {
                e->flash_g = 0xFF;
            }
            if (e->flash_b < 0xF6) {
                e->flash_b = e->flash_b + 8;
            } else {
                e->flash_b = 0xFF;
            }
            e->radius += 4;
            if (HI16(e, 0xD9C) == 0xFFFF0000 && e->flash_b == 0xFF) {
                e->phase = 2;
            }
        } else if (e->phase == 2) {
            if (e->flash_r < 0x89) {
                e->flash_r = 0x80;
            } else {
                e->flash_r = e->flash_r - 8;
            }
            if (e->flash_g < 0x89) {
                e->flash_g = 0x80;
            } else {
                e->flash_g = e->flash_g - 8;
            }
            if (e->flash_b < 0x89) {
                e->flash_b = 0x80;
            } else {
                e->flash_b = e->flash_b - 8;
            }
            e->radius -= 2;
            if (HI16(e, 0xD9C) == 0x80800000 && e->flash_b == 0x80) {
                e->phase = 1;
            }
        }
    }

    if ((HI16(e, 0xD90) != 0 || e->colors[0].b != 0) &&
        (HI16(e, 0xD94) != 0 || e->colors[1].b != 0) &&
        (HI16(e, 0xD98) != 0 || e->colors[2].b != 0)) {
        GsSetLsMatrix(&ls);
        RotTrans(&e->origin, VECTOR_VIEW(m.t), (PSXLONG *)&flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (i = 0; i < 64; i++) {
            setRGB0(f, e->colors[i % 3].r, e->colors[i % 3].g, e->colors[i % 3].b);
            setUVWH(f, (e->dust_frame[i] % 2) << 5, (e->dust_frame[i] / 2 << 5) + 0x40, 0x1F, 0x1F);
            setVector(&q[0], -0x20, -0x20, 0);
            setVector(&q[1], 0x20, -0x20, 0);
            setVector(&q[2], -0x20, 0x20, 0);
            setVector(&q[3], 0x20, 0x20, 0);
            for (l = 0; l < 4; l++) {
                addVector(&q[l], &e->dust[i]);
            }
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                              (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
            if (otz >= 0) {
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
                }
            }
            addVector(&e->dust[i], &e->dust_speed[i]);
            e->dust_frame[i] = (e->dust_frame[i] + 1) % 4;
        }
        for (i = 0; i < 3; i++) {
            if (e->colors[i].r >= 0x10) {
                e->colors[i].r = e->colors[i].r - 0xF;
            } else {
                e->colors[i].r = 0;
            }
            if (e->colors[i].g >= 0x10) {
                e->colors[i].g = e->colors[i].g - 0xF;
            } else {
                e->colors[i].g = 0;
            }
            if (e->colors[i].b >= 0x10) {
                e->colors[i].b = e->colors[i].b - 0xF;
            } else {
                e->colors[i].b = 0;
            }
        }
    }

    f->tpage = e->spark_texture[0].tpage;
    f->clut = e->spark_texture[0].clut;
    setVector(&q[0], -0x40, -0x100, 0);
    setVector(&q[1], 0x40, -0x100, 0);
    setVector(&q[2], -0x40, 0, 0);
    setVector(&q[3], 0x40, 0, 0);
    for (i = 0; i < e->spark_count; i++) {
        if (e->spark_colors[i].r != 0 || e->spark_colors[i].g != 0 ||
            e->spark_colors[i].b != 0) {
            copyVector(&pos, &e->origin);
            addVector(&pos, &e->sparks[i]);
            GsSetLsMatrix(&ls);
            RotTrans(&pos, VECTOR_VIEW(m.t), (PSXLONG *)&flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setUVWH(f, e->spark_frame[i] * 32, 0, 0x1F, 0x3F);
            setRGB0(f, e->spark_colors[i].r, e->spark_colors[i].g, e->spark_colors[i].b);
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                              (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
            }
            e->spark_frame[i] = (e->spark_frame[i] + 1) % 8;
            if (e->stage < 2) {
                if (e->spark_colors[i].r < 0x61) {
                    e->spark_colors[i].r = e->spark_colors[i].r + 0x1F;
                } else {
                    e->spark_colors[i].r = 0x80;
                }
                if (e->spark_colors[i].g < 0x61) {
                    e->spark_colors[i].g = e->spark_colors[i].g + 0x1F;
                } else {
                    e->spark_colors[i].g = 0x80;
                }
                if (e->spark_colors[i].b < 0x61) {
                    e->spark_colors[i].b = e->spark_colors[i].b + 0x1F;
                } else {
                    e->spark_colors[i].b = 0x80;
                }
            }
            if (e->stage == 1) {
                addVector(&e->sparks[i], &e->spark_drift[i]);
            }
            if (e->stage == 2) {
                addVector(&e->sparks[i], &e->spark_rise[i]);
                e->spark_rise[i].vy += 2;
                if (e->spark_colors[i].r >= 9) {
                    e->spark_colors[i].r = e->spark_colors[i].r - 8;
                } else {
                    e->spark_colors[i].r = 0;
                }
                if (e->spark_colors[i].g >= 9) {
                    e->spark_colors[i].g = e->spark_colors[i].g - 8;
                } else {
                    e->spark_colors[i].g = 0;
                }
                if (e->spark_colors[i].b >= 9) {
                    e->spark_colors[i].b = e->spark_colors[i].b - 8;
                } else {
                    e->spark_colors[i].b = 0;
                }
            }
        }
    }

    if ((e->spark_colors[e->spark_count - 1].r >= 0x41 ||
         e->spark_colors[e->spark_count - 1].g >= 0x41 ||
         e->spark_colors[e->spark_count - 1].b >= 0x41) &&
        e->stage == 0) {
        if ((u16)(e->spark_count % 3) == 0) {
            SD_SEPlay(1, 0xFF, 0);
        }
        e->spark_count++;
        if (e->spark_count > 32) {
            e->spark_count = 32;
            e->stage = 1;
        }
    }
stage_test:
    if (e->stage >= 2 && (HI16(e, 0xE20) != 0 || e->smoke_b != 0)) {
        f->tpage = e->tpage;
        f->clut = e->clut;
        setVector(&pos, 0, -0x220, 0);
        addVector(&pos, &e->origin);
        GsSetLsMatrix(&ls);
        RotTrans(&pos, VECTOR_VIEW(m.t), (PSXLONG *)&flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        sign = 1;
        for (i = 0; i < 4; i++) {
            sign = sign * -1;
            setVector(&q[0], -((sign << 16) >> 9), i - 2 >= 0 ? -0x80 : 0x80, 0);
            setVector(&q[1], 0, i - 2 >= 0 ? -0x80 : 0x80, 0);
            setVector(&q[2], -((sign << 16) >> 9), 0, 0);
            setVector(&q[3], 0, 0, 0);
            setRGB0(f, e->smoke_r, e->smoke_g, e->smoke_b);
            setUVWH(f, 0x40, 0, 0x3F, 0x3F);
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                              (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
            if (otz >= 0) {
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
                }
            }
            setRGB0(f, e->smoke_r >> 1, e->smoke_g >> 1, e->smoke_b >> 1);
            setUVWH(f, 0x40, 0x40, 0x3F, 0x3F);
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                              (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
            }
        }
        setUVWH(f, 0, 0x80, 0x1F, 0x7F);
        setVector(&q[0], -4, 0, 0);
        setVector(&q[1], 4, 0, 0);
        setRGB0(f, e->smoke_r >> 2, e->smoke_g >> 2, e->smoke_b >> 2);
        for (i = 0; i < 32; i++) {
            setVector(&q[2], -2, 0, 0);
            setVector(&q[3], 2, 0, 0);
            addVector(&q[2], &e->embers[i]);
            addVector(&q[3], &e->embers[i]);
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                              (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
            }
        }
        for (i = 0; i < 64; i++) {
            setRGB0(f, e->smoke_r, e->smoke_g, e->smoke_b);
            setUVWH(f, (e->smoke_frame[i] % 2) << 5, (e->smoke_frame[i] / 2 << 5) + 0x40, 0x1F,
                    0x1F);
            setVector(&q[0], -0x20, -0x20, 0);
            setVector(&q[1], 0x20, -0x20, 0);
            setVector(&q[2], -0x20, 0x20, 0);
            setVector(&q[3], 0x20, 0x20, 0);
            for (l = 0; l < 4; l++) {
                addVector(&q[l], &e->smoke[i]);
            }
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                              (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
            if (otz >= 0) {
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket(PACKET_WORD_VIEW(f), ot, otz & 0xFFFF, 1);
                }
            }
            addVector(&e->smoke[i], &e->smoke_speed[i]);
            e->smoke_frame[i] = (e->smoke_frame[i] + 1) % 4;
            e->smoke_speed[i].vy += 2;
        }
        if (e->smoke_r >= 0x10) {
            e->smoke_r = e->smoke_r - 0xF;
        } else {
            e->smoke_r = 0;
        }
        if (e->smoke_g >= 0x10) {
            e->smoke_g = e->smoke_g - 0xF;
        } else {
            e->smoke_g = 0;
        }
        if (e->smoke_b >= 0x10) {
            e->smoke_b = e->smoke_b - 0xF;
        } else {
            e->smoke_b = 0;
        }
    }

    if ((s8)e->fade >= 0) {
        Model_SetSlotTintTarget(slot, 5, e->fade, e->fade, e->fade);
    } else {
        Model_SetSlotTintTarget(slot, 0, 0x80, 0x80, 0x80);
    }
    if (e->stage != 0) {
        if (e->fade < 0x7C) {
            e->fade = e->fade + 4;
        } else {
            e->fade = 0x80;
            e->stage = 2;
        }
    }
    e->frame++;
    PopMatrix();
    if (HI16(e, 0xE20) == 0 && e->smoke_b == 0 && HI16(e, 0xDA0) == 0 &&
        e->spark_colors[0].b == 0 && HI16(e, 0xD9C) == 0 && e->flash_b == 0) {
        return 2;
    }
    return 0;
}

/*
 * Fourth handler in the gModel_apfnEffectHandlers model effect table: a spark burst. With
 * a non-negative mode it seeds 32 sparks, 32 flashes and 64 dust particles
 * from rand(); otherwise it draws the sparks (a fan of four quads and a
 * growing Gouraud quad each), the flashes and the dust as quads through
 * RotAverage4, fades every colour, respawns spent sparks and flashes, and
 * once every spark is lit hands over to the dust. It returns 2 once the
 * first flash and the dust have faded to black.
 *
 * The vectors and the packets are written through the libgpu helper macros
 * (setVector, addVector, setRGB0..3, setUVWH, setPolyFT4, setPolyG4) with
 * the address of the indexed element as the argument, the four quad vertices
 * are one SVECTOR array, and the counters i, j and k are shared by every
 * loop: the dust quad reuses j as its vertex counter, which is what gives j
 * (and so the pos pointer) the s2 slot.
 */

#define RAND_SPREAD(n) ((rand() - rand()) % 4096 * (n) / 4096)
#define RAND_DROP(n) (-(rand() % 4096 * (n)) / 4096)
#define SPARK_U16_AT(p, o) (*(u16 *)((u8 *)(p) + (o)))

s32 Model_UpdateSparkEffect(void *data, s32 arg1)
{
    MATRIX ls;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR q[4];
    s32 flag;
    s32 p;
    s32 slot;
    GsOT *ot;
    s16 sign;
    ModelSparkEffect *e;
    s32 i;
    s32 j;
    s32 k;
    s32 otz;
    POLY_FT4 *f;
    POLY_G4 *g;

    f = &ft4;
    g = &g4;
    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModel_SparkEffectInitialScale;
    e = data;
    slot = Model_GetActiveSlotIndex();

    if (arg1 >= 0) {
        e->tpage = 0xAE;
        e->clut = MODEL_BURST_CLUT;
        for (i = 0; i < 32; i++) {
            setVector(&e->sparks[i], RAND_SPREAD(320), 0, RAND_SPREAD(320));
            e->spark_colors[i].r = 0x60;
            e->spark_colors[i].g = 0x60;
            e->spark_colors[i].b = 0x7F;
            e->sizes[i].size = 8;
            e->sizes[i].grow = 0x100;
            setVector(&e->flashes[i], RAND_SPREAD(320), RAND_DROP(128), RAND_SPREAD(320));
            e->flash_colors[i].r = 0x40;
            e->flash_colors[i].g = 0x40;
            e->flash_colors[i].b = 0x55;
        }
        for (i = 0; i < 64; i++) {
            setVector(&e->dust[i], RAND_SPREAD(160), 0, RAND_SPREAD(160));
            setVector(&e->dust_speed[i], RAND_SPREAD(24), RAND_DROP(24), RAND_SPREAD(24));
            e->dust_frame[i] = rand() % 4;
        }
        e->dust_r = 0;
        e->dust_g = 0;
        e->dust_b = 0;
        e->spark_count = 0;
        e->flash_count = 0;
        e->mode = 0;
        e->frame = 0;
        e->fade = 0x80;
        return 0;
    }

    ot = func_80058F10();
    Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    PushMatrix();
    ls = *(MATRIX *)Model_GetLightSourceMatrix();
    Model_CopySlotPosition(slot, (u16 *)&pos);
    pos.vy = 0;
    GsSetLsMatrix(&ls);
    RotTrans(&pos, (VECTOR *)m.t, (PSXLONG *)&flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    setPolyFT4(f);
    f->tpage = e->tpage;
    f->clut = e->clut;
    setUVWH(f, 0x40, 0, 0x3F, 0x3F);
    setPolyG4(g);

    for (i = 0; i < e->spark_count; i++) {
        if (e->spark_colors[i].r != 0 || e->spark_colors[i].g != 0 ||
            e->spark_colors[i].b != 0) {
            setRGB0(f, e->spark_colors[i].r, e->spark_colors[i].g, e->spark_colors[i].b);
            setRGB0(g, 0, 0, 0);
            setRGB1(g, 0, 0, 0);
            if (e->spark_count < 32) {
                setRGB2(g, e->spark_colors[i].r >> 1, e->spark_colors[i].g >> 1,
                        e->spark_colors[i].b >> 1);
                setRGB3(g, e->spark_colors[i].r >> 1, e->spark_colors[i].g >> 1,
                        e->spark_colors[i].b >> 1);
            } else {
                setRGB2(g, e->spark_colors[i].r, e->spark_colors[i].g, e->spark_colors[i].b);
                setRGB3(g, e->spark_colors[i].r, e->spark_colors[i].g, e->spark_colors[i].b);
            }
            sign = 1;
            for (j = 0; j < 4; j++) {
                sign = sign * -1;
                setVector(&q[0], -((sign << 16) >> 9), j - 2 >= 0 ? -0x80 : 0x80, 0);
                setVector(&q[1], 0, j - 2 >= 0 ? -0x80 : 0x80, 0);
                setVector(&q[2], -((sign << 16) >> 9), 0, 0);
                setVector(&q[3], 0, 0, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&q[k], &e->sparks[i]);
                }
                otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                                  (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)f, ot, otz & 0xFFFF, 1);
                }
            }
            setVector(&q[0], -e->sizes[i].size, -e->sizes[i].grow, 0);
            setVector(&q[1], e->sizes[i].size, -e->sizes[i].grow, 0);
            setVector(&q[2], -e->sizes[i].size, 0, 0);
            setVector(&q[3], e->sizes[i].size, 0, 0);
            for (k = 0; k < 4; k++) {
                addVector(&q[k], &e->sparks[i]);
            }
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&g->x0, (PSXLONG *)&g->x1,
                              (PSXLONG *)&g->x2, (PSXLONG *)&g->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)g, ot, otz & 0xFFFF, 1);
            }
            if (e->mode == 0) {
                e->sizes[i].size += 2;
                e->sizes[i].grow += 0x80;
            }
            if ((s16)e->sizes[i].size > 0x20) {
                e->sizes[i].size = 0x20;
            }
            if ((s16)e->sizes[i].grow > 0x600) {
                e->sizes[i].grow = 0x600;
            }
            if (e->spark_count < 32 || e->mode == 1) {
                if (e->spark_colors[i].r >= 0x10) {
                    e->spark_colors[i].r = e->spark_colors[i].r - 0xF;
                } else {
                    e->spark_colors[i].r = 0;
                }
                if (e->spark_colors[i].g >= 0x10) {
                    e->spark_colors[i].g = e->spark_colors[i].g - 0xF;
                } else {
                    e->spark_colors[i].g = 0;
                }
                if (e->spark_colors[i].b >= 0x10) {
                    e->spark_colors[i].b = e->spark_colors[i].b - 0xF;
                } else {
                    e->spark_colors[i].b = 0;
                }
            } else {
                if (e->spark_colors[i].r >= 7) {
                    e->spark_colors[i].r = e->spark_colors[i].r - 4;
                } else {
                    e->spark_colors[i].r = 0;
                }
                if (e->spark_colors[i].g >= 7) {
                    e->spark_colors[i].g = e->spark_colors[i].g - 4;
                } else {
                    e->spark_colors[i].g = 0;
                }
                if (e->spark_colors[i].b >= 7) {
                    e->spark_colors[i].b = e->spark_colors[i].b - 4;
                } else {
                    e->spark_colors[i].b = 0;
                }
            }
            if (e->spark_colors[i].r == 0 && e->spark_colors[i].g == 0 &&
                e->spark_colors[i].b == 0 && e->mode == 0) {
                setVector(&e->sparks[i], RAND_SPREAD(320), 0, RAND_SPREAD(320));
                e->spark_colors[i].r = 0xC0;
                e->spark_colors[i].g = 0xC0;
                e->spark_colors[i].b = 0xFF;
                e->sizes[i].size = 8;
                e->sizes[i].grow = 0x100;
            }
        }
    }

    setUVWH(f, 0x40, 0x40, 0x3F, 0x3F);
    for (i = 0; i < e->flash_count; i++) {
        if (e->flash_colors[i].r != 0 || e->flash_colors[i].g != 0 ||
            e->flash_colors[i].b != 0) {
            setRGB0(f, e->flash_colors[i].r, e->flash_colors[i].g, e->flash_colors[i].b);
            sign = 1;
            for (j = 0; j < 4; j++) {
                sign = sign * -1;
                setVector(&q[0], -sign * 0x50, j - 2 >= 0 ? -0x50 : 0x50, 0);
                setVector(&q[1], 0, j - 2 >= 0 ? -0x50 : 0x50, 0);
                setVector(&q[2], -sign * 0x50, 0, 0);
                setVector(&q[3], 0, 0, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&q[k], &e->flashes[i]);
                }
                otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                                  (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)f, ot, otz & 0xFFFF, 1);
                }
            }
            if (e->flash_colors[i].r >= 0x20) {
                e->flash_colors[i].r = e->flash_colors[i].r - 0x1F;
            } else {
                e->flash_colors[i].r = 0;
            }
            if (e->flash_colors[i].g >= 0x20) {
                e->flash_colors[i].g = e->flash_colors[i].g - 0x1F;
            } else {
                e->flash_colors[i].g = 0;
            }
            if (e->flash_colors[i].b >= 0x20) {
                e->flash_colors[i].b = e->flash_colors[i].b - 0x1F;
            } else {
                e->flash_colors[i].b = 0;
            }
            if (e->flash_colors[i].r == 0 && e->flash_colors[i].g == 0 &&
                e->flash_colors[i].b == 0) {
                setVector(&e->flashes[i], RAND_SPREAD(320), RAND_DROP(128), RAND_SPREAD(320));
                e->flash_colors[i].r = 0x40;
                e->flash_colors[i].g = 0x40;
                e->flash_colors[i].b = 0x55;
            }
        }
    }

    if (e->mode == 0 && !(e->frame & 1)) {
        SD_SEPlay(2, 0xFF, 0);
        e->spark_count++;
        if (e->spark_count > 32) {
            e->spark_count = 32;
        }
        if (!(e->frame & 1)) {
            e->flash_count++;
            if (e->flash_count > 32) {
                e->flash_count = 32;
            }
        }
    }
    Model_SetSlotTintTarget(slot, 5, e->fade, e->fade, e->fade);
    if (e->fade >= 4) {
        e->fade = e->fade - 2;
    } else {
        e->fade = 0;
    }
    if (e->spark_count == 32 && e->flash_count == e->spark_count && e->fade == 0) {
        e->dust_r = 0xC0;
        e->dust_g = 0xC0;
        e->dust_b = 0xFF;
        e->flash_colors[0].r = 0;
        e->flash_colors[0].g = 0;
        e->flash_colors[0].b = 0;
        e->flash_count = 0;
        e->mode = 1;
        Model_SetSlotTintTarget(slot, 5, 0, 0, 0);
    }
    if (SPARK_U16_AT(e, 0x80E) != 0 || e->dust_b != 0) {
        if (e->mode == 1) {
            setRGB0(f, e->dust_r, e->dust_g, e->dust_b);
            for (i = 0; i < 64; i++) {
                setUVWH(f, (e->dust_frame[i] % 2) << 5, (e->dust_frame[i] / 2 << 5) + 0x40, 0x1F,
                        0x1F);
                setVector(&q[0], -0x10, -0x10, 0);
                setVector(&q[1], 0x10, -0x10, 0);
                setVector(&q[2], -0x10, 0x10, 0);
                setVector(&q[3], 0x10, 0x10, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&q[j], &e->dust[i]);
                }
                otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (PSXLONG *)&f->x0, (PSXLONG *)&f->x1,
                                  (PSXLONG *)&f->x2, (PSXLONG *)&f->x3, (PSXLONG *)&p, (PSXLONG *)&flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)f, ot, otz & 0xFFFF, 1);
                }
                addVector(&e->dust[i], &e->dust_speed[i]);
                e->dust_frame[i] = (e->dust_frame[i] + 1) % 4;
            }
            if (e->dust_r >= 0xA) {
                e->dust_r = e->dust_r - 8;
            } else {
                e->dust_r = 0;
            }
            if (e->dust_g >= 0xA) {
                e->dust_g = e->dust_g - 8;
            } else {
                e->dust_g = 0;
            }
            if (e->dust_b >= 0xA) {
                e->dust_b = e->dust_b - 8;
            } else {
                e->dust_b = 0;
            }
        }
    }
    e->frame++;
    PopMatrix();
    if (SPARK_U16_AT(e, 0x78E) == 0 && e->flash_colors[0].b == 0 && SPARK_U16_AT(e, 0x80E) == 0 &&
        e->dust_b == 0) {
        return 2;
    }
    return 0;
}
