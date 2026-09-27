#include "../../types.h"

/* SLES-03947 build of src/game/duel_init_model_scene.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_MODEL_SCENE_BUFFER_SIZE 0x65800

#include "../duel_init_model_scene.c"
