#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/model.c. These builds keep the
 * North American layout of the scene controllers' small zero-initialized
 * block, so they leave out the European storage (#2602). */
#define EUROPEAN_LANGUAGE_BUILD
#include "../european/model.c"
