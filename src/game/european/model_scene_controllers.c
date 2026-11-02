#include "../../types.h"

/* SLES-03947..51 build of src/game/model_scene_controllers.c: the whole unit
 * in one object. */
#define VERSION_EUROPE
/* As in model_debug_controller's wrapper. */
#define D_8009B004 D_8009BF8C

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define MODEL_INTRO_PACKAGE_START_SECTOR 0x6E7
#define MODEL_TEXTURE_RESET_ROW_X 0x280

#include "../model_scene_controllers.c"
