#include "../types.h"

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"

/* Zero-initialized storage this unit owns at 0x800F56A0-0x800F5750, right
 * after gModel_aSlots, defined in address order ahead of the headers that
 * declare it (#2602); nothing in the image names the last 0x20 bytes. */
GsCOORDUNIT gModel_CameraCoordinateUnit;
GsRVIEW2 gModel_RView;
GsRVIEW2 gModel_SavedRefView;
static PADDING_VARIABLE(0x20);

#include "model.h"
#include "camera_view.h"
#include "model_distance_queries.h"

#define MODEL_SLOT_HALFWORDS(slots, index) \
    ((u16 *)((u8 *)(slots) + (index) * MODEL_SLOT_SIZE))

s32 Model_GetSlotDistanceFromCameraEye(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u16 *entry = MODEL_SLOT_HALFWORDS(gModel_aSlots[0].cached_position, index);

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
    u16 *entry = MODEL_SLOT_HALFWORDS(gModel_aSlots[0].cached_position, index);

    memset(&difference, 0, sizeof(difference));
    difference.vx = gModel_RView.vrx - entry[0];
    difference.vy = gModel_RView.vry - entry[1];
    difference.vz = gModel_RView.vrz - entry[2];
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}
