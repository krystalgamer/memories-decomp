#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/model_scene_controllers.c. These
 * builds keep the North American layout of the unit's small zero-initialized
 * block, so they leave out the European storage (#2602). */
#define EUROPEAN_LANGUAGE_BUILD
#include "../european/model_scene_controllers.c"
