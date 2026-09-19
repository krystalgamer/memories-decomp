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
#include "../game/func_800540B4.h"
#include "func_800556E8.h"
#include "gpu_packets.h"
#include "ordering_tables.h"
#include "func_8002A3CC.h"
#include "duel_projection_axes.h"
#include "func_80029934.h"

void func_80029934(void)
{
    LINE_G3 *prim;
    SVECTOR *vec;
    SVECTOR *vec1;
    SVECTOR *vec2;
    SVECTOR *vec3;
    long *ctl;
    SVECTOR *par;
    s32 angle;
    GsOT *ot;
    s32 t;
    s32 i;
    s32 step;
    s32 y;
    s32 next_angle;

    angle = 0;
    SetGeomOffset(0xD0, 0x60);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);
    vec = (SVECTOR *)0x1F800038;
    ctl = (long *)0x1F800060;
    par = (SVECTOR *)0x1F800200;
    ot = D_800E9D90[3];
    GsSetLsMatrix(&D_800FE148);

    {
        u32 marker = 0x55555555;
        vec1 = (SVECTOR *)0x1F800040;
        vec2 = (SVECTOR *)0x1F800048;
        vec3 = (SVECTOR *)0x1F800050;
        prim = (LINE_G3 *)0x1F800000;

        /* Retail materializes this fixed view buffer with literal lui/ori. */
        *par = *(SVECTOR *)0x80181000;

        *(s32 *)&prim->r0 = 0;
        setlen(prim, 7);
        setcode(prim, 0x58);
        prim->pad = marker;
        prim->p2 = 0;
        /* vec3->vy, vec2->vy and vec1->vy, stored through their absolute
         * addresses; a member store through the SVECTOR pointers reorders
         * the whole setup. */
        *(s16 *)0x1F800052 = 0;
        *(s16 *)0x1F80004A = 0;
        *(s16 *)0x1F800042 = 0;
        vec->vy = 0;
    }

    do {
        t = (0x400 - angle) * 0xFF / 0x400;
        prim->b1 = t;
        prim->g1 = t;
        prim->r1 = t;
        *(s32 *)&prim->r2 = *(s32 *)&prim->r1;
        ctl[2] = D_800EA1E8.globe_radius * rcos(angle) / 4096;
        if (ctl[2] == 0) {
            break;
        }
        ctl[3] = D_800EA1E8.globe_radius * rsin(angle) / 4096;
        vec->vz = vec1->vz = vec2->vz = vec3->vz = (u16)ctl[3] + (u16)par->vz;
        func_80029684(prim, ot, vec, ctl, par->vx, ctl[2]);
        func_80029684(prim, ot, vec, ctl, par->vx, -ctl[2]);
        if (angle != 0) {
            vec->vz = vec1->vz = vec2->vz = vec3->vz = (u16)par->vz - (u16)ctl[3];
            func_80029684(prim, ot, vec, ctl, par->vx, ctl[2]);
            func_80029684(prim, ot, vec, ctl, par->vx, -ctl[2]);
        }
        vec[0].vx = vec[1].vx = vec[2].vx = vec[3].vx = (u16)par->vx + (u16)ctl[3];
        func_800297DC(prim, ot, vec, ctl, par->vz, ctl[2]);
        func_800297DC(prim, ot, vec, ctl, par->vz, -ctl[2]);
        if (angle != 0) {
            vec[0].vx = vec[1].vx = vec[2].vx = vec[3].vx = (u16)par->vx - (u16)ctl[3];
            func_800297DC(prim, ot, vec, ctl, par->vz, ctl[2]);
            func_800297DC(prim, ot, vec, ctl, par->vz, -ctl[2]);
        }
        angle += 0x80;
    } while (angle < 0x400);

    {
        u32 marker;
        *(s32 *)&prim->r0 = 0;
        *(s32 *)&prim->r1 = 0xFFFFFF;
        *(s32 *)&prim->r2 = 0;
        marker = 0x55555555;
        angle = 0;
        setlen(prim, 7);
        setcode(prim, 0x58);
        prim->pad = marker;
        prim->p2 = 0;
        vec[0].vy = 0;
        vec[1].vy = 0;
        do {
            step = D_800EA1E8.globe_radius + 0x80;
            vec[0].vx = (u16)par->vx + step * rcos(angle) / 4096;
            next_angle = angle + 0x80;
            vec[1].vx = (u16)par->vx + step * rcos(next_angle) / 4096;
            vec[0].vz = (u16)par->vz + step * rsin(angle) / 4096;
            vec[1].vz = (u16)par->vz + step * rsin(next_angle) / 4096;
            step = step / 40;
            gte_ldv0(&vec[0]);
            gte_rtps();
            gte_stsxy(&prim->x0);
            gte_ldv0(&vec[1]);
            gte_rtps();
            gte_stsxy(&prim->x2);
            i = 0;
            /* Retail reads this y0 unsigned (lhu). */
            y = (u16)prim->y0 - step;
            prim->x1 = prim->x0;
            prim->y2 = y;
            prim->y1 = y;
            do {
                func_8005B260((u32 *)prim, ot, 1, 1);
                prim->y0 -= step;
                if (prim->y0 <= 0) {
                    break;
                }
                i++;
                prim->y1 -= step;
                prim->y2 -= step;
            } while (i < 7);
            angle += 0x80;
        } while (angle < 0x1000);

    }

    func_800540B4(0);
    func_800559D4(0);
    func_800556E8(0);
}
