#include "../../types.h"
#define TEXT_BOX_RUNTIME_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x800F0430-0x800F05D0 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 D_800F0430_sles_03947[0x1A0];

/* SLES-03947 build of src/game/text_box_runtime.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define FUNC_80039794_CHOICE_FLAG 0x10

#include "../text_box_runtime.c"
