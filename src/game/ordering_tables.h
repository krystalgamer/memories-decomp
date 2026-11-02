#ifndef MEMORIES_DECOMP_ORDERING_TABLES_H
#define MEMORIES_DECOMP_ORDERING_TABLES_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Graphics_BeginFrame publishes the four typed descriptors in the active
 * GraphicsFrameBuffer and clears each through GsClearOt.
 * Renderers select one with DisplayObject.ot_index; the main-menu overlay
 * and duel-effect requests use the same pointers. */
extern GsOT *gGraphics_apOrderingTables[4];

/* Slots are always reached by indexing this array. Because its 16-byte
 * extent is not small data, -G8 readers keep the absolute %hi/%lo form
 * (gGraphics_apOrderingTables+4/+8/+12) rather than a gp-relative load. */

typedef char OrderingTable_descriptor_size_must_be_0x14[
    sizeof(GsOT) == 0x14 ? 1 : -1
];
typedef char OrderingTable_slots_size_must_be_0x10[
    sizeof(gGraphics_apOrderingTables) == 0x10 ? 1 : -1
];

#endif
