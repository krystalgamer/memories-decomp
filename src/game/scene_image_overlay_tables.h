#ifndef MEMORIES_DECOMP_SCENE_IMAGE_OVERLAY_TABLES_H
#define MEMORIES_DECOMP_SCENE_IMAGE_OVERLAY_TABLES_H

#include "../types.h"

/* The two overlay tables func_8002E128 reads when it rebuilds a scene-script
   image record. Declared here so the source that defines them and the one
   that reads them cannot drift apart. */
extern u8 D_80090BA8[];
extern u8 D_80090C00[];

#endif
