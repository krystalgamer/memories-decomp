#include "../types.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "model.h"

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

    RotMatrix_gte(&sv88, &work);
    /* GCC defines alloca(0) as sp + 0x10 here; this rematerializes &work. */
    MulMatrix(
        &arg3->matrix,
        (MATRIX *)((u8 *)__builtin_alloca(0) + 0x88)
    );

    arg3->matrix.t[2] = 0;
    arg3->matrix.t[1] = 0;
    arg3->matrix.t[0] = 0;
    arg3->super = &unit;
}
