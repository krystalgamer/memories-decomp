#include "../types.h"
#include "screen_projection.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"

GsRVIEW2 *Model_GetCameraViewBuffer(void)
{
    return &gModel_RView;
}

ModelViewMetrics *Model_GetViewMetricsBuffer(void)
{
    return (ModelViewMetrics *)&gModel_wViewDistance;
}

GsCOORDUNIT *Model_GetCameraCoordinateUnit(void)
{
    return &D_800F56A0;
}

void *Model_GetLightSourceMatrix(void)
{
    return &D_800FE148;
}
