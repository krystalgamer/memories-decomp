#include "../types.h"
#include "model_lock_orbit_direction.h"
#include "model_transfer_flags.h"
#include "model_graphics_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gModel_bOrbitDirectionTimer;
s8 gModel_bOrbitDirection;

void Model_LockOrbitDirection(int value){int one=1;if(Model_GetCameraHoldState()!=one||Model_GetHandlerCallbackState()!=one){if(value>=0)gModel_bOrbitDirection=one;else gModel_bOrbitDirection=-1;gModel_bOrbitDirectionTimer=255;}}
