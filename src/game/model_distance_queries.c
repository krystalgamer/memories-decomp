#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800F3A10-0x800F5750, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 D_800F3A10[0x1D40];

#include "../psyq/libgte.h"
#include "../psyq/memory.h"

#include "model.h"
#include "camera_view.h"
#include "model_distance_queries.h"

#define MODEL_SLOT_HALFWORDS(slots, index) \
    ((u16 *)((u8 *)(slots) + (index) * MODEL_SLOT_SIZE))

s32 Model_GetSlotDistanceFromCameraEye(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u16 *entry = MODEL_SLOT_HALFWORDS(gModel_awSlot0Position, index);

    memset(&difference, 0, sizeof(difference));
    difference.vx = gModel_RView.vpx - entry[0];
    difference.vy = gModel_RView.vpy - entry[1];
    difference.vz = gModel_RView.vpz - entry[2];
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}

s32 Model_GetSlotDistanceFromCameraTarget(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u16 *entry = MODEL_SLOT_HALFWORDS(gModel_awSlot0Position, index);

    memset(&difference, 0, sizeof(difference));
    difference.vx = gModel_RView.vrx - entry[0];
    difference.vy = gModel_RView.vry - entry[1];
    difference.vz = gModel_RView.vrz - entry[2];
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}
