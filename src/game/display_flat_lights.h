#ifndef YUGIOH_GAME_DISPLAY_FLAT_LIGHTS_H
#define YUGIOH_GAME_DISPLAY_FLAT_LIGHTS_H

#include "../types.h"
#include "../psyq/libgs.h"

/* The two flat lights at 0x80090FCC, declared with the type GsSetFlatLight
 * takes rather than as byte arrays its caller has to cast: one along +y at
 * 0x60 grey and one along -y at 0x30 grey. The display-object runtime installs
 * them as GsSetFlatLight 0 and 1/2. */
extern GsF_LIGHT gDisplayObject_FlatLightDown;
extern GsF_LIGHT gDisplayObject_FlatLightUp;

#endif
