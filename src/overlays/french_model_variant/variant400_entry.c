#include "../../types.h"
#include "variant400_entry.h"

#ifndef MODEL_VARIANT400_CLUT_X
#define MODEL_VARIANT400_CLUT_X 640
#endif

s32 func_8013B004(SVECTOR *point, s32 command)
{
    Variant400EntryState *work = (Variant400EntryState *)point;
    MATRIX unused_matrix0;
    MATRIX unused_matrix1;
    SVECTOR position, target;
    VECTOR squared;
    Variant400EntryProjection projection;
    Variant400EntryNode *node = work->nodes;
    Variant400EntrySheet *sheet = work->sheets;
    Variant400EntryColumn *column = work->columns;
    POLY_GT4 *textured = work->textured;
    POLY_FT4 *flat = work->flat_textured;
    POLY_G3 *triangle = &work->triangle;
    POLY_G4 *quad = &work->quad;
    s32 result;
    s32 sheet_index, sheet_point;
    s32 i, j, angle;
    u16 tpage, clut;
    s32 flat_texture, flat_page;
    s32 packed;
    GsRVIEW2 *view;
    s16 screen_x;
    s32 screen_y;
    u32 frame;
    s32 dy;

    if (command >= 0) {
        work->slot = Model_GetActiveSlotIndex();
        work->config = (Variant400EntryConfig *)(D_8013D748 + 0xFC) + command;
        work->part = Model_GetSlotDataEntry(work->slot, work->config->part);
        result = 6;
        if (work->slot == 0) {
            setVector(&work->target, 0, -300, -450);
        } else {
            setVector(&work->target, 0, -300, 450);
        }
        GsGetLwUnit(work->part, &work->matrix);
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        tpage = GetTPage(1, 1, 896, 0);
        clut = GetClut(MODEL_VARIANT400_CLUT_X, 244);
        GetTPage(1, 1, 896, 0);
        GetClut(MODEL_VARIANT400_CLUT_X, 244);
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D748 + 0xC4));
        flat_texture = packed;
        flat_page = packed >> 16;
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D748 + 0x8C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D748 + 0x1C));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D748 + 0x38));
        packed = ModelTexture_PackPageClutCopy(work->slot, 1, (GsIMAGE *)(D_8013D748 + 0x54));
        SetPolyG3(triangle);
        SetSemiTrans(triangle, 1);
        SetPolyG4(quad);
        SetSemiTrans(quad, 1);
        SetPolyGT4(textured);
        textured->tpage = tpage;
        setUV4(textured, 126, 0, 127, 0, 126, 63, 127, 63);
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        textured++;
        SetPolyGT4(textured);
        textured->tpage = tpage;
        setUV4(textured, 64, 0, 127, 0, 64, 63, 127, 63);
        textured->clut = clut;
        SetSemiTrans(textured, 1);
        SetShadeTex(textured, 0);
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 0, 128, 0, 175, 48, 128, 48, 175);
        flat->clut = flat_texture;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        flat++;
        SetPolyFT4(flat);
        flat->tpage = flat_page;
        setUV4(flat, 0, 176, 0, 223, 48, 176, 48, 223);
        flat->clut = flat_texture;
        SetSemiTrans(flat, 1);
        SetShadeTex(flat, 1);
        for (i = 0; i < 5; i++, node++) {
            setVector(&node->scale, 4096, 4096, 4096);
            node->inner.r = 128;
            node->inner.g = 128;
            node->inner.b = 128;
            node->outer.r = 0;
            node->outer.g = 16;
            node->outer.b = 128;
            node->field_238 = 0;
            node->field_23A = 0;
            node->field_23C = 0;
            if (i == 0) {
                angle = 1024;
            } else if (i % 2 == 1) {
                angle = 1024 + (i + 1) * 2048 / 6;
            } else {
                angle = 1024 - i * 2048 / 6;
            }
            node->position.vx = work->matrix.t[0] - ((rcos(angle) * 384) >> 12);
            node->position.vy = work->matrix.t[1] - ((rsin(angle) * 384) >> 12);
            node->position.vz = work->matrix.t[2];
        }
        for (sheet_index = 0; sheet_index < 7; sheet_index++, sheet++) {
            for (sheet_point = 0; sheet_point < 4; sheet_point++) {
                if (sheet_point == 0) {
                    setVector(&sheet->points[sheet_point], -128, -128, 0);
                    setVector(&sheet->points[sheet_point + 4], 0, -128, 0);
                    setVector(&sheet->points[sheet_point + 8], -128, 0, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 1) {
                    setVector(&sheet->points[sheet_point], 128, -128, 0);
                    setVector(&sheet->points[sheet_point + 4], 128, 0, 0);
                    setVector(&sheet->points[sheet_point + 8], 0, -128, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 2) {
                    setVector(&sheet->points[sheet_point], 128, 128, 0);
                    setVector(&sheet->points[sheet_point + 4], 0, 128, 0);
                    setVector(&sheet->points[sheet_point + 8], 128, 0, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                } else if (sheet_point == 3) {
                    setVector(&sheet->points[sheet_point], -128, 128, 0);
                    setVector(&sheet->points[sheet_point + 4], -128, 0, 0);
                    setVector(&sheet->points[sheet_point + 8], 0, 128, 0);
                    setVector(&sheet->points[sheet_point + 12], 0, 0, 0);
                }
            }
            sheet->inner.r = 255;
            sheet->inner.g = 255;
            sheet->inner.b = 255;
            sheet->outer.r = 0;
            sheet->outer.g = 64;
            sheet->outer.b = 255;
            sheet->field_88 = 0;
            sheet->field_8C = 0;
            sheet->field_90 = 0;
        }
        for (i = 0; i < 2; i++, column++) {
            for (j = 0; j < 17; j++) {
                column->color[j].r = 128;
                column->color[j].g = 128;
                column->color[j].b = 128;
            }
            column->field_2A8 = 0;
        }
        work->field_2068 = 1024;
        work->field_206C = 0;
        work->field_2070 = 0;
        work->field_2074 = 0;
        work->field_2076 = 0;
        work->field_2088 = 0;
        work->field_208C = 0;
        work->field_2078 = 1024;
        work->field_207C = 0;
        work->field_2080 = 0;
        work->field_2084 = 0;
        work->frame_count = 0;
        work->frame = 0;
        work->animation_frame = 0;
        work->step = 0;
        work->duration = work->config->end - work->config->start;
        work->fade = 0;
        work->tint = 2048;
        work->phase = 0;
        work->command = command;
    } else {
        view = Model_GetCameraViewBuffer();
        work->view_delta.vx = view->vrx - view->vpx;
        work->view_delta.vy = view->vry - view->vpy;
        work->view_delta.vz = view->vrz - view->vpz;
        Square0(&work->view_delta, &squared);
        work->angles[0] = ratan2(work->view_delta.vy, SquareRoot0(squared.vx + squared.vz));
        work->angles[1] = -ratan2(work->view_delta.vz, work->view_delta.vx) - 1024;
        if (work->slot == 0) {
            Model_CopySlotPosition(1, (u16 *)&work->target);
        } else {
            Model_CopySlotPosition(0, (u16 *)&work->target);
        }
        if (work->frame <= work->config->end) {
            GsGetLwUnit(work->part, &work->matrix);
            node = work->nodes;
            for (i = 0; i < 5; i++, node++) {
                if (i == 0) {
                    angle = 1024;
                } else if (i % 2 == 1) {
                    angle = 1024 + (i + 1) * 2048 / 6;
                } else {
                    angle = 1024 - i * 2048 / 6;
                }
                node->position.vx = work->matrix.t[0] - ((rcos(angle) * 384) >> 12);
                node->position.vy = work->matrix.t[1] - ((rsin(angle) * 384) >> 12);
                node->position.vz = work->matrix.t[2];
            }
        }
        work->direction.vx = work->target.vx - work->matrix.t[0];
        work->direction.vy = work->target.vy - work->matrix.t[1];
        work->direction.vz = work->target.vz - work->matrix.t[2];
        GsSetLsMatrix(Model_GetLightSourceMatrix());
        setVector(&position, work->matrix.t[0], work->matrix.t[1], work->matrix.t[2]);
        RotTransPers(&position, (PSXLONG *)&projection.projected,
                     &projection.interpolation, &projection.flag);
        screen_x = projection.projected.vx;
        screen_y = projection.projected.vy;
        setVector(&target, work->target.vx, work->target.vy, work->target.vz);
        RotTransPers(&target, (PSXLONG *)&projection.target,
                     &projection.interpolation, &projection.flag);
        work->screen_delta.vx = projection.target.vx - screen_x;
        dy = projection.target.vy - screen_y;
        work->screen_delta.vy = dy;
        if (work->config->start <= work->frame) {
            func_8013BA88((u8 *)point);
            func_8013C838((u8 *)point);
            func_8013CDB8((u8 *)point);
        }
        work->frame_count++;
        frame = Model_GetSlotAnimationFrame(work->slot);
        if (frame != work->animation_frame) {
            work->frame += Model_GetFrameStep();
            work->step = Model_GetFrameStep();
            work->animation_frame = frame;
        }
    }
    Model_SetSlotAmbient(2, work->tint, work->tint, work->tint);
    if (work->phase < 6) {
        if (work->tint > 0) {
            work->tint -= 128;
            if (work->tint <= 0) {
                work->tint = 0;
            }
        }
    } else {
        work->tint = work->fade << 5;
    }
    if (work->phase < 5) {
        result = 0;
    } else if (work->phase < 9) {
        result = 4;
    } else if (work->phase == 9) {
        result = 1;
        work->phase = 10;
    } else if (work->phase == 10) {
        result = 0;
        work->fade += work->step;
        if (work->fade > 64) {
            work->fade = 64;
            work->phase = 11;
        }
    } else if (work->phase == 11) {
        result = 2;
    }
    return result;
}
