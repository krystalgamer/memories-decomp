#ifndef PAL_OPTIONS_RENDERERS_H
#define PAL_OPTIONS_RENDERERS_H

#include "../../types.h"
#include "../../game/display_object.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

void PalOptions_DrawScrollingStrips(DisplayObject *object, GsOT *ordering_table);
void PalOptions_DrawWaveSprite(DisplayObject *object, GsOT *ordering_table);

#endif
