#ifndef YUGIOH_GAME_DISPLAY_FLAT_LIGHTS_H
#define YUGIOH_GAME_DISPLAY_FLAT_LIGHTS_H

#include "../types.h"
#include "../psyq/libgs.h"

/* The two flat lights at 0x80090FCC, declared with the type GsSetFlatLight
 * takes rather than as byte arrays its caller has to cast. */
extern GsF_LIGHT D_80090FCC;
extern GsF_LIGHT D_80090FDC;

#endif
