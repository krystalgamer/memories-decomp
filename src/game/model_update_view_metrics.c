/* The first of the two steps of the camera-relative model transform.
 *
 * Model_UpdateViewMetrics recomputes the cached distance, yaw and pitch of
 * the current view. Model_BuildViewRelativeCoordinateUnit combines those
 * angles with a selected coordinate entry, retaining its slot-zero rotation
 * reads and lifetime-sensitive automatic parent link.
 *
 * The view is a GsRVIEW2 and the six offsets read here are its members:
 * 0x00, 0x04 and 0x08 are vpx, vpy and vpz, 0x0C, 0x10 and 0x14 are vrx,
 * vry and vrz. Three things say so -- this function already casts m to
 * GsRVIEW2 * to copy it whole into gModel_RView, camera_view.h declares that
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
 */
#include "../types.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "model.h"
#include "model_view_coordinate_helpers.h"

void Model_UpdateViewMetrics(GsRVIEW2 *m) {
    GsRVIEW2 *b;
    GsRVIEW2 *view;

    b = &gModel_RView;
    if (m == 0) {
        m = b;
    } else if (b != 0 && m != b) {
        gModel_RView = *m;
    }
    view = m;

    *(s16 *)&gModel_wViewDistance = SquareRoot0((*(s32 *)&view->vpx - *(s32 *)&view->vrx) * (*(s32 *)&view->vpx - *(s32 *)&view->vrx) + (*(s32 *)&view->vpz - *(s32 *)&view->vrz) * (*(s32 *)&view->vpz - *(s32 *)&view->vrz));

    gModel_wViewYaw = ratan2(*(s32 *)&view->vpz - *(s32 *)&view->vrz,
                        *(s32 *)&view->vpx - *(s32 *)&view->vrx);

    gModel_wViewPitch = ratan2(*(s32 *)&view->vpy - *(s32 *)&view->vry,
                        *(s16 *)&gModel_wViewDistance);

    *(s16 *)&gModel_wViewDistance = SquareRoot0((*(s32 *)&view->vpx - *(s32 *)&view->vrx) * (*(s32 *)&view->vpx - *(s32 *)&view->vrx) + (*(s32 *)&view->vpy - *(s32 *)&view->vry) * (*(s32 *)&view->vpy - *(s32 *)&view->vry) + (*(s32 *)&view->vpz - *(s32 *)&view->vrz) * (*(s32 *)&view->vpz - *(s32 *)&view->vrz));

    gModel_wViewYaw = (*(s16 *)&gModel_wViewYaw + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
    gModel_wViewPitch = (*(s16 *)&gModel_wViewPitch + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
}

static __inline__ void setup_rotation(
    SVECTOR *source,
    SVECTOR *copy,
    s32 turn,
    GsCOORDUNIT *unit
)
{
    source->vy = turn - gModel_wViewYaw;
    source->vz = gModel_wViewPitch;
    *copy = *source;

    unit->flg = 0;
    unit->rot.vx =
        turn - ((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vx;
    unit->rot.vy =
        turn - ((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vy;
    unit->rot.vz =
        turn - (u16)((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vz;
    RotMatrixZXY(&unit->rot, &unit->matrix);
}

static __inline__ void build_work_matrix(SVECTOR *rotation, MATRIX *work)
{
    RotMatrix_gte(rotation, work);
}

void Model_BuildViewRelativeCoordinateUnit(s32 index, s32 arg1, SVECTOR *arg2, GsCOORDUNIT *arg3)
{
    GsCOORDUNIT unit;
    SVECTOR ang;
    MATRIX ls;
    SVECTOR sv88;
    SVECTOR sv90;
    MATRIX work;
    u8 scratch[8];
    ModelSlot *slot;

    slot = &gModel_aSlots[index];
    if (slot->entry_count < arg1) {
        arg1 = slot->root_coordinate_index;
    }

    GsGetLwUnit(
        (GsCOORDUNIT *)(slot->entries + arg1 * MODEL_SLOT_DATA_ENTRY_SIZE),
        &ls
    );
    GsSetLsMatrix(&ls);

    RotTransSV(arg2, &ang, (PSXLONG *)scratch);

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
    memset(&sv90, 0, sizeof(sv90));

    setup_rotation(&sv90, &sv88, MODEL_ANGLE_FULL_TURN, arg3);

    build_work_matrix(&sv88, &work);
    MulMatrix(&arg3->matrix, &work);

    arg3->matrix.t[2] = 0;
    arg3->matrix.t[1] = 0;
    arg3->matrix.t[0] = 0;
    arg3->super = &unit;
}
