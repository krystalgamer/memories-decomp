/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers and 2 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/display_object_projection.c.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgte_abi_variants.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "../game/display_object.h"
#include "../game/display_object_projection.h"

/* RotAverageNclip3_nom at 0x80089CF0. libgte.h declares it
   `void RotAverageNclip3_nom(SVECTOR *, SVECTOR *, SVECTOR *)`; retail calls
   it with a fourth pointer and reads the result, so the address-qualified ABI
   alias preserves the retail call without changing the canonical declaration
   (notes/psyq.md). */

s32 func_80041F90(DisplayObject *obj, s32 arg1, s32 arg2, struct ProjectionOut *out) {
    MATRIX *mtx = (MATRIX *)0x1F8002D0;
    SVECTOR *v308 = (SVECTOR *)0x1F800308;
    s32 otz;

    out->f0 = arg1;
    out->f2 = arg2;
    out->f4 = obj->field_65;

    SetGeomOffset(arg1, arg2);
    SetGeomScreen(0x12C);

    v308->vx = (s16)(obj->field_20.b.field_20 * 0x10);
    v308->vy = (s16)(obj->field_20.b.field_21 * 0x10);
    v308->vz = (s16)(obj->field_20.b.field_22 * 0x10);
    RotMatrixZYX_gte(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = 0x12C;

    if (!(obj->attribute & GsROTOFF)) {
        ((VECTOR *)v308)->vx = obj->field_44.h.field_44;
        ((VECTOR *)v308)->vy = obj->field_44.h.field_46;
        ((VECTOR *)v308)->vz = ONE;
        ScaleMatrix(mtx, (VECTOR *)0x1F800308);
    }

    GsSetLsMatrix(mtx);

    {
        register SVECTOR *v318 asm("a3") = (SVECTOR *)0x1F800318;
        SVECTOR *v310 = (SVECTOR *)0x1F800310;
        s32 *otzp;

        v318->vx = 0;
        v308->vx = 0;
        v308[1].vx = 0x200;
        v310->vy = 0;
        v308->vy = 0;
        v308[2].vy = 0x200;
        v318->vz = 0;
        v310->vz = 0;
        v308->vz = 0;
        RotAverageNclip3_nom_80089CF0(v308, v310, v318, v318);

        otzp = &otz;
        gte_stopz(otzp);

        {
            s32 cb = obj->field_10;
            if (cb != 0) {
                if (cb < 0) {
                    ((void (*)(DisplayObject *, s32))cb)(obj, otz);
                }
                if (otz >= 0) {
                    return otz;
                }

                mtx->m[0][0] = -mtx->m[0][0];
                mtx->m[1][0] = -mtx->m[1][0];
                mtx->m[2][0] = -mtx->m[2][0];
                mtx->m[0][2] = -mtx->m[0][2];
                mtx->m[1][2] = -mtx->m[1][2];
                mtx->m[2][2] = -mtx->m[2][2];
                SetRotMatrix(mtx);

                RotAverageNclip3_nom_80089CF0(
                    v308, v310, (SVECTOR *)0x1F800318, v318);

                otzp = &otz;
                gte_stopz(otzp);
            }
        }
    }

    return otz;
}
