#include "../../types.h"

#include "model_variant.h"
#include "../french_model_variant/variant460_entry.h"

/* Draws a variable set of sheets: the count and how many follow the variant's
 * own slots come from the entry config's count, mode and kind. The first sheets
 * sit on the entry matrices and grow with the timing phases; the rest sit on
 * the entry targets and follow the state and length of the entry records. */
void ModelVariant443_DrawSheets(Variant460EntryState *entry)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant460EntryRecord *obj;
    GsOT *ot;
    s32 count;
    s32 first;
    Variant460EntryState *work;
    Variant460EntryConfig *rec;
    POLY_GT4 *poly;
    ModelVariantSheetSet *sheet;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 bias;
    s32 otz;

    sheet = entry->sheets;
    poly = entry->packets.textured;
    obj = (work = entry)->records;
    ot = Graphics_GetDefaultOrderingTable();
    rec = work->config;
    if (rec->mode == 0) {
        if (rec->kind == 0) {
            count = 2;
            first = 1;
        } else if (rec->kind == 2) {
            count = 3;
            first = 2;
        } else {
            count = rec->count + 1;
            first = rec->count;
        }
    } else if (rec->kind == 0) {
        first = 1;
        count = rec->count + 1;
    } else if (rec->kind == 2) {
        first = 2;
        count = rec->count + 2;
    } else {
        count = rec->count * 2;
        first = rec->count;
    }
    poly++;
    for (i = 0; i < count; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i < first) {
            m.t[0] = work->matrices[i].t[0];
            m.t[1] = work->matrices[i].t[1];
            m.t[2] = work->matrices[i].t[2];
        } else {
            m.t[0] = work->targets[i - first].vx;
            m.t[1] = work->targets[i - first].vy;
            m.t[2] = work->targets[i - first].vz;
        }
        scale.vx = sheet->size + bias;
        scale.vy = sheet->size + bias;
        scale.vz = sheet->size + bias;
        RotMatrix(&rot, &m);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ReadRotMatrix(&ls);
        RotMatrix(&rot, &ls);
        ScaleMatrix(&ls, &scale);
        SetRotMatrix(&ls);
        for (k = 0, v = sheet->v0; k < 4; k++, v++) {
            otz = RotTransPers4(v, &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            poly->r0 = sheet->inner[0];
            poly->g0 = sheet->inner[1];
            poly->b0 = sheet->inner[2];
            poly->r1 = sheet->inner[0];
            poly->g1 = sheet->inner[1];
            poly->b1 = sheet->inner[2];
            poly->r2 = sheet->inner[0];
            poly->g2 = sheet->inner[1];
            poly->b2 = sheet->inner[2];
            poly->r3 = sheet->outer[0];
            poly->g3 = sheet->outer[1];
            poly->b3 = sheet->outer[2];
            otz = otz * 7 / 10;
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (i < first) {
            if (work->tail.phase == 0) {
                if (sheet->size < 0x1000) {
                    sheet->size = ((work->clock.frame - work->config->sheet_start) << 12) /
                                  (work->config->ribbon_start - work->config->sheet_start);
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        work->tail.phase = 1;
                    }
                }
            } else if (work->tail.phase == 3 && sheet->size > 0) {
                if (work->clock.frame >= work->config->phase3) {
                    sheet->size = 0x1000 - ((work->clock.frame - work->config->phase3) << 12) /
                                  (work->config->phase4 - work->config->phase3);
                }
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (obj->state == 1) {
                if (sheet->shown == 0) {
                    sheet->size = 0x4000;
                    sheet->shown = 1;
                } else {
                    sheet->size = obj->length * 16;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        if (work->tail.phase < 4) {
                            sheet->shown = 0;
                        }
                    }
                }
            } else {
                sheet->size = 0;
            }
            obj++;
        }
    }
}
