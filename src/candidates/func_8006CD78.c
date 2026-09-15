/*
 * A burst handler in the D_800114E8 model effect table. With a non-negative
 * mode it seeds three rings, 64 dust particles, 32 sparks, 32 embers and 64
 * smoke particles from ccos/csin and rand(); otherwise it draws the rings,
 * the flash, the dust, the sparks and the smoke as quads through RotAverage4,
 * steps its phase and stage and fades every group. Current best under
 * gcc_2_8_1_g8_split: 2306 instructions against 2319 with opcode distance
 * 43 (15 surplus, 28 missing), with no hard register assignments and no
 * inline assembly.
 *
 * Levers measured on this body:
 * - the burst sign is an s16, which retail keeps in a halfword stack slot;
 * - the paired colour tests read a word and mask it with 0xFFFF0000;
 * - every loop has its own counter name, so no counter is spilled;
 * - the dust and smoke frames are read with % 2 and / 2, which gives
 *   retail's halfword read at each use;
 * - each quad's z term is -(radius >> 2) * (d < 0 ? x : y); the product is
 *   distributed into both arms and the quarter radius is computed once
 *   before the branch, as in retail;
 * - the ring stop test is one || chain over the three absolute components,
 *   so the move block falls through and the stop block follows it;
 * - the spark frame is stepped without an s16 cast, which keeps retail's
 *   unsigned halfword load;
 * - the sign is flipped as sign * -1, which keeps retail's sign extension
 *   before the negation, and the last quad block reads the flipped sign
 *   back instead of negating it again;
 * - the third ring component's absolute value tests >= 0 first, which
 *   drops a duplicated test and jump;
 * - the ember and smoke seeding loops walk embers, smoke and smoke_speed
 *   through SVECTOR cursors, as retail does.
 *
 * The earlier 2321-instruction build was two faults cancelling: 28 surplus
 * instructions in the two quad loops against the 26-instruction shortfall
 * elsewhere that this build still shows.
 *
 * Measured and not kept: named pointers for the vector sums and an FT4
 * accessed through a pointer (both shorten the build but spill a register,
 * raising the surplus to 24-46), and the spark colour fades as ternary
 * stores (surplus 27-28). Cursors in the ring, dust and spark seeding
 * loops add surplus the same way (17-41).
 *
 * Residual: census addiu -2, addu -4, andi +2, beqz +1, lbu +5, lh +2,
 * lhu -7, lw +1, negu +2, nop -14, slt -1, sltu +2. Retail reaches the FT4
 * and the vector sums through pointers; spelling that here spills a
 * register (2301 instructions, surplus 19).
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/memory.h"
#include "../psyq/rand.h"
#include "../game/func_80057E20.h"
#include "../game/func_80058E1C.h"
#include "../game/model_copy_slot_u16_values.h"
#include "../game/gpu_packets.h"
#include "../game/model_graphics_state.h"
#include "../game/model_burst_effect.h"
#include "../game/model_geometry_tables.h"
#include "../game/model_slot_properties.h"
#include "../game/model_state_setters.h"
#include "../game/screen_projection.h"
#include "../game/sound.h"

extern VECTOR D_8001188C;

#define HI16(p, o) (*(u32 *)((u8 *)(p) + (o)) & 0xFFFF0000)

s32 func_8006CD78(void *data, s32 arg1)
{
    MATRIX ls;
    MATRIX m;
    POLY_FT4 ft4;
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
    s16 sign;
    ModelBurstEffect *e;
    u16 *w;
    u8 *c;
    SVECTOR *t;
    SVECTOR *u;
    s32 i;
    s32 j;
    s32 k;
    s32 l;
    s32 n1;
    s32 n2;
    s32 n3;
    s32 n4;
    s32 n5;
    s32 n6;
    s32 n7;
    s32 n8;
    s32 n9;
    s32 n10;
    s32 n11;
    s32 a;
    s32 b;
    s32 s;
    s32 d;
    s32 lo;
    s32 hi;
    s32 otz;
    u16 r16;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8001188C;
    e = data;
    slot = func_80058DCC();

    if (arg1 >= 0) {
        i = 0;
        w = D_80091610;
        c = (u8 *)e;
        e->table = (u8 *)D_800916D4;
        e->tpage = 0xAE;
        e->clut = 0x3D20;
        do {
            i++;
            *(u16 *)(c + 0xD8E) = ((w[0] & 3) << 7) | (((s16)(w[3] & 0x100) >> 4) | 0x20) |
                                  ((u32)(w[2] & 0x3FF) >> 6) | ((w[3] & 0x200) * 4);
            *(u16 *)(c + 0xD90) = (w[9] << 6) | ((w[8] >> 4) & 0x3F);
            w += 14;
            c += 4;
        } while (i <= 0);
        func_80057E20(slot, (ModelEffectAdjustment *)&pos);
        for (n1 = 0, a = 0; n1 < 3; n1++, a += 0x555) {
            b = ccos(a);
            r16 = pos.vz;
            if (pos.vz < pos.vx) {
                r16 = pos.vx;
            }
            e->rings[n1].vx = (s16)r16 * (b * 2) / 4096;
            e->rings[n1].vy = 0;
            d = csin(a);
            r16 = pos.vz;
            if (pos.vz < pos.vx) {
                r16 = pos.vx;
            }
            e->rings[n1].vz = (s16)r16 * (d * 2) / 4096;
            e->ring_speed[n1].vx = -e->rings[n1].vx / 48;
            e->ring_speed[n1].vy = 0;
            e->ring_speed[n1].vz = -e->rings[n1].vz / 48;
            e->colors[n1].r = e->table[n1] >> 3;
            e->colors[n1].g = e->table[n1 + 3] >> 3;
            e->colors[n1].b = e->table[n1 + 6] >> 3;
        }
        r16 = pos.vz;
        if (pos.vz < pos.vx) {
            r16 = pos.vx;
        }
        e->radius = (s16)r16 / 2 + 200;
        if (e->radius > 0x200) {
            e->radius = 0x200;
        }
        e->flash_r = 0x80;
        e->flash_g = 0x80;
        e->flash_b = 0x80;
        for (n2 = 0; n2 < 64; n2++) {
            a = (rand() - rand()) % 4096;
            b = (rand() - rand()) % 4096;
            e->dust[n2].vx = e->radius * a / 4096;
            e->dust[n2].vy = 0;
            e->dust[n2].vz = e->radius * b / 4096;
            e->dust_speed[n2].vx = a * 24 / 4096;
            e->dust_speed[n2].vy = -(rand() % 4096 * 24) / 4096;
            e->dust_speed[n2].vz = b * 24 / 4096;
            e->dust_frame[n2] = rand() % 4;
        }
        for (n3 = 0; n3 < 32; n3++) {
            a = n3 << 7;
            e->sparks[n3].vx = e->radius * ccos(a) / 4096;
            e->sparks[n3].vy = 0;
            e->sparks[n3].vz = e->radius * csin(a) / 4096;
            e->spark_drift[n3].vx = -e->sparks[n3].vx / 128;
            e->spark_drift[n3].vy = -0x10;
            e->spark_drift[n3].vz = -e->sparks[n3].vz / 128;
            e->spark_rise[n3].vx = ccos(a) / 128;
            e->spark_rise[n3].vy = 0;
            e->spark_rise[n3].vz = csin(a) / 128;
            e->spark_frame[n3] = rand() % 8;
            e->spark_colors[n3].r = 1;
            e->spark_colors[n3].g = 1;
            e->spark_colors[n3].b = 1;
        }
        t = e->embers;
        for (n4 = 0; n4 < 32; n4++, t++) {
            t->vx = (rand() - rand()) % 4096 * 0xA0 / 4096;
            t->vy = (rand() - rand()) % 4096 * 0xA0 / 4096;
            t->vz = (rand() - rand()) % 4096 * 0xA0 / 4096;
        }
        u = e->smoke;
        t = e->smoke_speed;
        for (n5 = 0; n5 < 64; n5++, t++, u++) {
            u->vx = 0;
            u->vy = 0;
            u->vz = 0;
            t->vx = (rand() - rand()) % 4096 * 0x60 / 4096;
            t->vy = (rand() - rand()) % 4096 * 0x18 / 4096;
            t->vz = (rand() - rand()) % 4096 * 0x60 / 4096;
            e->smoke_frame[n5] = rand() % 4;
        }
        e->smoke_r = 0x80;
        e->smoke_g = 0x40;
        e->smoke_b = 0x79;
        Model_CopySlotU16Values(slot, (u16 *)&rot);
        rot.vy = 0;
        e->origin.vx = rot.vx;
        e->origin.vy = rot.vy;
        e->phase = 0;
        e->stage = 0;
        e->frame = 0;
        e->spark_count = 1;
        e->fade = 0;
        e->origin.vz = rot.vz;
        SD_SEPlay(0, 0xFF, 0);
        return 0;
    }

    ot = func_80058F10();
    func_80058E1C();
    func_80059AEC(1);
    PushMatrix();
    ls = *(MATRIX *)func_80059220();
    Model_CopySlotU16Values(slot, (u16 *)&pos);
    pos.vy = 0;
    e->origin.vx = pos.vx;
    e->origin.vy = pos.vy;
    e->origin.vz = pos.vz;
    if (e->phase == 0) {
        rot.vx = 0;
        rot.vz = 0;
        rot.vy = (e->frame * 0xA0) & 0xFFF;
    }
    GsSetLsMatrix(&ls);
    RotTrans(&e->origin, (VECTOR *)m.t, (long *)&flag);
    RotMatrix(&rot, &m);
    MulMatrix2(&ls, &m);
    ScaleMatrix(&m, &scale);
    GsSetLsMatrix(&m);
    setlen(&ft4, 9);
    setcode(&ft4, 0x2C);
    ft4.tpage = e->tpage;
    ft4.u0 = 0;
    ft4.v0 = 0;
    ft4.u1 = 0x3F;
    ft4.v1 = 0;
    ft4.u2 = 0;
    ft4.v2 = 0x3F;
    ft4.u3 = 0x3F;
    ft4.v3 = 0x3F;
    ft4.clut = e->clut;

    if (e->phase == 0) {
        for (n6 = 0; n6 < 3; n6++) {
            if (e->colors[n6].r != 0 || e->colors[n6].g != 0 || e->colors[n6].b != 0) {
                sign = 1;
                ft4.r0 = e->colors[n6].r;
                ft4.g0 = e->colors[n6].g;
                ft4.b0 = e->colors[n6].b;
                for (j = 0; j < 4; j++) {
                    d = j - 2;
                    sign = sign * -1;
                    s = (s16)sign * -1;
                    for (k = 0; k < 4; k++) {
                        for (l = 0, lo = -1, hi = 1; l < 4; l++, lo--, hi++) {
                            q0.vy = 0;
                            q0.vx = s * -(e->radius >> 2) * (k + 1);
                            q0.vz = -(e->radius >> 2) * (d < 0 ? lo : hi);
                            q1.vy = 0;
                            q1.vx = s * -(e->radius >> 2) * k;
                            q1.vz = -(e->radius >> 2) * (d < 0 ? lo : hi);
                            q2.vy = 0;
                            q2.vx = s * -(e->radius >> 2) * (k + 1);
                            q2.vz = -(e->radius >> 2) * (d < 0 ? -l : l);
                            q3.vy = 0;
                            q3.vx = s * -(e->radius >> 2) * k;
                            q3.vz = -(e->radius >> 2) * (d < 0 ? -l : l);
                            q0.vx += e->rings[n6].vx;
                            q0.vy += e->rings[n6].vy;
                            q0.vz += e->rings[n6].vz;
                            q1.vx += e->rings[n6].vx;
                            q1.vy += e->rings[n6].vy;
                            q1.vz += e->rings[n6].vz;
                            q2.vx += e->rings[n6].vx;
                            q2.vy += e->rings[n6].vy;
                            q2.vz += e->rings[n6].vz;
                            q3.vx += e->rings[n6].vx;
                            q3.vy += e->rings[n6].vy;
                            q3.vz += e->rings[n6].vz;
                            a = (3 - k) * 16;
                            b = (3 - l) * 16;
                            ft4.v0 = b;
                            ft4.v1 = b;
                            ft4.v2 = b + 0xF;
                            ft4.v3 = b + 0xF;
                            ft4.u0 = a;
                            ft4.u1 = a + 0xF;
                            ft4.u2 = a;
                            ft4.u3 = a + 0xF;
                            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0,
                                              (long *)&ft4.x1, (long *)&ft4.x2,
                                              (long *)&ft4.x3, (long *)&p, (long *)&flag);
                            if (otz >= 0 && flag >= 0) {
                                func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
                            }
                        }
                    }
                }
                if ((e->rings[n6].vx < 0 ? -e->rings[n6].vx : e->rings[n6].vx) >= 0x21 ||
                    (e->rings[n6].vy < 0 ? -e->rings[n6].vy : e->rings[n6].vy) >= 0x21 ||
                    (e->rings[n6].vz >= 0 ? e->rings[n6].vz : -e->rings[n6].vz) >= 0x21) {
                    e->rings[n6].vx += e->ring_speed[n6].vx;
                    e->rings[n6].vy += e->ring_speed[n6].vy;
                    e->rings[n6].vz += e->ring_speed[n6].vz;
                } else {
                    if (e->phase == 0) {
                        e->phase = 1;
                    }
                    e->rings[n6].vx = 0;
                    e->rings[n6].vy = 0;
                    e->rings[n6].vz = 0;
                }
                if (e->phase == 0) {
                    a = e->colors[n6].r;
                    b = e->table[n6] >> 1;
                    if (a < b - 9) {
                        e->colors[n6].r = a + 8;
                    } else {
                        e->colors[n6].r = b;
                    }
                    a = e->colors[n6].g;
                    b = e->table[n6 + 3] >> 1;
                    if (a < b - 9) {
                        e->colors[n6].g = a + 8;
                    } else {
                        e->colors[n6].g = b;
                    }
                    a = e->colors[n6].b;
                    b = e->table[n6 + 6] >> 1;
                    if (a < b - 9) {
                        e->colors[n6].b = a + 8;
                    } else {
                        e->colors[n6].b = b;
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
        ft4.r0 = e->flash_r;
        sign = 1;
        ft4.g0 = e->flash_g;
        ft4.b0 = e->flash_b;
        for (j = 0; j < 4; j++) {
            d = j - 2;
            sign = sign * -1;
            s = (s16)sign * -1;
            for (k = 0; k < 4; k++) {
                for (l = 0, lo = -1, hi = 1; l < 4; l++, lo--, hi++) {
                    q0.vy = 0;
                    q0.vx = s * -(e->radius >> 2) * (k + 1);
                    q0.vz = -(e->radius >> 2) * (d < 0 ? lo : hi);
                    q1.vy = 0;
                    q1.vx = s * -(e->radius >> 2) * k;
                    q1.vz = -(e->radius >> 2) * (d < 0 ? lo : hi);
                    q2.vy = 0;
                    q2.vx = s * -(e->radius >> 2) * (k + 1);
                    q2.vz = -(e->radius >> 2) * (d < 0 ? -l : l);
                    q3.vy = 0;
                    q3.vx = s * -(e->radius >> 2) * k;
                    q3.vz = -(e->radius >> 2) * (d < 0 ? -l : l);
                    a = (3 - k) * 16;
                    b = (3 - l) * 16;
                    ft4.v0 = b;
                    ft4.v1 = b;
                    ft4.v2 = b + 0xF;
                    ft4.v3 = b + 0xF;
                    ft4.u0 = a;
                    ft4.u1 = a + 0xF;
                    ft4.u2 = a;
                    ft4.u3 = a + 0xF;
                    otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0,
                                      (long *)&ft4.x1, (long *)&ft4.x2,
                                      (long *)&ft4.x3, (long *)&p, (long *)&flag);
                    if (otz >= 0 && flag >= 0) {
                        func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
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
            a = e->flash_r + 8;
            if (e->flash_r >= 0xF6) {
                a = 0xFF;
            }
            e->flash_r = a;
            a = e->flash_g + 8;
            if (e->flash_g >= 0xF6) {
                a = 0xFF;
            }
            e->flash_g = a;
            a = e->flash_b + 8;
            if (e->flash_b >= 0xF6) {
                a = 0xFF;
            }
            e->flash_b = a;
            e->radius += 4;
            if (HI16(e, 0xD9C) == 0xFFFF0000 && e->flash_b == 0xFF) {
                e->phase = 2;
            }
        } else if (e->phase == 2) {
            a = 0x80;
            if (e->flash_r >= 0x89) {
                a = e->flash_r - 8;
            }
            e->flash_r = a;
            a = 0x80;
            if (e->flash_g >= 0x89) {
                a = e->flash_g - 8;
            }
            e->flash_g = a;
            a = 0x80;
            if (e->flash_b >= 0x89) {
                a = e->flash_b - 8;
            }
            e->flash_b = a;
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
        RotTrans(&e->origin, (VECTOR *)m.t, (long *)&flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (n7 = 0; n7 < 64; n7++) {
            c = (u8 *)&e->colors[n7 % 3];
            ft4.r0 = c[0];
            ft4.g0 = c[1];
            ft4.b0 = c[2];
            ft4.u0 = (e->dust_frame[n7] % 2) << 5;
            ft4.v0 = (e->dust_frame[n7] / 2 << 5) + 0x40;
            ft4.u1 = ((e->dust_frame[n7] % 2) << 5) + 0x1F;
            ft4.v1 = (e->dust_frame[n7] / 2 << 5) + 0x40;
            ft4.u2 = (e->dust_frame[n7] % 2) << 5;
            ft4.v2 = (e->dust_frame[n7] / 2 << 5) + 0x5F;
            ft4.u3 = ((e->dust_frame[n7] % 2) << 5) + 0x1F;
            ft4.v3 = (e->dust_frame[n7] / 2 << 5) + 0x5F;
            q0.vx = -0x20;
            q0.vy = -0x20;
            q0.vz = 0;
            q1.vx = 0x20;
            q1.vy = -0x20;
            q1.vz = 0;
            q2.vx = -0x20;
            q2.vy = 0x20;
            q2.vz = 0;
            q3.vx = 0x20;
            q3.vy = 0x20;
            q3.vz = 0;
            for (k = 0, t = &q0; k < 4; k++, t++) {
                t->vx += e->dust[n7].vx;
                t->vy += e->dust[n7].vy;
                t->vz += e->dust[n7].vz;
            }
            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0, (long *)&ft4.x1,
                              (long *)&ft4.x2, (long *)&ft4.x3, (long *)&p, (long *)&flag);
            if (otz >= 0) {
                if (flag >= 0) {
                    func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
                }
            }
            e->dust[n7].vx += e->dust_speed[n7].vx;
            e->dust[n7].vy += e->dust_speed[n7].vy;
            e->dust[n7].vz += e->dust_speed[n7].vz;
            e->dust_frame[n7] = ((s16)e->dust_frame[n7] + 1) % 4;
        }
        for (c = (u8 *)e->colors; c < (u8 *)e->colors + 12; c += 4) {
            if (c[0] >= 0x10) {
                c[0] = c[0] - 0xF;
            } else {
                c[0] = 0;
            }
            if (c[1] >= 0x10) {
                c[1] = c[1] - 0xF;
            } else {
                c[1] = 0;
            }
            if (c[2] >= 0x10) {
                c[2] = c[2] - 0xF;
            } else {
                c[2] = 0;
            }
        }
    }

    ft4.tpage = e->spark_tpage;
    ft4.clut = e->spark_clut;
    q0.vx = -0x40;
    q0.vy = -0x100;
    q0.vz = 0;
    q1.vx = 0x40;
    q1.vy = -0x100;
    q1.vz = 0;
    q2.vx = -0x40;
    q2.vy = 0;
    q2.vz = 0;
    q3.vx = 0x40;
    q3.vy = 0;
    q3.vz = 0;
    for (n8 = 0; n8 < e->spark_count; n8++) {
        if (e->spark_colors[n8].r != 0 || e->spark_colors[n8].g != 0 ||
            e->spark_colors[n8].b != 0) {
            pos.vx = e->origin.vx;
            pos.vy = e->origin.vy;
            pos.vz = e->origin.vz;
            pos.vx += e->sparks[n8].vx;
            pos.vy += e->sparks[n8].vy;
            pos.vz += e->sparks[n8].vz;
            GsSetLsMatrix(&ls);
            RotTrans(&pos, (VECTOR *)m.t, (long *)&flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            ft4.v0 = 0;
            ft4.u0 = e->spark_frame[n8] << 5;
            ft4.v1 = 0;
            ft4.u1 = (e->spark_frame[n8] << 5) + 0x1F;
            ft4.v2 = 0x3F;
            ft4.u2 = e->spark_frame[n8] << 5;
            ft4.v3 = 0x3F;
            ft4.u3 = (e->spark_frame[n8] << 5) + 0x1F;
            ft4.r0 = e->spark_colors[n8].r;
            ft4.g0 = e->spark_colors[n8].g;
            ft4.b0 = e->spark_colors[n8].b;
            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0, (long *)&ft4.x1,
                              (long *)&ft4.x2, (long *)&ft4.x3, (long *)&p, (long *)&flag);
            if (otz >= 0 && flag >= 0) {
                func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
            }
            e->spark_frame[n8] = (e->spark_frame[n8] + 1) % 8;
            if (e->stage < 2) {
                a = e->spark_colors[n8].r + 0x1F;
                if (e->spark_colors[n8].r >= 0x61) {
                    a = 0x80;
                }
                e->spark_colors[n8].r = a;
                a = e->spark_colors[n8].g + 0x1F;
                if (e->spark_colors[n8].g >= 0x61) {
                    a = 0x80;
                }
                e->spark_colors[n8].g = a;
                a = e->spark_colors[n8].b + 0x1F;
                if (e->spark_colors[n8].b >= 0x61) {
                    a = 0x80;
                }
                e->spark_colors[n8].b = a;
            }
            if (e->stage == 1) {
                e->sparks[n8].vx += e->spark_drift[n8].vx;
                e->sparks[n8].vy += e->spark_drift[n8].vy;
                e->sparks[n8].vz += e->spark_drift[n8].vz;
            }
            if (e->stage == 2) {
                e->sparks[n8].vx += e->spark_rise[n8].vx;
                e->sparks[n8].vy += e->spark_rise[n8].vy;
                e->sparks[n8].vz += e->spark_rise[n8].vz;
                e->spark_rise[n8].vy += 2;
                if (e->spark_colors[n8].r >= 9) {
                    e->spark_colors[n8].r = e->spark_colors[n8].r - 8;
                } else {
                    e->spark_colors[n8].r = 0;
                }
                if (e->spark_colors[n8].g >= 9) {
                    e->spark_colors[n8].g = e->spark_colors[n8].g - 8;
                } else {
                    e->spark_colors[n8].g = 0;
                }
                if (e->spark_colors[n8].b >= 9) {
                    e->spark_colors[n8].b = e->spark_colors[n8].b - 8;
                } else {
                    e->spark_colors[n8].b = 0;
                }
            }
        }
    }

    c = (u8 *)&e->spark_colors[e->spark_count - 1];
    if ((c[0] >= 0x41 || c[1] >= 0x41 || c[2] >= 0x41) && e->stage == 0) {
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
        ft4.tpage = e->tpage;
        sign = 1;
        ft4.clut = e->clut;
        pos.vx = 0;
        pos.vy = -0x220;
        pos.vz = 0;
        pos.vx += e->origin.vx;
        pos.vy += e->origin.vy;
        pos.vz += e->origin.vz;
        GsSetLsMatrix(&ls);
        RotTrans(&pos, (VECTOR *)m.t, (long *)&flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        for (n9 = 0; n9 < 4; n9++) {
            sign = sign * -1;
            q0.vx = -((sign << 16) >> 9);
            q0.vy = n9 - 2 >= 0 ? -0x80 : 0x80;
            q0.vz = 0;
            q1.vx = 0;
            q1.vy = n9 - 2 >= 0 ? -0x80 : 0x80;
            q1.vz = 0;
            q2.vy = 0;
            q2.vz = 0;
            q3.vx = 0;
            q3.vy = 0;
            q3.vz = 0;
            q2.vx = -((sign << 16) >> 9);
            ft4.r0 = e->smoke_r;
            ft4.g0 = e->smoke_g;
            ft4.v2 = 0x3F;
            ft4.v3 = 0x3F;
            ft4.u0 = 0x40;
            ft4.v0 = 0;
            ft4.u1 = 0x7F;
            ft4.v1 = 0;
            ft4.u2 = 0x40;
            ft4.u3 = 0x7F;
            ft4.b0 = e->smoke_b;
            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0, (long *)&ft4.x1,
                              (long *)&ft4.x2, (long *)&ft4.x3, (long *)&p, (long *)&flag);
            if (otz >= 0) {
                if (flag >= 0) {
                    func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
                }
            }
            ft4.r0 = e->smoke_r >> 1;
            ft4.g0 = e->smoke_g >> 1;
            ft4.u0 = 0x40;
            ft4.v0 = 0x40;
            ft4.u1 = 0x7F;
            ft4.v1 = 0x40;
            ft4.u2 = 0x40;
            ft4.v2 = 0x7F;
            ft4.u3 = 0x7F;
            ft4.v3 = 0x7F;
            ft4.b0 = e->smoke_b >> 1;
            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0, (long *)&ft4.x1,
                              (long *)&ft4.x2, (long *)&ft4.x3, (long *)&p, (long *)&flag);
            if (otz >= 0 && flag >= 0) {
                func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
            }
        }
        ft4.v0 = 0x80;
        ft4.v1 = 0x80;
        ft4.v2 = 0xFF;
        ft4.v3 = 0xFF;
        ft4.u0 = 0;
        ft4.u1 = 0x1F;
        ft4.u2 = 0;
        ft4.u3 = 0x1F;
        q0.vx = -4;
        q0.vy = 0;
        q0.vz = 0;
        q1.vx = 4;
        q1.vy = 0;
        q1.vz = 0;
        ft4.r0 = e->smoke_r >> 2;
        ft4.g0 = e->smoke_g >> 2;
        ft4.b0 = e->smoke_b >> 2;
        for (n10 = 0; n10 < 32; n10++) {
            q2.vx = -2;
            q2.vy = 0;
            q2.vz = 0;
            q3.vx = 2;
            q3.vy = 0;
            q3.vz = 0;
            q2.vx += e->embers[n10].vx;
            q2.vy += e->embers[n10].vy;
            q2.vz += e->embers[n10].vz;
            q3.vx += e->embers[n10].vx;
            q3.vy += e->embers[n10].vy;
            q3.vz += e->embers[n10].vz;
            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0, (long *)&ft4.x1,
                              (long *)&ft4.x2, (long *)&ft4.x3, (long *)&p, (long *)&flag);
            if (otz >= 0 && flag >= 0) {
                func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
            }
        }
        for (n11 = 0; n11 < 64; n11++) {
            ft4.r0 = e->smoke_r;
            ft4.g0 = e->smoke_g;
            ft4.b0 = e->smoke_b;
            ft4.u0 = (e->smoke_frame[n11] % 2) << 5;
            ft4.v0 = (e->smoke_frame[n11] / 2 << 5) + 0x40;
            ft4.u1 = ((e->smoke_frame[n11] % 2) << 5) + 0x1F;
            ft4.v1 = (e->smoke_frame[n11] / 2 << 5) + 0x40;
            ft4.u2 = (e->smoke_frame[n11] % 2) << 5;
            ft4.v2 = (e->smoke_frame[n11] / 2 << 5) + 0x5F;
            ft4.u3 = ((e->smoke_frame[n11] % 2) << 5) + 0x1F;
            ft4.v3 = (e->smoke_frame[n11] / 2 << 5) + 0x5F;
            q0.vx = -0x20;
            q0.vy = -0x20;
            q0.vz = 0;
            q1.vx = 0x20;
            q1.vy = -0x20;
            q1.vz = 0;
            q2.vx = -0x20;
            q2.vy = 0x20;
            q2.vz = 0;
            q3.vx = 0x20;
            q3.vy = 0x20;
            q3.vz = 0;
            for (k = 0, t = &q0; k < 4; k++, t++) {
                t->vx += e->smoke[n11].vx;
                t->vy += e->smoke[n11].vy;
                t->vz += e->smoke[n11].vz;
            }
            otz = RotAverage4(&q0, &q1, &q2, &q3, (long *)&ft4.x0, (long *)&ft4.x1,
                              (long *)&ft4.x2, (long *)&ft4.x3, (long *)&p, (long *)&flag);
            if (otz >= 0) {
                if (flag >= 0) {
                    func_8005B260((u32 *)&ft4, ot, otz & 0xFFFF, 1);
                }
            }
            e->smoke[n11].vx += e->smoke_speed[n11].vx;
            e->smoke[n11].vy += e->smoke_speed[n11].vy;
            e->smoke[n11].vz += e->smoke_speed[n11].vz;
            e->smoke_frame[n11] = ((s16)e->smoke_frame[n11] + 1) % 4;
            e->smoke_speed[n11].vy += 2;
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

    k = 5;
    if (e->fade >= 0) {
        a = (u8)e->fade;
        b = a;
    } else {
        k = 0;
        a = 0x80;
        b = 0x80;
    }
    func_80059590(slot, k, a, b, a);
    if (e->stage != 0) {
        if ((u8)e->fade < 0x7C) {
            e->fade = (u8)e->fade + 4;
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
