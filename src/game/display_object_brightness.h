#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_BRIGHTNESS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_BRIGHTNESS_H

#include "display_object.h"

/* Display objects whose first entry receives the shared brightness update. */
extern DisplayObject *D_800EB184[4];

/* The sound and campaign debug editors dim the primary object on entry and
 * restore its neutral RGB brightness on exit. */
void DebugMenu_DimPrimaryDisplayObject(void);
void DebugMenu_RestorePrimaryDisplayObject(void);

#endif
