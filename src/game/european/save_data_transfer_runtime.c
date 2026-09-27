#include "../../types.h"

/* SLES-03947 build of src/game/save_data_transfer_runtime.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define SAVE_DATA_LOAD_BOX_FLAGS 0x1010
#define SAVE_DATA_LOAD_CHANNEL_FLAG 0x10
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009C015

#include "../save_data_transfer_runtime.c"
