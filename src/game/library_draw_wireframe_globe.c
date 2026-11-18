/* Separate packet-marker lifetimes preserve retail's two setup registers. */
#include "../types.h"
#include "scratchpad.h"
#include "graphics_packet_submission.h"
#include "model_slot_control.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/inline_c.h"
#include "../psyq/libgs.h"
#include "model_slot_support.h"
#include "../game/model.h"
#include "gpu_packets.h"
#include "ordering_tables.h"
#include "library.h"
#include "library_draw_wireframe_globe.h"

/* Regional value: the geometry offset y (0x60 -> 0x68). The European build
 * (src/game/european/) defines its own. */
#ifndef GLOBE_GEOM_OFFSET_Y
#define GLOBE_GEOM_OFFSET_Y 0x60
#endif

void Library_DrawWireframeGlobe(void)
{
    GlobeLinePacket *prim;
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
    SetGeomOffset(0xD0, GLOBE_GEOM_OFFSET_Y);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);
    vec = (SVECTOR *)SCRATCH_ADDR(0x38);
    ctl = (long *)SCRATCH_ADDR(0x60);
    par = (SVECTOR *)SCRATCH_ADDR(0x200);
    ot = gGraphics_apOrderingTables[3];
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);

    {
        u32 marker = 0x55555555;
        vec1 = (SVECTOR *)SCRATCH_ADDR(0x40);
        vec2 = (SVECTOR *)SCRATCH_ADDR(0x48);
        vec3 = (SVECTOR *)SCRATCH_ADDR(0x50);
        prim = (GlobeLinePacket *)SCRATCH_ADDR(0);

        /* Retail materializes this fixed view buffer with literal lui/ori. */
        *par = *(SVECTOR *)0x80181000;

        prim->rgb0.word = 0;
        setlen(prim, 7);
        setcode(prim, 0x58);
        prim->pad = marker;
        prim->rgb2.c.code = 0;
        /* vec3->vy, vec2->vy and vec1->vy, stored through their absolute
         * addresses; a member store through the SVECTOR pointers reorders
         * the whole setup. */
        *(s16 *)SCRATCH_ADDR(0x52) = 0;
        *(s16 *)SCRATCH_ADDR(0x4A) = 0;
        *(s16 *)SCRATCH_ADDR(0x42) = 0;
        vec->vy = 0;
    }

    do {
        t = (0x400 - angle) * 0xFF / 0x400;
        prim->rgb1.c.b = t;
        prim->rgb1.c.g = t;
        prim->rgb1.c.r = t;
        prim->rgb2.word = prim->rgb1.word;
        ctl[2] = gLibrary_State.motion.globe_radius * rcos(angle) / 4096;
        if (ctl[2] == 0) {
            break;
        }
        ctl[3] = gLibrary_State.motion.globe_radius * rsin(angle) / 4096;
        vec->vz = vec1->vz = vec2->vz = vec3->vz = (u16)ctl[3] + (u16)par->vz;
        Library_DrawGlobeXSpan(prim, ot, vec, ctl, par->vx, ctl[2]);
        Library_DrawGlobeXSpan(prim, ot, vec, ctl, par->vx, -ctl[2]);
        if (angle != 0) {
            vec->vz = vec1->vz = vec2->vz = vec3->vz = (u16)par->vz - (u16)ctl[3];
            Library_DrawGlobeXSpan(prim, ot, vec, ctl, par->vx, ctl[2]);
            Library_DrawGlobeXSpan(prim, ot, vec, ctl, par->vx, -ctl[2]);
        }
        vec[0].vx = vec[1].vx = vec[2].vx = vec[3].vx = (u16)par->vx + (u16)ctl[3];
        Library_DrawGlobeZSpan(prim, ot, vec, ctl, par->vz, ctl[2]);
        Library_DrawGlobeZSpan(prim, ot, vec, ctl, par->vz, -ctl[2]);
        if (angle != 0) {
            vec[0].vx = vec[1].vx = vec[2].vx = vec[3].vx = (u16)par->vx - (u16)ctl[3];
            Library_DrawGlobeZSpan(prim, ot, vec, ctl, par->vz, ctl[2]);
            Library_DrawGlobeZSpan(prim, ot, vec, ctl, par->vz, -ctl[2]);
        }
        angle += 0x80;
    } while (angle < 0x400);

    {
        u32 marker;
        prim->rgb0.word = 0;
        prim->rgb1.word = 0xFFFFFF;
        prim->rgb2.word = 0;
        marker = 0x55555555;
        angle = 0;
        setlen(prim, 7);
        setcode(prim, 0x58);
        prim->pad = marker;
        prim->rgb2.c.code = 0;
        vec[0].vy = 0;
        vec[1].vy = 0;
        do {
            step = gLibrary_State.motion.globe_radius + 0x80;
            vec[0].vx = (u16)par->vx + step * rcos(angle) / 4096;
            next_angle = angle + 0x80;
            vec[1].vx = (u16)par->vx + step * rcos(next_angle) / 4096;
            vec[0].vz = (u16)par->vz + step * rsin(angle) / 4096;
            vec[1].vz = (u16)par->vz + step * rsin(next_angle) / 4096;
            step = step / 40;
            gte_ldv0(&vec[0]);
            gte_rtps();
            gte_stsxy(&prim->xy0.sxy);
            gte_ldv0(&vec[1]);
            gte_rtps();
            gte_stsxy(&prim->xy2.sxy);
            i = 0;
            /* Retail reads this y0 unsigned (lhu). */
            y = (u16)prim->xy0.h.y - step;
            prim->xy1.h.x = prim->xy0.h.x;
            prim->xy2.h.y = y;
            prim->xy1.h.y = y;
            do {
                Graphics_SubmitDrawModePacket(prim, ot, 1, 1);
                prim->xy0.h.y -= step;
                if (prim->xy0.h.y <= 0) {
                    break;
                }
                i++;
                prim->xy1.h.y -= step;
                prim->xy2.h.y -= step;
            } while (i < 7);
            angle += 0x80;
        } while (angle < 0x1000);

    }

    Model_DrawSlot(0);
    Model_UpdateSlotControl(0);
    Model_UpdateSlotAnimation(0);
}
