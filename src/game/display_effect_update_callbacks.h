#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H

#include "../types.h"
#include "menu_record.h"

/* Step 4 rebuilds/crossfades old and new resource rows, with complementary
 * color and position updates; completion retains only the new primary row.
 * Step 5 is the separate portrait transition using only the first two
 * primary-row slots and its original keep-versus-release endpoints.
 * Both retain the MenuRecord callback ABI and all existing row views. */
void DisplayEffect_CrossfadeResources(MenuRecord *record);
void DisplayEffect_UpdatePortraitTransition(MenuRecord *record);

#endif
