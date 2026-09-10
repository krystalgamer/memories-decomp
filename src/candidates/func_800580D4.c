/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/model_update_view_metrics.c.
 */
/* The camera-relative model transform, in two steps.
 *
 * Model_UpdateViewMetrics recomputes the cached distance, yaw and pitch of
 * the current view; func_800580D4 is the only thing that consumes the two
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
 * display_object_list_renderers.c needed; both files store to fixed
 * addresses between the reads.
 *
 * The parameter itself stays u8 *, for a separate reason: GsRVIEW2 is an
 * anonymous typedef, so this unit's header cannot forward declare it the
 * way model.h does for struct _GsCOORDUNIT, and pulling libgs.h in needs
 * the libgte and libgpu chain that seven of this header's nine includers
 * do not have. */
#include "../types.h"
#include "../game/camera_view.h"
#include "../game/model_update_view_metrics.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "../game/model.h"

/* Builds a coordinate unit for one model slot's entry and hands the caller's
 * GsCOORDUNIT (arg3) a matrix built from the camera angles, parented to it.
 *
 * arg3's rotation is read from the slot's own unit at field_D18 through a
 * volatile view of the slot table, which reloads the pointer for each angle
 * as retail does. The view has to be a struct reference: through a cast
 * pointer the load is a fixed-address scalar and floats above the flg store.
 * The third angle keeps its (u16) because it lands in an int, where the sign
 * would otherwise turn lhu into lh.
 *
 * The local `unit` is a whole GsCOORDUNIT: `matrix` and `workm` are the pair
 * GsGetLwUnit copies between, `rot` is the zeroed vector RotMatrix_gte reads,
 * and `super` is the parent link, left null because this unit is the root of
 * the chain arg3 is spliced onto at the end. */
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
    s32 z;
    register s32 turn asm("$6");
    /* Prevent GCC from carrying &work across the final pair of calls. */
    register u8 *stack_pointer asm("$sp");

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

    turn = MODEL_ANGLE_FULL_TURN;
    sv90.vy = turn - D_8009B47A;
    sv90.vz = D_8009B47C;
    sv88 = sv90;

    arg3->flg = 0;
    arg3->rot.vx = turn - ((volatile ModelSlot *)D_800F2C40)->field_D18->rot.vx;
    arg3->rot.vy = turn - ((volatile ModelSlot *)D_800F2C40)->field_D18->rot.vy;
    z = turn - (u16)((volatile ModelSlot *)D_800F2C40)->field_D18->rot.vz;
    arg3->rot.vz = z;
    RotMatrixZXY(&arg3->rot, &arg3->matrix);

    RotMatrix_gte(&sv88, &work);
    MulMatrix(&arg3->matrix, (MATRIX *)(stack_pointer + 0x98));

    arg3->matrix.t[2] = 0;
    arg3->matrix.t[1] = 0;
    arg3->matrix.t[0] = 0;
    arg3->super = &unit;
}
