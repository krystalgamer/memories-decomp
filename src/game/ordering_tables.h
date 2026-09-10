#ifndef MEMORIES_DECOMP_ORDERING_TABLES_H
#define MEMORIES_DECOMP_ORDERING_TABLES_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Graphics_BeginFrame publishes four descriptors at buffer + 0x5110,
 * stepping sizeof(GsOT) == 0x14, and clears each through GsClearOt.
 * Renderers select one with DisplayObject.ot_index; the main-menu overlay
 * and duel-effect requests use the same pointers. */
extern GsOT *D_800E9D90[4];

/* Interior relocation symbols for slots 1, 2 and 3, not separate storage.
 * Keep the oversized/unsized views used by the -G8 resident readers:
 * replacing them with scalars changes absolute loads into gp-relative ones.
 * The scalar views belong to -G0 overlay/candidate consumers. */
#ifdef ORDERING_TABLE_SLOT1_ARRAY
extern GsOT *D_800E9D94[4];
#else
extern GsOT *D_800E9D94;
#endif

#ifdef ORDERING_TABLE_SLOT2_ARRAY
extern GsOT *D_800E9D98[];
#else
extern GsOT *D_800E9D98;
#endif
extern GsOT *D_800E9D9C;

typedef char OrderingTable_descriptor_size_must_be_0x14[
    sizeof(GsOT) == 0x14 ? 1 : -1
];
typedef char OrderingTable_slots_size_must_be_0x10[
    sizeof(D_800E9D90) == 0x10 ? 1 : -1
];

#endif
