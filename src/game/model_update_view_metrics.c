/* The first of the two steps of the camera-relative model transform.
 *
 * Model_UpdateViewMetrics recomputes the cached distance, yaw and pitch of
 * the current view. The second step, func_800580D4, now a candidate in
 * src/candidates/func_800580D4.c, is the only thing that consumes the two
 * angles it produces, turning them plus one model slot's coordinate unit
 * into the transform its caller asked for.
 *
 * The view is a GsRVIEW2 and the six offsets read here are its members:
 * 0x00, 0x04 and 0x08 are vpx, vpy and vpz, 0x0C, 0x10 and 0x14 are vrx,
 * vry and vrz. Three things say so -- this function already casts m to
 * GsRVIEW2 * to copy it whole into D_800F56F0, camera_view.h declares that
 * global as GsRVIEW2, and the arithmetic reads as the standard camera
 * derivation: the horizontal distance and the yaw both take vp - vr in x
 * and z, and the pitch takes the y pair against that distance.
 *
 * They are read as *(s32 *)&view->vpx rather than view->vpx, and that is
 * measured. The plain member reads keep the instruction count at 226 but
 * rename registers across the whole function. Taking each member's address
 * keeps every read a scalar reference, as the old (m + offset) casts were,
 * and builds byte-identically. It is the same device
 * the two DisplayObject_Render*GouraudQuadList functions use; they too store to
 * fixed addresses between the reads.
 *
 * The parameter itself stays u8 *, for a separate reason: GsRVIEW2 is an
 * anonymous typedef, so this unit's header cannot forward declare it the
 * way model.h does for struct _GsCOORDUNIT, and pulling libgs.h in needs
 * the libgte and libgpu chain that seven of this header's nine includers
 * do not have. */
#include "../types.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "model.h"

void Model_UpdateViewMetrics(u8 *m) {
    u8 *b;
    GsRVIEW2 *view;

    b = (u8 *)&D_800F56F0;
    if (m == 0) {
        m = b;
    } else if (b != 0 && m != b) {
        D_800F56F0 = *(GsRVIEW2 *)m;
    }
    view = (GsRVIEW2 *)m;

    *(s16 *)&D_8009B478 = SquareRoot0((*(s32 *)&view->vpx - *(s32 *)&view->vrx) * (*(s32 *)&view->vpx - *(s32 *)&view->vrx) + (*(s32 *)&view->vpz - *(s32 *)&view->vrz) * (*(s32 *)&view->vpz - *(s32 *)&view->vrz));

    D_8009B47A = ratan2(*(s32 *)&view->vpz - *(s32 *)&view->vrz,
                        *(s32 *)&view->vpx - *(s32 *)&view->vrx);

    D_8009B47C = ratan2(*(s32 *)&view->vpy - *(s32 *)&view->vry,
                        *(s16 *)&D_8009B478);

    *(s16 *)&D_8009B478 = SquareRoot0((*(s32 *)&view->vpx - *(s32 *)&view->vrx) * (*(s32 *)&view->vpx - *(s32 *)&view->vrx) + (*(s32 *)&view->vpy - *(s32 *)&view->vry) * (*(s32 *)&view->vpy - *(s32 *)&view->vry) + (*(s32 *)&view->vpz - *(s32 *)&view->vrz) * (*(s32 *)&view->vpz - *(s32 *)&view->vrz));

    D_8009B47A = (*(s16 *)&D_8009B47A + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
    D_8009B47C = (*(s16 *)&D_8009B47C + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
}

static __inline__ void setup_rotation(
    SVECTOR *source,
    SVECTOR *copy,
    s32 turn,
    GsCOORDUNIT *unit
)
{
    source->vy = turn - D_8009B47A;
    source->vz = D_8009B47C;
    *copy = *source;

    unit->flg = 0;
    unit->rot.vx =
        turn - ((volatile ModelSlot *)D_800F2C40)->field_D18->rot.vx;
    unit->rot.vy =
        turn - ((volatile ModelSlot *)D_800F2C40)->field_D18->rot.vy;
    unit->rot.vz =
        turn - (u16)((volatile ModelSlot *)D_800F2C40)->field_D18->rot.vz;
    RotMatrixZXY(&unit->rot, &unit->matrix);
}

static __inline__ void build_work_matrix(SVECTOR *rotation, MATRIX *work)
{
    RotMatrix_gte(rotation, work);
}

void func_800580D4(s32 index, s32 arg1, u8 *arg2, GsCOORDUNIT *arg3)
{
    GsCOORDUNIT unit;
    SVECTOR ang;
    MATRIX ls;
    SVECTOR sv88;
    SVECTOR sv90;
    MATRIX work;
    u8 scratch[8];
    ModelSlot *slot;

    slot = &D_800F2C40[index];
    if (slot->entry_count < arg1) {
        arg1 = slot->field_E18;
    }

    GsGetLwUnit(
        (GsCOORDUNIT *)(slot->entries + arg1 * MODEL_SLOT_DATA_ENTRY_SIZE),
        &ls
    );
    GsSetLsMatrix(&ls);

    RotTransSV((SVECTOR *)arg2, &ang, (long *)scratch);

    unit.rot.vz = 0;
    unit.rot.vy = 0;
    unit.rot.vx = 0;
    RotMatrix_gte(&unit.rot, &unit.matrix);

    unit.flg = 1;
    unit.matrix.t[0] = ang.vx;
    unit.matrix.t[1] = ang.vy;
    unit.matrix.t[2] = ang.vz;
    unit.workm = unit.matrix;
    unit.super = NULL;
    memset(&sv90, 0, 8);

    setup_rotation(&sv90, &sv88, MODEL_ANGLE_FULL_TURN, arg3);

    build_work_matrix(&sv88, &work);
    MulMatrix(&arg3->matrix, &work);

    arg3->matrix.t[2] = 0;
    arg3->matrix.t[1] = 0;
    arg3->matrix.t[0] = 0;
    arg3->super = &unit;
}
