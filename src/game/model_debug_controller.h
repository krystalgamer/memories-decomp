#ifndef MEMORIES_DECOMP_MODEL_DEBUG_CONTROLLER_H
#define MEMORIES_DECOMP_MODEL_DEBUG_CONTROLLER_H

#include "../types.h"

/* Loads missing scene slots and handles the model-selection debug display.
 * The animated-battle frame uses the completion sentinel return value. */
s32 ModelDebug_UpdateController(void);

extern char gModelDebug_szLoadStageFormat[];
extern char gModelDebug_szLoadModel0Format[];
extern char gModelDebug_szLoadModel1Format[];
extern char gModelDebug_szSlotFlagsFormat[];
extern char gModelDebug_szStageFormat[];

#endif
