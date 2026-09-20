/*
 * Fourth handler in the D_800114E8 model effect table: a spark burst. With
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

#define RAND_SPREAD(n) ((rand() - rand()) % 4096 * (n) / 4096)
#define RAND_DROP(n) (-(rand() % 4096 * (n)) / 4096)
#define HI16(p, o) (*(u16 *)((u8 *)(p) + (o)))

s32 func_8006F1B4(void *data, s32 arg1)
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
    scale = D_8001189C;
    e = data;
    slot = Model_GetActiveSlotIndex();

    if (arg1 >= 0) {
        e->tpage = 0xAE;
        e->clut = 0x3D20;
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
    Model_CopySlotU16Values(slot, (u16 *)&pos);
    pos.vy = 0;
    GsSetLsMatrix(&ls);
    RotTrans(&pos, (VECTOR *)m.t, (long *)&flag);
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
                otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (long *)&f->x0, (long *)&f->x1,
                                  (long *)&f->x2, (long *)&f->x3, (long *)&p, (long *)&flag);
                if (otz >= 0 && flag >= 0) {
                    func_8005B260((u32 *)f, ot, otz & 0xFFFF, 1);
                }
            }
            setVector(&q[0], -e->sizes[i].size, -e->sizes[i].grow, 0);
            setVector(&q[1], e->sizes[i].size, -e->sizes[i].grow, 0);
            setVector(&q[2], -e->sizes[i].size, 0, 0);
            setVector(&q[3], e->sizes[i].size, 0, 0);
            for (k = 0; k < 4; k++) {
                addVector(&q[k], &e->sparks[i]);
            }
            otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (long *)&g->x0, (long *)&g->x1,
                              (long *)&g->x2, (long *)&g->x3, (long *)&p, (long *)&flag);
            if (otz >= 0 && flag >= 0) {
                func_8005B260((u32 *)g, ot, otz & 0xFFFF, 1);
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
                otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (long *)&f->x0, (long *)&f->x1,
                                  (long *)&f->x2, (long *)&f->x3, (long *)&p, (long *)&flag);
                if (otz >= 0 && flag >= 0) {
                    func_8005B260((u32 *)f, ot, otz & 0xFFFF, 1);
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
    if (HI16(e, 0x80E) != 0 || e->dust_b != 0) {
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
                otz = RotAverage4(&q[0], &q[1], &q[2], &q[3], (long *)&f->x0, (long *)&f->x1,
                                  (long *)&f->x2, (long *)&f->x3, (long *)&p, (long *)&flag);
                if (otz >= 0 && flag >= 0) {
                    func_8005B260((u32 *)f, ot, otz & 0xFFFF, 1);
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
    if (HI16(e, 0x78E) == 0 && e->flash_colors[0].b == 0 && HI16(e, 0x80E) == 0 &&
        e->dust_b == 0) {
        return 2;
    }
    return 0;
}
