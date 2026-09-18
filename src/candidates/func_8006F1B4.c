/*
 * Fourth handler in the D_800114E8 model effect table: a spark burst. With
 * a non-negative mode it seeds 32 sparks, 32 flashes and 64 dust particles
 * from rand(); otherwise it draws the three particle groups as quads through
 * RotAverage4, fades their colours and respawns spent particles. Current best
 * under gcc_2_8_1_g8_split: 1269 instructions against 1270 with opcode
 * distance 1 (0 surplus, 1 missing), with no hard register assignments and
 * no inline assembly.
 *
 * Levers measured on this body:
 * - the spark and flash draw loops are guarded do/while loops;
 * - the spark and flash quads are summed through a byte offset into the
 *   effect, the spark colours and sizes through a four-byte cursor, and the
 *   dust through a halfword frame cursor and a byte cursor over the effect;
 * - the POLY_FT4 and POLY_G4 packets are built through pointers;
 * - the colour fades are if/else chains with a store in each arm, and the
 *   paired colour tests read the pair as a u16;
 * - the dust frame is read with % 2 and / 2, which is retail's halfword
 *   read, and its -0x10/0x10 bounds are named before the loop;
 * - the init loops use pointers derived from the particle cursor, and the
 *   burst sign is an int negated through (s16);
 * - in both draw loops the negated sign goes through otz, which is dead
 *   until RotAverage4 assigns it, and is copied back;
 * - the POLY_FT4 pointer is copied into pr for the spark loop's RotAverage4
 *   arguments, into pq before the flash loop for every later use, and into
 *   pc for the dust loop's func_8005B260 call.
 *
 * Residual: census addu -1, and register choice.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/memory.h"
#include "../psyq/rand.h"
#include "../game/func_80058E1C.h"
#include "../game/model_copy_slot_u16_values.h"
#include "../game/gpu_packets.h"
#include "../game/model_graphics_state.h"
#include "../game/model_slot_properties.h"
#include "../game/model_spark_effect.h"
#include "../game/model_subdivided_effect.h"
#include "../game/model_state_setters.h"
#include "../game/screen_projection.h"
#include "../game/sound.h"

s32 func_8006F1B4(void *data, s32 arg1)
{
    MATRIX ls;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR q0;
    SVECTOR q1;
    SVECTOR q2;
    SVECTOR q3;
    s32 flag;
    s32 p;
    s32 slot;
    GsOT *ot;
    ModelSparkEffect *e;
    SVECTOR *v;
    SVECTOR *w;
    s32 i;
    s32 j;
    s32 k;
    s32 sign;
    s32 dir;
    s32 otz;
    s32 fade;
    SVECTOR *ps;
    u8 *p8;
    u8 *p4;
    s32 off;
    s32 lo16;
    s32 hi16;
    u16 *fr;
    POLY_FT4 *pf;
    POLY_G4 *pg;
    POLY_FT4 *pc;
    POLY_FT4 *pq;
    POLY_FT4 *pr;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8001189C;
    e = data;
    slot = func_80058DCC();
    pf = &ft4;
    pg = &g4;

    if (arg1 >= 0) {
        e->tpage = 0xAE;
        e->clut = 0x3D20;
        for (i = 0; i < 32; i++) {
            e->sparks[i].vx = (rand() - rand()) % 4096 * 320 / 4096;
            e->sparks[i].vy = 0;
            e->sparks[i].vz = (rand() - rand()) % 4096 * 320 / 4096;
            e->spark_colors[i].r = 0x60;
            e->spark_colors[i].g = 0x60;
            e->spark_colors[i].b = 0x7F;
            e->sizes[i].size = 8;
            e->sizes[i].grow = 0x100;
            v = &e->sparks[i] + 48;
            v->vx = (rand() - rand()) % 4096 * 320 / 4096;
            v->vy = -(rand() % 4096 * 128) / 4096;
            v->vz = (rand() - rand()) % 4096 * 320 / 4096;
            e->flash_colors[i].r = 0x40;
            e->flash_colors[i].g = 0x40;
            e->flash_colors[i].b = 0x55;
        }
        for (i = 0; i < 64; i++) {
            v = &e->sparks[i] + 80;
            v->vx = (rand() - rand()) % 4096 * 160 / 4096;
            v->vy = 0;
            v->vz = (rand() - rand()) % 4096 * 160 / 4096;
            w = &e->sparks[i] + 144;
            w->vx = (rand() - rand()) % 4096 * 24 / 4096;
            w->vy = -(rand() % 4096 * 24) / 4096;
            w->vz = (rand() - rand()) % 4096 * 24 / 4096;
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
    func_80058E1C();
    func_80059AEC(1);
    PushMatrix();
    ls = *(MATRIX *)func_80059220();
    Model_CopySlotU16Values(slot, (u16 *)&pos);
    pos.vy = 0;
    GsSetLsMatrix(&ls);
    RotTrans(&pos, (VECTOR *)m.t, (long *)&flag);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);

    setlen(pf, 9);
    setcode(pf, 0x2C);
    pf->tpage = e->tpage;
    pf->u0 = 0x40;
    pf->u2 = 0x40;
    pf->v2 = 0x3F;
    pf->v3 = 0x3F;
    pf->v0 = 0;
    pf->u1 = 0x7F;
    pf->v1 = 0;
    pf->u3 = 0x7F;
    pf->clut = e->clut;
    setlen(pg, 8);
    setcode(pg, 0x38);

    if (e->spark_count != 0) {
        i = 0;
        off = 0;
        p4 = (u8 *)e;
        ps = e->sparks;
        do {
        if (p4[0x70E] != 0 || p4[0x70F] != 0 || p4[0x710] != 0) {
            pf->r0 = p4[0x70E];
            pf->g0 = p4[0x70F];
            pf->b0 = p4[0x710];
            pg->r0 = 0;
            pg->g0 = 0;
            pg->b0 = 0;
            pg->r1 = 0;
            pg->g1 = 0;
            pg->b1 = 0;
            if (e->spark_count < 32) {
                pg->r2 = p4[0x70E] >> 1;
                pg->g2 = p4[0x70F] >> 1;
                pg->b2 = p4[0x710] >> 1;
                pg->r3 = p4[0x70E] >> 1;
                pg->g3 = p4[0x70F] >> 1;
                pg->b3 = p4[0x710] >> 1;
            } else {
                pg->r2 = p4[0x70E];
                pg->g2 = p4[0x70F];
                pg->b2 = p4[0x710];
                pg->r3 = p4[0x70E];
                pg->g3 = p4[0x70F];
                pg->b3 = p4[0x710];
            }
            sign = 1;
            for (j = 0, dir = -2; j < 4; j++, dir++) {
                otz = -(s16)sign;
                sign = otz;
                q0.vx = -((sign << 16) >> 9);
                q0.vy = dir >= 0 ? -0x80 : 0x80;
                q0.vz = 0;
                q1.vx = 0;
                q1.vy = dir >= 0 ? -0x80 : 0x80;
                q1.vz = 0;
                q2.vx = -((sign << 16) >> 9);
                q2.vy = 0;
                q2.vz = 0;
                q3.vx = 0;
                q3.vy = 0;
                q3.vz = 0;
                pr = pf;
                v = (SVECTOR *)((u8 *)e + off);
                for (k = 0; k < 4; k++) {
                    (&q0)[k].vx += v->vx;
                    (&q0)[k].vy += v->vy;
                    (&q0)[k].vz += v->vz;
                }
                otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&pr->x0,
                                  (long *)&pr->x1, (long *)&pr->x2,
                                  (long *)&pr->x3, (long *)&p, (long *)&flag);
                if (otz >= 0 && flag >= 0) {
                    func_8005B260((u32 *)pf, ot, otz & 0xFFFF, 1);
                }
            }
            q0.vx = -*(u16 *)(p4 + 0x100);
            q0.vz = 0;
            q0.vy = -*(u16 *)(p4 + 0x102);
            q1.vx = *(u16 *)(p4 + 0x100);
            q1.vz = 0;
            q1.vy = -*(u16 *)(p4 + 0x102);
            q2.vy = 0;
            q2.vz = 0;
            q2.vx = -*(u16 *)(p4 + 0x100);
            q3.vy = 0;
            q3.vz = 0;
            q3.vx = *(u16 *)(p4 + 0x100);
            v = ps;
            for (k = 0; k < 4; k++) {
                (&q0)[k].vx += v->vx;
                (&q0)[k].vy += v->vy;
                (&q0)[k].vz += v->vz;
            }
            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&pg->x0,
                              (long *)&pg->x1, (long *)&pg->x2, (long *)&pg->x3,
                              (long *)&p, (long *)&flag);
            if (otz >= 0 && flag >= 0) {
                func_8005B260((u32 *)pg, ot, otz & 0xFFFF, 1);
            }
            if (e->mode == 0) {
                *(u16 *)(p4 + 0x100) += 2;
                *(u16 *)(p4 + 0x102) += 0x80;
            }
            if ((s16)*(u16 *)(p4 + 0x100) > 0x20) {
                *(u16 *)(p4 + 0x100) = 0x20;
            }
            if ((s16)*(u16 *)(p4 + 0x102) > 0x600) {
                *(u16 *)(p4 + 0x102) = 0x600;
            }
            if (e->spark_count < 32 || e->mode == 1) {
                if (p4[0x70E] >= 0x10) {
                    p4[0x70E] = p4[0x70E] - 0xF;
                } else {
                    p4[0x70E] = 0;
                }
                if (p4[0x70F] >= 0x10) {
                    p4[0x70F] = p4[0x70F] - 0xF;
                } else {
                    p4[0x70F] = 0;
                }
                if (p4[0x710] >= 0x10) {
                    p4[0x710] = p4[0x710] - 0xF;
                } else {
                    p4[0x710] = 0;
                }
            } else {
                if (p4[0x70E] >= 7) {
                    p4[0x70E] = p4[0x70E] - 4;
                } else {
                    p4[0x70E] = 0;
                }
                if (p4[0x70F] >= 7) {
                    p4[0x70F] = p4[0x70F] - 4;
                } else {
                    p4[0x70F] = 0;
                }
                if (p4[0x710] >= 7) {
                    p4[0x710] = p4[0x710] - 4;
                } else {
                    p4[0x710] = 0;
                }
            }
            if (p4[0x70E] == 0 && p4[0x70F] == 0 && p4[0x710] == 0 && e->mode == 0) {
                ps->vx = (rand() - rand()) % 4096 * 320 / 4096;
                ps->vy = 0;
                ps->vz = (rand() - rand()) % 4096 * 320 / 4096;
                p4[0x70E] = 0xC0;
                p4[0x70F] = 0xC0;
                p4[0x710] = 0xFF;
                *(u16 *)(p4 + 0x100) = 8;
                *(u16 *)(p4 + 0x102) = 0x100;
            }
        }
            ps++;
            p4 += 4;
            off += 8;
            i++;
        } while (i < e->spark_count);
    }

    pf->u0 = 0x40;
    pf->v0 = 0x40;
    pf->u1 = 0x7F;
    pf->v1 = 0x40;
    pf->u2 = 0x40;
    pf->v2 = 0x7F;
    pf->u3 = 0x7F;
    pf->v3 = 0x7F;
    pq = pf;
    if (e->flash_count != 0) {
        i = 0;
        off = 0;
        p4 = (u8 *)e;
        do {
        if (p4[0x78E] != 0 || p4[0x78F] != 0 || p4[0x790] != 0) {
            sign = 1;
            pq->r0 = p4[0x78E];
            pq->g0 = p4[0x78F];
            pq->b0 = p4[0x790];
            for (j = 0, dir = -2; j < 4; j++, dir++) {
                otz = -(s16)sign;
                sign = otz;
                q0.vx = -(s16)sign * 0x50;
                q0.vy = dir >= 0 ? -0x50 : 0x50;
                q0.vz = 0;
                q1.vx = 0;
                q1.vy = dir >= 0 ? -0x50 : 0x50;
                q1.vz = 0;
                q2.vx = -(s16)sign * 0x50;
                q2.vy = 0;
                q2.vz = 0;
                q3.vx = 0;
                q3.vy = 0;
                q3.vz = 0;
                v = (SVECTOR *)((u8 *)e + off + 0x180);
                for (k = 0; k < 4; k++) {
                    (&q0)[k].vx += v->vx;
                    (&q0)[k].vy += v->vy;
                    (&q0)[k].vz += v->vz;
                }
                otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&pq->x0,
                                  (long *)&pq->x1, (long *)&pq->x2,
                                  (long *)&pq->x3, (long *)&p, (long *)&flag);
                if (otz >= 0 && flag >= 0) {
                    func_8005B260((u32 *)pq, ot, otz & 0xFFFF, 1);
                }
            }
            if (p4[0x78E] >= 0x20) {
                p4[0x78E] = p4[0x78E] - 0x1F;
            } else {
                p4[0x78E] = 0;
            }
            if (p4[0x78F] >= 0x20) {
                p4[0x78F] = p4[0x78F] - 0x1F;
            } else {
                p4[0x78F] = 0;
            }
            if (p4[0x790] >= 0x20) {
                p4[0x790] = p4[0x790] - 0x1F;
            } else {
                p4[0x790] = 0;
            }
            if (p4[0x78E] == 0 && p4[0x78F] == 0 && p4[0x790] == 0) {
                e->flashes[i].vx = (rand() - rand()) % 4096 * 320 / 4096;
                e->flashes[i].vy = -(rand() % 4096 * 128) / 4096;
                e->flashes[i].vz = (rand() - rand()) % 4096 * 320 / 4096;
                p4[0x78E] = 0x40;
                p4[0x78F] = 0x40;
                p4[0x790] = 0x55;
            }
        }
            p4 += 4;
            off += 8;
            i++;
        } while (i < e->flash_count);
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
    fade = e->fade;
    func_80059590(slot, 5, fade, fade, fade);
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
        func_80059590(slot, 5, 0, 0, 0);
    }
    if (*(u16 *)&e->dust_r != 0 || e->dust_b != 0) {
        i = 0;
        if (e->mode == 1) {
            lo16 = -0x10;
            hi16 = 0x10;
            pq->r0 = e->dust_r;
            pc = pq;
            pq->g0 = e->dust_g;
            pq->b0 = e->dust_b;
            fr = (u16 *)e;
            p8 = (u8 *)e;
            for (; i < 64; i++) {
                pq->u0 = (fr[0x340] % 2) << 5;
                pq->v0 = (fr[0x340] / 2 << 5) + 0x40;
                pq->u1 = ((fr[0x340] % 2) << 5) + 0x1F;
                pq->v1 = (fr[0x340] / 2 << 5) + 0x40;
                pq->u2 = (fr[0x340] % 2) << 5;
                pq->v2 = (fr[0x340] / 2 << 5) + 0x5F;
                pq->u3 = ((fr[0x340] % 2) << 5) + 0x1F;
                pq->v3 = (fr[0x340] / 2 << 5) + 0x5F;
                q0.vx = lo16;
                q0.vy = lo16;
                q0.vz = 0;
                q1.vx = hi16;
                q1.vy = lo16;
                q1.vz = 0;
                q2.vx = lo16;
                q2.vy = hi16;
                q2.vz = 0;
                q3.vx = hi16;
                q3.vy = hi16;
                q3.vz = 0;
                v = (SVECTOR *)(p8 + 0x280);
                for (k = 0; k < 4; k++) {
                    (&q0)[k].vx += v->vx;
                    (&q0)[k].vy += v->vy;
                    (&q0)[k].vz += v->vz;
                }
                otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&pq->x0,
                                  (long *)&pq->x1, (long *)&pq->x2,
                                  (long *)&pq->x3, (long *)&p, (long *)&flag);
                if (otz >= 0 && flag >= 0) {
                    func_8005B260((u32 *)pc, ot, otz & 0xFFFF, 1);
                }
                w = (SVECTOR *)(p8 + 0x480);
                v->vx += w->vx;
                v->vy += w->vy;
                v->vz += w->vz;
                fr[0x340] = (fr[0x340] + 1) % 4;
                fr++;
                p8 += 8;
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
    if (*(u16 *)&e->flash_colors[0].r == 0 && e->flash_colors[0].b == 0 &&
        *(u16 *)&e->dust_r == 0 && e->dust_b == 0) {
        return 2;
    }
    return 0;
}
