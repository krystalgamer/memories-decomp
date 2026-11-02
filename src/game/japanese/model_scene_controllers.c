#include "../../types.h"

/* SLPM-86398 build of src/game/model_scene_controllers.c. */
#define VERSION_JAPAN
/* The Japanese camera move record has no field_06 (model.h). */
#define MODEL_CAMERA_MOVE_COMPACT_LAYOUT
#include "../model_scene_controllers.c"
