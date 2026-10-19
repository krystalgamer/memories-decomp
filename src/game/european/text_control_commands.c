#include "../../types.h"

/* SLES-03947 build of src/game/text_control_commands.c. The US source is
 * included as is. */
#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define TEXT_BOX_LAYOUT_QUAD_FLAG 0x40
#define TEXT_BOX_LAYOUT_HEIGHT 0xC0

#include "../text_control_commands.c"
