#include "../../types.h"
#include "effect_15.h"

void func_8014FF40(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR translation;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    SVECTOR quad[4];
    POLY_FT4 *packet;
    DuelEffect15Work *work;
    s32 i;

    memset(&translation, 0, sizeof(translation));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146168;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 8) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015AC08[phase];
            if (work->config->selector == 999) {
                Duel_CollectFieldRowCardObjects(D_8015B7A0, 1);
            } else {
                Duel_CollectMatchingFieldCardObjects(D_8015B7A0,
                                                     work->config->selector);
            }
            work->count = 0;
            for (i = 0; D_8015B7A0[i] != 0; i++) {
                work->count++;
            }
            work->visible = 1;
            work->state = 0;
            work->timer = 0;
            work->scale = 4096;
            func_8014F010((u8 *)&work->color, 1);
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            D_8009B261 = 1;
        }
    } else {
        Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        scale.vx = work->scale;
        scale.vy = work->scale;
        scale.vz = work->scale;
        if ((u16)func_8014D3AC((u8 *)&work->color) && work->visible != 0) {
            setPolyFT4(packet);
            packet->tpage = D_8015B748.pairs[13][0];
            packet->clut = D_8015B748.pairs[13][1];
            setRGB0(packet, work->color.r, work->color.g, work->color.b);
            if (work->scale > 1024) {
                setUV4(packet, 32, 0, 95, 0, 32, 63, 95, 63);
            } else {
                setUV4(packet, 32, 64, 95, 64, 32, 127, 95, 127);
            }
            func_8014F358(quad, 128);
            for (i = 0; i < work->count; i++) {
                setVector(&translation,
                    ((DisplayObject *)D_8015B7A0[i])->field_30.h.field_30,
                    ((DisplayObject *)D_8015B7A0[i])->field_30.h.field_32,
                    ((DisplayObject *)D_8015B7A0[i])->field_34.h.field_34);
                func_801513F4(&world, &saved, &translation, &rotation, &scale, 0);
                func_80151218(packet, quad, 32, 1);
                func_80151218(packet, quad, 32, 1);
            }
            if (work->scale > 1024) {
                work->scale -= 512;
            }
            if (work->state == 0) {
                work->state = func_80153F98((u8 *)&work->color,
                    work->config->color.r, work->config->color.g,
                    work->config->color.b, 15);
            }
            if (work->state == 2 && work->scale == 1024) {
                func_80153F28((u8 *)&work->color, 31);
            }
        }
        if (work->state == 1 && work->timer < 16) {
            work->timer++;
            if (!(work->timer % 4)) {
                if (work->visible == 1) {
                    work->visible = 0;
                } else {
                    work->visible = 1;
                }
            }
        } else if (work->timer >= 16) {
            work->state = 2;
        }
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->color)) {
            D_8009B261 = 1;
        }
    }
}
