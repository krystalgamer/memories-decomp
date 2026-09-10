/* The camera-relative model transform, in two steps.
 *
 * Model_UpdateViewMetrics recomputes the cached distance, yaw and pitch of
 * the current view; func_800580D4 is the only thing that consumes the two
 * angles it produces, turning them plus one model slot's coordinate unit
 * into the transform its caller asked for. */
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

    b = (u8 *)&D_800F56F0;
    if (m == 0) {
        m = b;
    } else if (b != 0 && m != b) {
        D_800F56F0 = *(GsRVIEW2 *)m;
    }

    *(s16 *)&D_8009B478 = SquareRoot0((*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) * (*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) + (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)) * (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)));

    D_8009B47A = ratan2(*(s32 *)(m + 8) - *(s32 *)(m + 0x14),
                        *(s32 *)(m + 0) - *(s32 *)(m + 0xC));

    D_8009B47C = ratan2(*(s32 *)(m + 4) - *(s32 *)(m + 0x10),
                        *(s16 *)&D_8009B478);

    *(s16 *)&D_8009B478 = SquareRoot0((*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) * (*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) + (*(s32 *)(m + 4) - *(s32 *)(m + 0x10)) * (*(s32 *)(m + 4) - *(s32 *)(m + 0x10)) + (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)) * (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)));

    D_8009B47A = (*(s16 *)&D_8009B47A + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
    D_8009B47C = (*(s16 *)&D_8009B47C + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
}

/* Builds a coordinate unit for one model slot's entry and hands the caller's
 * record (arg3) a matrix built from the camera angles, parented to it.
 *
 * The local `unit` is a whole GsCOORDUNIT: `matrix` and `workm` are the pair
 * GsGetLwUnit copies between, `rot` is the zeroed vector RotMatrix_gte reads,
 * and `super` is the parent link, left null because this unit is the root of
 * the chain arg3 is spliced onto at the end. */
void func_800580D4(s32 index, s32 arg1, u8 *arg2, u8 *arg3)
{
    GsCOORDUNIT unit;
    SVECTOR ang;
    MATRIX ls;
    SVECTOR sv88;
    SVECTOR sv90;
    MATRIX work;
    u8 scratch[8];
    u8 *p;
    s32 z;
    register s32 turn asm("$6");
    /* Prevent GCC from carrying &work across the final pair of calls. */
    register u8 *stack_pointer asm("$sp");

    p = (u8 *)D_800F2C40 + index * MODEL_SLOT_SIZE;
    if (p[0xE17] < arg1) {
        arg1 = p[0xE18];
    }

    GsGetLwUnit(
        (GsCOORDUNIT *)(*(u8 **)(p + 0xD14) + arg1 * MODEL_SLOT_DATA_ENTRY_SIZE),
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

    *(s32 *)arg3 = 0;
    *(s16 *)(arg3 + 0x44) = turn - *(u16 *)((*(u8 * volatile *)((u8 *)D_800F2C40 + 0xD18)) + 0x44);
    *(s16 *)(arg3 + 0x46) = turn - *(u16 *)((*(u8 * volatile *)((u8 *)D_800F2C40 + 0xD18)) + 0x46);
    z = turn - *(u16 *)((*(u8 * volatile *)((u8 *)D_800F2C40 + 0xD18)) + 0x48);
    *(s16 *)(arg3 + 0x48) = z;
    RotMatrixZXY((SVECTOR *)(arg3 + 0x44), (MATRIX *)(arg3 + 4));

    RotMatrix_gte(&sv88, &work);
    MulMatrix((MATRIX *)(arg3 + 4), (MATRIX *)(stack_pointer + 0x98));

    *(s32 *)(arg3 + 0x20) = 0;
    *(s32 *)(arg3 + 0x1C) = 0;
    *(s32 *)(arg3 + 0x18) = 0;
    *(GsCOORDUNIT **)(arg3 + 0x4C) = &unit;
}
