#ifndef MEMORIES_DECOMP_SD_CALC_SPATIAL_VOLUME_PAN_H
#define MEMORIES_DECOMP_SD_CALC_SPATIAL_VOLUME_PAN_H

#include "../types.h"

/* Distance-to-camera fade and pan for one sound source at (source_x, source_z). The
 * square root of the distance from the listener record at gModel_RView, minus
 * 700, sets the volume byte in *volume (0xFF down to 0x10). The signed
 * projection onto the listener direction, divided by 16 and clamped to
 * +-0x7F, sets the pan in *pan. */
void SD_CalcSpatialVolumePan(
    s32 source_x, s32 source_z, u8 *volume, u8 *pan
);

#endif
