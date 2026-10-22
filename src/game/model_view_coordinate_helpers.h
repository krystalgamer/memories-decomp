#ifndef GAME_MODEL_VIEW_COORDINATE_HELPERS_H
#define GAME_MODEL_VIEW_COORDINATE_HELPERS_H

#include "model_update_view_metrics.h"

void Model_BuildViewRelativeCoordinateUnit(
    s32 index, s32 entry, SVECTOR *point, struct _GsCOORDUNIT *unit);
void Model_GetSlot0OffsetWorldPosition(s16 value, s16 *out);

#endif
