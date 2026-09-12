#ifndef MEMORIES_DECOMP_SCRIPT_OP_LOAD_IMAGE_SCENE_H
#define MEMORIES_DECOMP_SCRIPT_OP_LOAD_IMAGE_SCENE_H

#include "../types.h"

/* D_80090C50 handler: enters a script image scene. Resets the viewport pair,
 * releases the three image slots through ScriptImage_ReleaseObjects, then
 * takes a 16-bit
 * image id from the stream -- and when bit 15 is set, two further halfwords
 * that seed the viewport, advancing the cursor past all six bytes. */
void Script_OpLoadImageScene(void);

#endif
