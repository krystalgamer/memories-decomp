/* Separate packet-marker lifetimes preserve retail's two setup registers. */
#include "../types.h"
#include "model_control.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/inline_c.h"
#include "../psyq/libgs.h"
#include "screen_projection.h"
#include "../unmatched.h"
#include "func_800556E8.h"
#include "gpu_packets.h"
#include "ordering_tables.h"
#include "library_runtime.h"
#include "duel_projection_axes.h"
#include "func_80029934.h"

void func_80029934(void)
{
    u8 *prim;
    s16 *vec;
    s16 *vec1;
    s16 *vec2;
    s16 *vec3;
    s32 *ctl;
    s16 *par;
    s32 angle;
    s32 arg;
    s32 t;
    s32 i;
    s32 step;
    s32 y;
    s32 next_angle;

    angle = 0;
    SetGeomOffset(0xD0, 0x60);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);
    vec = (s16 *)0x1F800038;
    ctl = (s32 *)0x1F800060;
    par = (s16 *)0x1F800200;
    arg = (s32)D_800E9D90[3];
    GsSetLsMatrix(&D_800FE148);

    {
        u32 marker = 0x55555555;
        vec1 = (s16 *)0x1F800040;
        vec2 = (s16 *)0x1F800048;
        vec3 = (s16 *)0x1F800050;
        prim = (u8 *)0x1F800000;

        /* Retail materializes this fixed view buffer with literal lui/ori. */
        *(Bytes8 *)par = *(Bytes8 *)0x80181000;

        *(s32 *)(prim + 4) = 0;
        setlen(prim, 7);
        setcode(prim, 0x58);
        ((LINE_G3 *)prim)->pad = marker;
        ((LINE_G3 *)prim)->p2 = 0;
        *(s16 *)0x1F800052 = 0;
        *(s16 *)0x1F80004A = 0;
        *(s16 *)0x1F800042 = 0;
        vec[1] = 0;
    }

    do {
        t = (0x400 - angle) * 0xFF / 0x400;
        prim[0xE] = t;
        prim[0xD] = t;
        prim[0xC] = t;
        *(s32 *)(prim + 0x14) = *(s32 *)(prim + 0xC);
        { s16 *t1 = (s16 *)D_800EA1E8; ctl[2] = t1[8] * rcos(angle) / 4096; }
        if (ctl[2] == 0) {
            break;
        }
        { s16 *t1 = (s16 *)D_800EA1E8; ctl[3] = t1[8] * rsin(angle) / 4096; }
        vec[2] = vec1[2] = vec2[2] = vec3[2] = (u16)ctl[3] + (u16)par[2];
        func_80029684((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[0], ctl[2]);
        func_80029684((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[0], -ctl[2]);
        if (angle != 0) {
            vec[2] = vec1[2] = vec2[2] = vec3[2] = (u16)par[2] - (u16)ctl[3];
            func_80029684((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[0], ctl[2]);
            func_80029684((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[0], -ctl[2]);
        }
        vec[0] = vec[4] = vec[8] = vec[12] = (u16)par[0] + (u16)ctl[3];
        func_800297DC((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[2], ctl[2]);
        func_800297DC((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[2], -ctl[2]);
        if (angle != 0) {
            vec[0] = vec[4] = vec[8] = vec[12] = (u16)par[0] - (u16)ctl[3];
            func_800297DC((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[2], ctl[2]);
            func_800297DC((LINE_G3 *)prim, (GsOT *)arg, (SVECTOR *)vec, (long *)ctl, par[2], -ctl[2]);
        }
        angle += 0x80;
    } while (angle < 0x400);

    {
        s16 *radius_state;
        u32 marker;
        *(s32 *)(prim + 4) = 0;
        *(s32 *)(prim + 0xC) = 0xFFFFFF;
        *(s32 *)(prim + 0x14) = 0;
        marker = 0x55555555;
        angle = 0;
        radius_state = (s16 *)D_800EA1E8;
        setlen(prim, 7);
        setcode(prim, 0x58);
        ((LINE_G3 *)prim)->pad = marker;
        ((LINE_G3 *)prim)->p2 = 0;
        vec[1] = 0;
        vec[5] = 0;
        do {
            step = radius_state[8] + 0x80;
            vec[0] = (u16)par[0] + step * rcos(angle) / 4096;
            next_angle = angle + 0x80;
            vec[4] = (u16)par[0] + step * rcos(next_angle) / 4096;
            vec[2] = (u16)par[2] + step * rsin(angle) / 4096;
            vec[6] = (u16)par[2] + step * rsin(next_angle) / 4096;
            step = step / 40;
            gte_ldv0(vec);
            gte_rtps();
            gte_stsxy(prim + 8);
            gte_ldv0(vec + 4);
            gte_rtps();
            gte_stsxy(prim + 0x18);
            i = 0;
            y = *(u16 *)(prim + 0xA) - step;
            *(s16 *)(prim + 0x10) = *(u16 *)(prim + 8);
            *(s16 *)(prim + 0x1A) = y;
            *(s16 *)(prim + 0x12) = y;
            do {
                func_8005B260((u32 *)prim, (GsOT *)arg, 1, 1);
                *(s16 *)(prim + 0xA) = *(u16 *)(prim + 0xA) - step;
                if ((s16)*(u16 *)(prim + 0xA) <= 0) {
                    break;
                }
                i++;
                *(s16 *)(prim + 0x12) = *(u16 *)(prim + 0x12) - step;
                *(s16 *)(prim + 0x1A) = *(u16 *)(prim + 0x1A) - step;
            } while (i < 7);
            angle += 0x80;
        } while (angle < 0x1000);

    }

    func_800540B4(0);
    func_800559D4(0);
    func_800556E8(0);
}
