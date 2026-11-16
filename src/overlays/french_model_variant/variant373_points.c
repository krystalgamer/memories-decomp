#include "../../types.h"
#include "variant373_entry.h"

void FrenchModelVariant373_DrawSparks(EntryState373 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light_matrix;
    GsCOORDINATE2 coordinate;
    /* Retail reserves 16 bytes before the two color locals. */
    u8 unknown_stack[16];
    CVECTOR dark;
    CVECTOR light;
    long screen;
    long interpolation;
    long flag;
    GsOT *ot;
    s32 i;
    EntryState373 *work;
    EntrySparkGroup373 *group;
    POLY_FT4 *quad;
    s32 j;
    s32 fade;
    s32 depth;

    work = context;
    group = work->sparks;
    quad = &work->spark_packet;
    ot = Graphics_GetDefaultOrderingTable();
    i = 0;
    ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx);
    do {
        if (group->scale > 0) {
            rsin(group->scale);
            if (group->scale < 5120) {
                light.r = 192;
                light.g = 192;
                light.b = 192;
                dark.r = 192;
                dark.g = 64;
            } else {
                fade = group->scale - 5120;
                light.r = 192 - fade * 192 / 1024;
                light.g = 192 - fade * 192 / 1024;
                light.b = 192 - fade * 192 / 1024;
                dark.r = 192 - fade * 192 / 1024;
                dark.g = 64 - fade / 16;
            }
            dark.b = 0;
            rotation.vx = group->rotation.vx;
            rotation.vy = group->rotation.vy;
            rotation.vz = group->rotation.vz;
            matrix.t[0] = work->aim.target.vx;
            matrix.t[1] = work->aim.target.vy;
            matrix.t[2] = work->aim.target.vz;
            scale.vx = group->scale;
            scale.vy = group->scale;
            scale.vz = group->scale;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light_matrix);
            GsSetLsMatrix(&light_matrix);
            for (j = 0; j < 16; j++) {
                depth = RotTransPers(&group->points[j], &screen, &interpolation, &flag);
                setRGB0(quad, light.r, light.g, light.b);
                quad->x0 = screen - 8;
                quad->y0 = (screen >> 16) - 8;
                quad->x1 = screen + 8;
                quad->y1 = (screen >> 16) - 8;
                quad->x2 = screen - 8;
                quad->y2 = (screen >> 16) + 8;
                quad->x3 = screen + 8;
                quad->y3 = (screen >> 16) + 8;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (group->scale < 6144) {
            group->scale += work->clock.step * 192;
            if (group->scale >= 6144) {
                group->scale -= 6144;
                group->rotation.vx += work->clock.step * 192;
                group->rotation.vz += work->clock.step * 400;
                if (work->phase >= 7) {
                    group->scale = 6144;
                }
            }
        }
        i++;
        group++;
    } while (i < 3);
}
