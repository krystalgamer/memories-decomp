#include "../types.h"
#include "screen_projection.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"

GsRVIEW2 *Model_GetCameraViewBuffer(void)
{
    return &D_800F56F0;
}

void *Model_GetViewMetricsBuffer(void)
{
    return &D_8009B478;
}

GsCOORDUNIT *Model_GetCameraCoordinateUnit(void)
{
    return &D_800F56A0;
}

void *Model_GetLightSourceMatrix(void)
{
    return &D_800FE148;
}
