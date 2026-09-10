#ifndef MEMORIES_DECOMP_FUNC_8003A990_H
#define MEMORIES_DECOMP_FUNC_8003A990_H

#include "../types.h"

/* D_80090F68 display effect step: the easing sweep. It runs the record's six
 * tail halfwords -- 0x40 and 0x42 the destination, 0x44 and 0x46 the distance
 * still to travel, 0x48 the eased angle and 0x4A its per-tick step -- and
 * hands the result to the position helpers as x and y.
 *
 * The parameter is still u8 * because that is what the definition says;
 * menu_record.h names every offset it uses, so this is a conversion waiting
 * to happen rather than a considered choice, but changing it belongs with
 * that conversion and not with moving the declaration. */
void func_8003A990(u8 *record);

#endif
