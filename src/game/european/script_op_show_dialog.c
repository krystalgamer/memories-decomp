#include "../../types.h"

/* SLES-03947 build of src/game/script_op_show_dialog.c, with its European bodies. */

#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define SCRIPT_DIALOG_BOX_HEIGHT 0x40
#define SCRIPT_DIALOG_BOX_FLAG 0x10

#include "../script_op_show_dialog.c"
