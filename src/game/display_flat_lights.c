#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_flat_lights.h"

/* Initialized data at 0x80090FCC: the two flat lights func_80041340 installs
 * before walking the display object lists.
 *
 * The type is not inferred. func_80041340 passes both to GsSetFlatLight,
 * whose second parameter is a GsF_LIGHT *, so the records are that type by
 * the call rather than by resemblance. Sixteen bytes each, which is what
 * GsF_LIGHT occupies once its three unsigned chars are padded out.
 *
 * The two are a key and a fill: one direction vector points along +y at
 * intensity 0x60, the other along -y at 0x30, and func_80041340 installs the
 * second one in both slots 1 and 2.
 *
 * D_80090FDC's gap to the next name is 0x2C, which is NOT its size. The
 * twenty-eight bytes above it hold seven pointers into func_80041340's own
 * range and carry no name of their own, so they stay in the blob rather than
 * being claimed here. Reading that gap as the size is exactly the failure
 * memory-map.md records. */

GsF_LIGHT D_80090FCC = { 0, 0x1000, 0, 0x60, 0x60, 0x60 };
GsF_LIGHT D_80090FDC = { 0, -0x1000, 0, 0x30, 0x30, 0x30 };
