#include "../types.h"
#include "model_lock_orbit_direction.h"
#include "model_transfer_flags.h"
#include "model_graphics_state.h"

void Model_LockOrbitDirection(int value){int one=1;if(func_8005F174()!=one||func_8005F18C()!=one){if(value>=0)gModel_bOrbitDirection=one;else gModel_bOrbitDirection=-1;gModel_bOrbitDirectionTimer=255;}}
