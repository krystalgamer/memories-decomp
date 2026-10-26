#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_BUILD_RESOURCE_OBJECTS_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_BUILD_RESOURCE_OBJECTS_H

#include "../types.h"
#include "display_object.h"
#include "menu_record.h"

/* Builds up to three display objects and writes them back through `out` as
 * out[0], out[1] and out[2], storing NULL in the slots it does not fill. That
 * is why the second parameter is DisplayObject **. The backing MenuRecord
 * grid has mixed consumers, so callers cast each three-word row to this
 * output view. DisplayEffect_CrossfadeResources passes rows 0 through 3, and each call fills its
 * own three-slot group. Returns 1 when it built anything and 0 when
 * DisplayEffect_HasResourceEntry rejected the first index; every caller
 * discards that result.
 *
 * The first parameter is the record itself. menu_record.h already names the
 * three fields this reads -- slot_index selects which of the two buffer bases
 * and which layer constants to use, and field_34/field_36 are the x/y pair
 * that header documents as "the pair DisplayEffect_UpdatePositionTween and DisplayEffect_FadeInRecord hand to
 * the position helpers". They are u16 there and read signed here, so the two
 * reads keep an explicit `*(s16 *)&` at the use.
 *
 * DisplayEffect_UpdateResourceAcquisition in display_effect_resource_setup.c holds the same memory as a
 * DisplayEffectState instead and says so with its cast; menu_record.h records
 * that the two views name these same bytes. */
s32 DisplayEffect_BuildResourceObjects(
    MenuRecord *record, DisplayObject **out, s32 resource_index);

#endif
